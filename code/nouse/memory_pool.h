#ifndef MEMORY_POOL
#define MEMORY_POOL

#include <cstring>   //perror
#include <iostream>
#include <unistd.h>  // write
#include <sys/uio.h> //readv
#include <vector> //readv
#include <atomic>
#include <assert.h>

class Memory_pool{
public:
    Memory_pool(int initBuffSize=65535);
    ~Memory_pool()=default;


    std::string ToStr();
    void RetrieveAll() ;
    std::string RetrieveAllToStr();
    
    void Append(const std::string& str);
    void Append(const char* str, size_t len);
    void Append(const void* data, size_t len);
    // void Append(const Buffer& buff);
    ssize_t ReadFd(int fd,int* Errno);
    ssize_t WriteFd(int fd,int* Errno);

private:
    // char* BeginPr_();
    char* BeginPtr_();                              //开始的指针
    const char* BeginPtr_() const;
    // void MakeSpace_(size_t len);

    // std::atomic<std::size_t> Pos;
    std::vector<char> buffer_;

};




#endif