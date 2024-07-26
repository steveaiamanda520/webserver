// #include "http/httprequest.h"
// #include <iostream>

// void TestHttpRequestParsing() {
//     // 创建一个 HttpRequest 对象
//     HttpRequest req;

//     // 假设有一个包含 HTTP 请求数据的 Buffer 对象
//     Buffer buff;
//     buff.Append("POST /register HTTP/1.1\r\n");
//     buff.Append("Host: localhost\r\n");
//     buff.Append("Content-Length: 29\r\n");
//     buff.Append("Content-Type: application/x-www-form-urlencoded\r\n");
//     buff.Append("\r\n");
//     buff.Append("username=test&password=123456\r\n");

//     // 解析 HTTP 请求
//     if (req.parse(buff)) {
//         // 输出解析后的信息
//         std::cout << "Method: " << req.method() << std::endl;
//         std::cout << "Path: " << req.path() << std::endl;
//         std::cout << "Version: " << req.version() << std::endl;
//         std::cout << "Is Keep-Alive: " << (req.IsKeepAlive() ? "true" : "false") << std::endl;
//         std::cout << "Username: " << req.GetPost("username") << std::endl;
//         std::cout << "Password: " << req.GetPost("password") << std::endl;
//     } else {
//         std::cerr << "Failed to parse HTTP request!" << std::endl;
//     }
// }

// int main() {
//     TestHttpRequestParsing();
//     return 0;
// }

#include <iostream>
#include <cassert>
#include "http/httprequest.h"  // 假设这里是 HttpRequest 类的头文件，包含了上述定义的类和函数

int main() {
    // 模拟一个 HTTP 请求的请求体（假设是 URL 编码的表单数据）
    std::string requestBody = "key1=value1&key2=value2+with+spaces&key3=hello%20world";

    // 创建一个模拟的 HttpRequest 对象
    HttpRequest request;
    request.body_ = requestBody;  // 设置请求体为模拟的请求体

    // 调用解析函数进行解析
    request.ParseFromUrlencoded_();

    // 输出解析后的结果，这里假设 HttpRequest 类有一个输出函数或者使用日志输出
    std::cout << "Parsed POST data:" << std::endl;
    for (const auto& pair : request.post_) {
        std::cout << pair.first << " = " << pair.second << std::endl;
    }

    return 0;
}
