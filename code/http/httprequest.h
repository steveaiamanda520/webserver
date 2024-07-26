/*
 * @Author       : mark
 * @Date         : 2020-06-25
 * @copyleft Apache 2.0
 */ 
#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <regex>
#include <errno.h>     
#include <mysql/mysql.h>  //mysql

// #include"../buffer/memory_pool.h"
#include "../buffer/buffer1.h"
#include "../log/log.h"
#include "../pool/sqlconnpool.h"
#include "../pool/sqlconnRAII.h"

class HttpRequest {
public:
    enum PARSE_STATE {
        REQUEST_LINE,   //正在解析请求首行
        HEADERS,        //请求头
        BODY,              //请求体
        FINISH,         //解析完成
    };

    enum HTTP_CODE {
        NO_REQUEST = 0,     //没有请求
        GET_REQUEST,        //获取到请求
        BAD_REQUEST,        //错误的请求
        NO_RESOURSE,        //没有资源
        FORBIDDENT_REQUEST,     //禁止访问请求
        FILE_REQUEST,           //请求一个文件
        INTERNAL_ERROR,         //内部错误
        CLOSED_CONNECTION,      //链接关闭
    };
    
    HttpRequest() { Init(); }           //函数作用：初始化HttpRequest对象的状态和成员变量。
    ~HttpRequest() = default;

    void Init();
    bool parse(Buffer& buff);           //函数作用：解析从Buffer对象中读取的HTTP请求内容。

    std::string path() const;       //获取巴拉巴拉
    std::string& path();            //函数作用：获取或设置请求路径。
    std::string method() const;     //函数作用：获取请求方法。返回请求方法（如GET、POST等）。
    std::string version() const;    //函数作用：获取HTTP协议版本。
    std::string GetPost(const std::string& key) const;      //函数作用：根据键获取POST请求中的表单数据。
    std::string GetPost(const char* key) const;

    bool IsKeepAlive() const;       //是否保持alive

    /* 
    todo 
    void HttpConn::ParseFormData() {}
    void HttpConn::ParseJson() {}
    */

public:
    bool ParseRequestLine_(const std::string& line);    //解析请求首行
    void ParseHeader_(const std::string& line); //解析请求头
    void ParseBody_(const std::string& line);   //解析请求体

    void ParsePath_();          //解析请求的路径
    void ParsePost_();          //解析post请求
    void ParseFromUrlencoded_();    //解析表单的数据    函数作用：解析POST请求中的表单数据。

    static bool UserVerify(const std::string& name, const std::string& pwd, bool isLogin);
    //验证用户登录

    PARSE_STATE state_; //解析的状态
    std::string method_, path_, version_, body_;    //请求方法、请求路径、协议版本和请求体
    std::unordered_map<std::string, std::string> header_;   //请求头
    std::unordered_map<std::string, std::string> post_;     //post请求表单数据

    static const std::unordered_set<std::string> DEFAULT_HTML;  //默认的网页
    static const std::unordered_map<std::string, int> DEFAULT_HTML_TAG; 
    
    static int ConverHex(char ch);  //转换成16进制  函数作用：解析表单数据的URL编码格式。
};
// header_

// 类型：std::unordered_map<std::string, std::string>
// 作用：存储HTTP请求的头部信息。
// 详细解释：HTTP请求报文由请求行、请求头部和请求体组成。header_ 用于存储解析后的请求头部信息，
//每个请求头部字段（如Host、User-Agent、Accept等）作为键，对应的值（即请求头部字段的值）作为映射值。
//通过这个 std::unordered_map 可以方便地查找和操作各个请求头字段。
// post_

// 类型：std::unordered_map<std::string, std::string>
// 作用：存储HTTP POST请求的表单数据。
// 详细解释：对于HTTP POST请求，客户端可以通过表单提交数据。这些数据通常以键值对的形式传递，
//例如通过 HTML 表单的 POST 方法提交的数据。post_ 成员变量用于存储这些表单数据，键是表单字段的名称，
//值是对应的字段值。在处理 POST 请求时，可以通过 post_ 成员变量获取表单数据，以便后续的处理和操作。

