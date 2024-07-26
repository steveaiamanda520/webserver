
#include <iostream>

// int main() {
//     // 实例化一个 Buffer 对象
//     Buffer buffer(1024);

//     // 向缓冲区中添加一些数据
//     buffer.Append("Hello, ");
//     buffer.Append("world!");

//     // 读取缓冲区中的数据并输出
//     std::cout << "Readable bytes in buffer: " << buffer.ReadableBytes() << std::endl;
//     std::cout << "Buffer content: " << buffer.Peek() << std::endl;

//     // 清空缓冲区
//     buffer.RetrieveAll();

//     // 输出清空后的状态
//     std::cout << "After clearing, Readable bytes in buffer: " << buffer.ReadableBytes() << std::endl;

//     return 0;
// }


#include <iostream>
#include <sys/uio.h> // 包含 readv 函数所需的头文件
#include <fcntl.h>
int main() {
    int fd; // 用于打开文件的文件描述符
    int saveErrno; // 保存错误号的变量

    // 打开文件描述符并进行错误检查
    if ((fd = open("example.txt", O_RDONLY)) < 0) {
        std::cerr << "Failed to open file." << std::endl;
        return -1;
    }

    Buffer buffer;
    ssize_t readBytes = buffer.ReadFd(fd, &saveErrno); // 调用 ReadFd 函数
    ssize_t witrBytes = buffer.WriteFd(fd,&saveErrno);

    if (readBytes < 0) {
        std::cerr << "Failed to read file. Error number: " << saveErrno << std::endl;
    } else {
        std::cout << "Read " << readBytes << " bytes from file." << std::endl;
        std::cout << "Wirte " << witrBytes << " bytes from file." << std::endl;
    }

    close(fd); // 关闭文件描述符

    return 0;
}
