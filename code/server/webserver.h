/*
 * @Author       : mark
 * @Date         : 2020-06-17
 * @copyleft Apache 2.0
 */ 
#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <unordered_map>
#include <fcntl.h>       // fcntl()
#include <unistd.h>      // close()
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "epoller.h"
#include "../log/log.h"
#include "../timer/heaptimer.h"
#include "../pool/sqlconnpool.h"
#include "../pool/threadpool.h"
#include "../pool/sqlconnRAII.h"
#include "../http/httpconn.h"

class WebServer {
public:
    WebServer(
        int port, int trigMode, int timeoutMS, bool OptLinger, /* 端口 ET模式 timeoutMs 优雅退出  */
        int sqlPort, const char* sqlUser, const  char* sqlPwd, /* Mysql配置 */
        const char* dbName, int connPoolNum, int threadNum,            /* 连接池数量 线程池数量 日志开关 日志等级 日志异步队列容量 */
        bool openLog, int logLevel, int logQueSize);        

    ~WebServer();
    void Start();

private:
    bool InitSocket_(); 
    void InitEventMode_(int trigMode);
    void AddClient_(int fd, sockaddr_in addr);
  
    void DealListen_();
    void DealWrite_(HttpConn* client);
    void DealRead_(HttpConn* client);

    void SendError_(int fd, const char*info);
    void ExtentTime_(HttpConn* client);
    void CloseConn_(HttpConn* client);

    void OnRead_(HttpConn* client);
    void OnWrite_(HttpConn* client);
    void OnProcess(HttpConn* client);

    static const int MAX_FD = 65536;        //最大的文件描述符的个数

    static int SetFdNonblock(int fd);       //设置文件描述符非阻塞

    int port_;          //端口
    bool openLinger_;       //是否打开，优雅关闭
    int timeoutMS_;  /* 毫秒MS */
    bool isClose_;          //是否关闭
    int listenFd_;          //监听的文件描述符
    char* srcDir_;          //资源的目录
    
    uint32_t listenEvent_;      //监听的文件描述符的事件
    uint32_t connEvent_;        //链接的文件描述符的事件
   
    std::unique_ptr<HeapTimer> timer_;          //定时器
    std::unique_ptr<ThreadPool> threadpool_;    //线程池
    std::unique_ptr<Epoller> epoller_;          //epoll对象
    std::unordered_map<int, HttpConn> users_;   //保存的是客户端连接的信息    文件描述符是int   客户信息是HttpConn
    //每一个链接int 对应一个http  ，http->httprequest  ->httpresponse 
};


#endif //WEBSERVER_H