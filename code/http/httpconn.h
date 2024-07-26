/*
 * @Author       : mark
 * @Date         : 2020-06-15
 * @copyleft Apache 2.0
 */ 

#ifndef HTTP_CONN_H
#define HTTP_CONN_H

#include <sys/types.h>
#include <sys/uio.h>     // readv/writev
#include <arpa/inet.h>   // sockaddr_in
#include <stdlib.h>      // atoi()
#include <errno.h>      

#include "../log/log.h"
#include "../pool/sqlconnRAII.h"
#include "../buffer/buffer1.h"
// #include"../buffer/memory_pool.h"
#include "httprequest.h"
#include "httpresponse.h"

class HttpConn {
public:
    HttpConn(); //构造函数，用于初始化 HttpConn 对象。

    ~HttpConn();    //析构函数，用于释放 HttpConn 对象所占用的资源。

    void init(int sockFd, const sockaddr_in& addr); //参数：sockFd 是套接字文件描述符，addr 是连接的地址信息。功能：初始化 HTTP 连接。

    ssize_t read(int* saveErrno);       //读取套接字数据到 readBuff_ 缓冲区中，处理完整的 HTTP 请求数据。

    ssize_t write(int* saveErrno);          //向连接中写入数据。

    void Close();           //关闭当前 HTTP 连接，释放相关资源，关闭套接字。

    int GetFd() const;          //获取套接字文件描述符。

    int GetPort() const;        //获取连接的端口号。

    const char* GetIP() const;  //获取连接的 IP 地址。
    
    sockaddr_in GetAddr() const;    //获取连接的地址信息。
    
    bool process();             //根据接收到的 HTTP 请求，调用 request_ 的方法解析请求，生成相应的 response_，并填充到 writeBuff_ 中准备发送。

    int ToWriteBytes() {            //计算当前 iov_ 数组中第一个和第二个元素的长度之和，用于确定发送数据的大小。
        return iov_[0].iov_len + iov_[1].iov_len; 
    }

    bool IsKeepAlive() const {          //调用 request_ 对象的 IsKeepAlive() 方法，判断当前 HTTP 请求是否要求保持连接。
        return request_.IsKeepAlive();
    }

    static bool isET;                           //静态布尔变量，可能用于指示是否启用边缘触发模式。               
    static const char* srcDir;                 //静态常量指针，表示资源的目录。
    static std::atomic<int> userCount;      //总共的客户端的连接数   客户端连接进来就+1，客户端断开就减1,静态原子整数，用于记录当前客户端连接数。
    
private:
   
    int fd_;                                //存储套接字文件描述符，用于标识当前 HTTP 连接的套接字。     
    struct  sockaddr_in addr_;              //存储连接的地址信息，包括 IP 地址和端口号。

    bool isClose_;                          //标识当前连接是否已关闭。
    
    int iovCnt_;                            //iov_ 数组的元素个数，用于在数据发送时确定有效数据的数量。
    struct iovec iov_[2];                   //用于进行数据的分散/聚集 I/O 操作，通常在进行数据发送时使用，表示发送数据的缓冲区。
    
    Buffer readBuff_; // 读缓冲区,保存请求数据的内容
    Buffer writeBuff_; // 写缓冲区，保存相应数据的内容

    HttpRequest request_;                   //用于处理当前 HTTP 连接接收到的请求。包括解析请求、处理请求参数等操作。
    HttpResponse response_;                 //用于构建和管理当前 HTTP 连接发送的响应。包括设置响应状态、设置响应头部、构建响应体等操作。
};


#endif //HTTP_CONN_H