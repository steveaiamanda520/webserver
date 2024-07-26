#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    // 打开文件以进行写入
    int fd = open("example.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // 写入数据到文件
    const char* buffer = "Hello, world!\n";
    ssize_t bytes_written = write(fd, buffer, strlen(buffer));
    if (bytes_written == -1) {
        perror("write");
        close(fd);
        return 1;
    }

    std::cout << "File written successfully." << bytes_written<<std::endl;

    // 关闭文件
    close(fd);

    // 打开文件以进行读取
    fd = open("example.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // 读取文件内容
    char read_buffer[256];
    ssize_t bytes_read = read(fd, read_buffer, sizeof(read_buffer));
    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return 1;
    }

    // 输出读取到的内容
    std::cout << "Read from file: " << read_buffer;

    // 关闭文件
    close(fd);

    return 0;
}
