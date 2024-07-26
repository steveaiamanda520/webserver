/*
 * @Author       : mark
 * @Date         : 2020-06-17
 * @copyleft Apache 2.0
 */

#include "webserver.h"

using namespace std;

WebServer::WebServer(
            int port, int trigMode, int timeoutMS, bool OptLinger,
            int sqlPort, const char* sqlUser, const  char* sqlPwd,
            const char* dbName, int connPoolNum, int threadNum,
            bool openLog, int logLevel, int logQueSize):
            port_(port), openLinger_(OptLinger), timeoutMS_(timeoutMS), isClose_(false),
            timer_(new HeapTimer()), threadpool_(new ThreadPool(threadNum)), epoller_(new Epoller())
    {
    srcDir_ = getcwd(nullptr, 256);
    ///root/practice/牛客网程序/WebServer-master
    assert(srcDir_);
    strncat(srcDir_, "/resources/", 16);
    ///root/practice/牛客网程序/WebServer-master/resources服务器资源的根路径
    HttpConn::userCount = 0;
    HttpConn::srcDir = srcDir_;
    SqlConnPool::Instance()->Init("localhost", sqlPort, sqlUser, sqlPwd, dbName, connPoolNum);

    //初始化事件的模式
    InitEventMode_(trigMode);
    if(!InitSocket_()) { isClose_ = true;}
    //如果初始化成功了，就继续执行，如果失败了，取了个非

    //日志相关的先不看
    if(openLog) {
        Log::Instance()->init(logLevel, "./log", ".log", logQueSize);
        if(isClose_) { LOG_ERROR("========== Server init error!=========="); }
        else {
            LOG_INFO("========== Server init ==========");
            LOG_INFO("Port:%d, OpenLinger: %s", port_, OptLinger? "true":"false");
            LOG_INFO("Listen Mode: %s, OpenConn Mode: %s",
                            (listenEvent_ & EPOLLET ? "ET": "LT"),
                            (connEvent_ & EPOLLET ? "ET": "LT"));
            LOG_INFO("LogSys level: %d", logLevel);
            LOG_INFO("srcDir: %s", HttpConn::srcDir);
            LOG_INFO("SqlConnPool num: %d, ThreadPool num: %d", connPoolNum, threadNum);
        }
    }
}

WebServer::~WebServer() {
    close(listenFd_);
    isClose_ = true;
    free(srcDir_);
    SqlConnPool::Instance()->ClosePool();
}

//初始化事件的模式
void WebServer::InitEventMode_(int trigMode) {
    listenEvent_ = EPOLLRDHUP;     
    //监听的事件，监听文件描述符要  那个的事件，检测文件描述符，对方有没有正常的关闭                 
    connEvent_ = EPOLLONESHOT | EPOLLRDHUP;         //检测错误的信息
    /*这个标志通常是与Linux中的I/O多路复用机制 epoll 相关的，用于配置 epoll 事件的行为。

    EPOLLONESHOT: 这个标志用于告诉 epoll 系统调用，在触发了某个文件描述符上的事件后，将其从 epoll 队列中删除。
    这意味着在每次 epoll_wait() 调用返回后，如果你希望继续监视这个文件描述符上的事件，你需要重新添加它到 epoll 队列中。
    这在某些情况下是很有用的，比如当你想要确保同一个事件只被处理一次时。

    EPOLLRDHUP: 这个标志表示对端关闭了连接，或者对端发送了一个半关闭的连接。当这个事件发生时，
    它通常表示你需要关闭该连接，因为对方已经不再发送数据了。

    这些标志通常通过位掩码的方式与其他 epoll 事件标志结合使用，例如 EPOLLIN（可读事件）和 EPOLLOUT（可写事件）。
    在实际的代码中，你可以通过使用 | 运算符将它们合并到一个标志中。*/
    switch (trigMode)
    {
    case 0:
        break;
    case 1:
        connEvent_ |= EPOLLET;
        break;
    case 2:
        listenEvent_ |= EPOLLET;
        break;
    case 3:
        listenEvent_ |= EPOLLET;
        connEvent_ |= EPOLLET;
        //两种事件都添加ET模式   epoll的非阻塞是  结合这几种模式去读取的
        break;
    default:
        listenEvent_ |= EPOLLET;
        connEvent_ |= EPOLLET;
        break;
    }
    HttpConn::isET = (connEvent_ & EPOLLET);//判断它里面有没有  epollET的模式         010 & 010  =1    001 & 010 =0
}