// 在给定的代码片段中，DEFAULT_HTML 和 DEFAULT_HTML_TAG 是 HttpRequest 类的两个静态成员变量，
//它们分别是 std::unordered_set<std::string> 和 std::unordered_map<std::string, int> 类型，具体作用如下：

// DEFAULT_HTML

// 类型：std::unordered_set<std::string>
// 作用：存储默认的网页路径。
// 详细解释：在很多Web服务器的设计中，当用户请求的路径为空或者是一个目录时，通常会返回一个默认的网页，
//比如主页 index.html 或者 index.htm。DEFAULT_HTML 这个静态成员变量用来存储这些默认网页的路径。使用 
//std::unordered_set 的原因可能是为了快速查找，因为对于默认的网页路径，我们通常只需判断是否存在即可，不需要按顺序访问。
// DEFAULT_HTML_TAG

// 类型：std::unordered_map<std::string, int>
// 作用：存储默认网页的标签。
// 详细解释：在处理 HTTP 请求时，服务器可能需要根据请求的路径来确定返回的内容类型或状态。DEFAULT_HTML_TAG 
//这个静态成员变量将默认网页路径与一个整数状态码进行关联，这个状态码可能表示默认网页的状态或类型，比如是普通网页（200）、
//未找到（404）、禁止访问（403）等。通过这种映射关系，可以快速地根据请求的路径确定默认网页的状态或类型。

#endif //HTTP_REQUEST_H


// Init()

// 函数作用：初始化HttpRequest对象的状态和成员变量。
// 详细解释：该函数在类的构造函数中调用，用于将HttpRequest对象初始化为一个初始状态，清空各个成员变量，准备解析新的HTTP请求。
// parse(Buffer& buff)

// 函数作用：解析从Buffer对象中读取的HTTP请求内容。
// 参数：buff是一个Buffer对象，包含了待解析的HTTP请求内容。
// 返回值：解析成功返回true，否则返回false。
// 详细解释：该函数根据HTTP协议的规范，从Buffer中解析出请求行、请求头部和请求体等内容，并根据解析结果更新HttpRequest对象的成员变量，如请求方法(method_)、请求路径(path_)、协议版本(version_)、请求头(header_)、请求体(body_)等。
// path() const, path()

// 函数作用：获取或设置请求路径。
// 返回值：const版本返回请求路径的常量引用，非const版本返回请求路径的引用。
// method() const

// 函数作用：获取请求方法。
// 返回值：返回请求方法（如GET、POST等）。
// version() const

// 函数作用：获取HTTP协议版本。
// 返回值：返回HTTP协议版本号。
// GetPost(const std::string& key) const, GetPost(const char* key) const

// 函数作用：根据键获取POST请求中的表单数据。
// 参数：key是要获取的表单字段的键。
// 返回值：返回对应键的表单值。
// IsKeepAlive() const

// 函数作用：判断HTTP请求是否需要保持长连接。
// 返回值：如果请求头中包含Connection: keep-alive，则返回true；否则返回false。
// ParseRequestLine_(const std::string& line)

// 函数作用：解析HTTP请求首行。
// 参数：line是请求首行的字符串。
// 返回值：解析成功返回true，否则返回false。
// ParseHeader_(const std::string& line)

// 函数作用：解析HTTP请求头部。
// 参数：line是请求头部的一行字符串。
// ParseBody_(const std::string& line)

// 函数作用：解析HTTP请求体。
// 参数：line是请求体的一行字符串。
// ParsePath_()

// 函数作用：解析HTTP请求的路径。
// ParsePost_()

// 函数作用：解析POST请求中的表单数据。
// ParseFromUrlencoded_()

// 函数作用：解析表单数据的URL编码格式。
// ConverHex(char ch)

// 函数作用：将字符转换成十六进制的数字。
// 参数：ch是待转换的字符。
// 返回值：返回字符对应的十六进制数字。