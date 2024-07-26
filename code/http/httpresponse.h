/*
 * @Author       : mark
 * @Date         : 2020-06-25
 * @copyleft Apache 2.0
 */ 
#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <unordered_map>
#include <fcntl.h>       // open
#include <unistd.h>      // close
#include <sys/stat.h>    // stat
#include <sys/mman.h>    // mmap, munmap
// #include"../buffer/memory_pool.h"
#include "../buffer/buffer1.h"
#include "../log/log.h"

class HttpResponse {
public:
    HttpResponse();                         //构造函数，用于初始化 HttpResponse 对象。
    ~HttpResponse();                        //析构函数，用于释放 HttpResponse 对象所占用的资源。通常用于清理动态分配的资源，如文件内存映射等。

    void Init(const std::string& srcDir, std::string& path, bool isKeepAlive = false, int code = -1);
    //设置响应所需的资源目录、资源路径、是否保持连接和响应状态码等信息。

    void MakeResponse(Buffer& buff);    //据初始化时设置的信息和请求的状态，构建完整的 HTTP 响应，包括状态行、响应头部和响应体等，并将生成的数据存储在 buff 中。
    void UnmapFile();                   //释放文件内存映射的资源，如果响应中使用了文件的内容作为响应体，需要在不再需要时取消映射。
    char* File();                       //返回当前响应使用的文件内存映射的指针。
    size_t FileLen() const;             //返回当前响应使用的文件的长度，通常用于确定需要发送的数据量。
    void ErrorContent(Buffer& buff, std::string message);   //生成包含错误消息的 HTTP 错误响应，填充到 buff 中。
    int Code() const { return code_; }  //返回当前响应设置的状态码。
    

private:
    void AddStateLine_(Buffer &buff);       //添加状态行到缓冲区。
    void AddHeader_(Buffer &buff);          //添加响应头部到缓冲区。
    void AddContent_(Buffer &buff);         //添加响应体到缓冲区。

    void init_resouce();                //初始化资源。
    void ErrorHtml_();                      //生成默认的 HTML 错误页面。
    std::string GetFileType_();             //获取文件类型。

    int code_;                  //相应状态码
    bool isKeepAlive_;          //是否保持连接

    std::string path_;          //资源的路径
    std::string srcDir_;        //资源的目录

    char* mmFile_;              //文件内存映射的指针
    struct stat mmFileStat_;    //文件的状态信息
    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;      //后缀 - 类型
    static const std::unordered_map<int, std::string> CODE_STATUS;              //状态码 - 描述
    static const std::unordered_map<int, std::string> CODE_PATH;                //状态码 - 路径
    // static std::unordered_map<std::string,void*> RESOURCE_MAP;            //建立映射
    static const std::vector<std::string> RESOURCES;                            //资源名称
};


#endif //HTTP_RESPONSE_H