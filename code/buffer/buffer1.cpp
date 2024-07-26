/*
 * @Author       : mark
 * @Date         : 2020-06-26
 * @copyleft Apache 2.0
 */ 
#include "buffer1.h"
#include<iostream>
using namespace std;
Buffer::Buffer(int initBuffSize) : buffer_(initBuffSize), readPos_(0), writePos_(0) {}
//初始化，读的位置设置为0，写的位置设置为0

//可以读的字节数
size_t Buffer::ReadableBytes() const {
    return writePos_ - readPos_;
}

//先写后读
//返回可以写的字节数
size_t Buffer::WritableBytes() const {
    return buffer_.size() - writePos_;
}


//前面可以用的空间
size_t Buffer::PrependableBytes() const {
    return readPos_;
}

const char* Buffer::Peek() const {
    return BeginPtr_() + readPos_;
}

void Buffer::Retrieve(size_t len) {                //读的位置向后延len的长度
    assert(len <= ReadableBytes());
    readPos_ += len;
}

void Buffer::RetrieveUntil(const char* end) {         //读的位置指针指向end的位置
    assert(Peek() <= end );
    Retrieve(end - Peek());
}

void Buffer::RetrieveAll() {                    //空间全部收回
    bzero(&buffer_[0], buffer_.size());
    readPos_ = 0;
    writePos_ = 0;
}
std::string Buffer::ToStr(){
    std::string str(Peek(), ReadableBytes());
    return str;
}

std::string Buffer::RetrieveAllToStr() {        //取出所有，并销毁空间，返回最后的str
    std::string str(Peek(), ReadableBytes());
    RetrieveAll();
    return str;
}

const char* Buffer::BeginWriteConst() const {
    return BeginPtr_() + writePos_;
}

char* Buffer::BeginWrite() {
    return BeginPtr_() + writePos_;
}


//写了多少长度，我的writePos_就往后面加
void Buffer::HasWritten(size_t len) {
    writePos_ += len;
} 

void Buffer::Append(const std::string& str) {
    Append(str.data(), str.length());
}

void Buffer::Append(const void* data, size_t len) {
    assert(data);
    Append(static_cast<const char*>(data), len);
}

//Append(buff,len -writable);  buff临时数组，len-writable是临时数组中的数据个数
void Buffer::Append(const char* str, size_t len) {
    assert(str);
    EnsureWriteable(len);
    //吧buff   copy到beginwrite,向前面移动了一下
    std::copy(str, str + len, BeginWrite());
    HasWritten(len);
}

void Buffer::Append(const Buffer& buff) {
    Append(buff.Peek(), buff.ReadableBytes());
}

void Buffer::EnsureWriteable(size_t len) {
    // if(WritableBytes() < len) {//可写的变成了0 ，是小于len
    //     MakeSpace_(len);
    // }
    assert(WritableBytes() >= len);
}

ssize_t Buffer::ReadFd(int fd, int* saveErrno) {
    // char buff[65535];           //65M大小临时数组，保证能够把所有数据全部读出来
    struct iovec iov[1];           
    //缓冲区结构体的数据   struct iovec {  void  *iov_base; 
    // 缓冲区的起始地址  size_t iov_len;  // 缓冲区的长度};
    const size_t writable = WritableBytes();
    /* 分散读， 保证数据全部读完 */
    iov[0].iov_base = BeginPtr_() + writePos_;  //第一个缓冲区就是buffer
    iov[0].iov_len = writable;                 //可以写的长度是   wirtable    
    const ssize_t len = readv(fd, iov, 1);   
    writePos_ += len;        
    cout<<ToStr();
    //readv是分散读取，如果第一个缓冲区满了，自动进入第二个缓冲区里面
    if(len < 0) {
        *saveErrno = errno;
    }
  
    return len;
}

ssize_t Buffer::WriteFd(int fd, int* saveErrno) {
    size_t readSize = ReadableBytes();
    ssize_t len = write(fd, Peek(), readSize);
    if(len < 0) {
        *saveErrno = errno;
        return len;
    } 
    readPos_ += len;
    return len;
}

char* Buffer::BeginPtr_() {
    return &*buffer_.begin();
}
/*&*vector.begin() 的含义是：先通过 begin() 获取到 vector 的第一个元素的迭代器，
然后使用 * 解引用该迭代器得到第一个元素的值，最后使用 & 获取该值的地址。*/

//因此，&*vector.begin() 会得到第一个元素的地址，而 vector.begin() 则得到第一个元素的迭代器。
const char* Buffer::BeginPtr_() const {
    return &*buffer_.begin();
}

//新增加的大小
// void Buffer::MakeSpace_(size_t len) {
//     if(WritableBytes() + PrependableBytes() < len) {
//         buffer_.resize(writePos_ + len + 1);//把数据添加进去
//     } 
    
//     else {
//         //将数据前移，重新拷贝到前面
//         size_t readable = ReadableBytes();
//         std::copy(BeginPtr_() + readPos_, BeginPtr_() + writePos_, BeginPtr_());
//         readPos_ = 0;
//         writePos_ = readPos_ + readable;
//         assert(readable == ReadableBytes());
//     }
// }