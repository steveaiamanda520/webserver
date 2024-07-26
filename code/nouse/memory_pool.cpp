#include"memory_pool.h"


Memory_pool::Memory_pool(int initBuffSize){
    buffer_.resize(initBuffSize);
}

std::string Memory_pool::ToStr(){
    std::string str(BeginPtr_(),buffer_.size());
}


void Memory_pool::RetrieveAll(){
    buffer_.clear();
}

std::string Memory_pool::RetrieveAllToStr(){
    ToStr();
    RetrieveAll();
}

void Memory_pool::Append(const std::string& str) {
    Append(str.data(), str.length());
}

void Memory_pool::Append(const void* data, size_t len) {
    assert(data);
    Append(static_cast<const char*>(data), len);
}

void Memory_pool::Append(const char* str, size_t len) {
    assert(str);
    //吧buff   copy到beginwrite
    std::copy(str, str + len, BeginPtr_());
}

char* Memory_pool::BeginPtr_() {
    return &*buffer_.begin();
}

const char* Memory_pool::BeginPtr_() const{
    return &*buffer_.begin();
}

ssize_t Memory_pool::ReadFd(int fd,int* saveErrno){
    struct iovec iov[1];
    iov[0].iov_base=BeginPtr_();
    iov[0].iov_len=buffer_.capacity();
    const ssize_t len=readv(fd,iov,1);

    if(len<0){
        *saveErrno=errno;
    }
    return len;
}

ssize_t Memory_pool::WriteFd(int fd,int* saveErrno){
    size_t readSize=buffer_.size();
    ssize_t len=write(fd,BeginPtr_(),readSize);
    if(len<0){
        *saveErrno=errno;
        return len;
    }
    return len;
}