void WebServer::Start() {
    int timeMS = -1;  /* epoll wait timeout == -1 无事件将阻塞 */
    if(!isClose_) { LOG_INFO("========== Server start =========="); }
    while(!isClose_) {
        if(timeoutMS_ > 0) {
            timeMS = timer_->GetNextTick();
        }//解决超时连接的

        //调用epoll_wait去检测    返回值就是你检测到有多少个
        int eventCnt = epoller_->Wait(timeMS);
        for(int i = 0; i < eventCnt; i++) {
            /* 处理事件 */
            int fd = epoller_->GetEventFd(i);
            //获取了   文件描述符
            uint32_t events = epoller_->GetEvents(i);
            //获取了检测的事件，如果文件描述符和监听的文件描述符一样，则处理监听的文件描述符
            if(fd == listenFd_) {
                DealListen_();//accept连接新的客户端进来
            }

            //表示连接出现了一些错误，把文件描述符关闭掉
            else if(events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                LOG_INFO("Wrong ,quit")
                assert(users_.count(fd) > 0);
                CloseConn_(&users_[fd]);
            }
            //如果说文件描述符不是监听的，处理读操作
            else if(events & EPOLLIN) {
                LOG_INFO("Deal Read")
                assert(users_.count(fd) > 0);
                DealRead_(&users_[fd]);
            }
            //处理写操作
            else if(events & EPOLLOUT) {
                LOG_INFO("Deal Write")
                assert(users_.count(fd) > 0);
                DealWrite_(&users_[fd]);
            } else {
                LOG_ERROR("Unexpected event");
            }
        }
    }
}

void WebServer::SendError_(int fd, const char*info) {
    assert(fd > 0);
    int ret = send(fd, info, strlen(info), 0);
    if(ret < 0) {
        LOG_WARN("send error to client[%d] error!", fd);
    }
    close(fd);
}

void WebServer::CloseConn_(HttpConn* client) {
    assert(client);
    LOG_INFO("Client[%d] quit!", client->GetFd());
    epoller_->DelFd(client->GetFd());
    client->Close();
}

void WebServer::AddClient_(int fd, sockaddr_in addr) {
    assert(fd > 0);
    users_[fd].init(fd, addr);
    if(timeoutMS_ > 0) {
        timer_->add(fd, timeoutMS_, std::bind(&WebServer::CloseConn_, this, &users_[fd]));
        //文件描述符，超时的时间，以及时间到了以后，要做的行为要去closeConn
    }
    epoller_->AddFd(fd, EPOLLIN | connEvent_); //监听是否有数据到达
    SetFdNonblock(fd);
    LOG_INFO("Client[%d] in!", users_[fd].GetFd());
}

void WebServer::DealListen_() {
    struct sockaddr_in addr;            //保存连接的客户端的信息
    socklen_t len = sizeof(addr);
    do {
        //当没有监听到的时候，直接return，就出去了
        int fd = accept(listenFd_, (struct sockaddr *)&addr, &len);
        if(fd <= 0) { return;}
        else if(HttpConn::userCount >= MAX_FD) {
            SendError_(fd, "Server busy!");
            LOG_WARN("Clients is full!");
            return;
        }
        AddClient_(fd, addr);       //把fd添加到客户端里面请求队列去
    } while(listenEvent_ & EPOLLET);//一次性要把所有的数据都读出来
}

void WebServer::DealRead_(HttpConn* client) {
    assert(client);
    ExtentTime_(client);
    threadpool_->AddTask(std::bind(&WebServer::OnRead_, this, client));//在子线程里面执行onread
}

void WebServer::DealWrite_(HttpConn* client) {
    assert(client);
    ExtentTime_(client);
    threadpool_->AddTask(std::bind(&WebServer::OnWrite_, this, client));//写数据
}

void WebServer::ExtentTime_(HttpConn* client) {
    assert(client);
    if(timeoutMS_ > 0) { timer_->adjust(client->GetFd(), timeoutMS_); }
}

void WebServer::OnRead_(HttpConn* client) {
    assert(client);
    int ret = -1;
    int readErrno = 0;
    ret = client->read(&readErrno);  //读取客户端的数据
    if(ret <= 0 && readErrno != EAGAIN) {
        CloseConn_(client);
        return;
    }
    OnProcess(client);          //处理客户端的请求
}

void WebServer::OnProcess(HttpConn* client) {
    if(client->process()) {
        epoller_->ModFd(client->GetFd(), connEvent_ | EPOLLOUT);//监听是否可写的事件
    } else {
        epoller_->ModFd(client->GetFd(), connEvent_ | EPOLLIN);
    }
}

void WebServer::OnWrite_(HttpConn* client) {
    assert(client);
    int ret = -1;
    int writeErrno = 0;
    ret = client->write(&writeErrno);
    if(client->ToWriteBytes() == 0) {
        /* 传输完成 */
        if(client->IsKeepAlive()) {
            OnProcess(client);
            return;
        }
    }
    else if(ret < 0) {
        if(writeErrno == EAGAIN) {
            /* 继续传输 */
            epoller_->ModFd(client->GetFd(), connEvent_ | EPOLLOUT);
            return;
        }
    }
    CloseConn_(client);
}

/* Create listenFd */
bool WebServer::InitSocket_() {
    int ret;
    struct sockaddr_in addr;        //套接字的地址
    //首先判断端口号，如果大于65535   或者小于1024则失败
    if(port_ > 65535 || port_ < 1024) {
        LOG_ERROR("Port:%d error!",  port_);
        return false;
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port_);
    struct linger optLinger = { 0 };
    if(openLinger_) {
        /* 优雅关闭: 直到所剩数据发送完毕或超时 */
        optLinger.l_onoff = 1;
        optLinger.l_linger = 1;
    }

    //创建一个socket    监听的文件描述符
    listenFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if(listenFd_ < 0) {
        LOG_ERROR("Create socket error!", port_);
        return false;
    }

    //设置   优雅的关闭
    ret = setsockopt(listenFd_, SOL_SOCKET, SO_LINGER, &optLinger, sizeof(optLinger));
    if(ret < 0) {
        close(listenFd_);
        LOG_ERROR("Init linger error!", port_);
        return false;
    }

    int optval = 1;
    /* 端口复用 */
    /* 只有最后一个套接字会正常接收数据。 */
    ret = setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));
    if(ret == -1) {
        LOG_ERROR("set socket setsockopt error !");
        close(listenFd_);
        return false;
    }

    ret = bind(listenFd_, (struct sockaddr *)&addr, sizeof(addr));
    if(ret < 0) {
        LOG_ERROR("Bind Port:%d error!", port_);
        close(listenFd_);
        return false;
    }

    ret = listen(listenFd_, 6);
    if(ret < 0) {
        LOG_ERROR("Listen port:%d error!", port_);
        close(listenFd_);
        return false;
    }
    //有数据到达，说明有客户端连接进来
    ret = epoller_->AddFd(listenFd_,  listenEvent_ | EPOLLIN);
    if(ret == 0) {
        LOG_ERROR("Add listen error!");
        close(listenFd_);
        return false;
    }
    //设置监听文件描述符  非阻塞
    SetFdNonblock(listenFd_);
    LOG_INFO("Server port:%d", port_);
    return true;
}

//如何设置非阻塞，还是下面的这个东西
int WebServer::SetFdNonblock(int fd) {
    assert(fd > 0);

    // int flag=fcntl(fd,F_GETFD,0);
    // flag=flag | O_NONBLOCK;

    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
}


