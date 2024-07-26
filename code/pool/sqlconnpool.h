/*
 * @Author       : mark
 * @Date         : 2020-06-16
 * @copyleft Apache 2.0
 */ 
#ifndef SQLCONNPOOL_H
#define SQLCONNPOOL_H

#include <mysql/mysql.h>
#include <string>
#include <queue>
#include <mutex>
#include <semaphore.h>
#include <thread>
#include "../log/log.h"

class SqlConnPool {
public:
    static SqlConnPool *Instance(); //提供的公共的方法

    MYSQL *GetConn();
    void FreeConn(MYSQL * conn);    //释放一个数据库链接，把他放到那个池子里面
    int GetFreeConnCount();         //获取空闲的用户的数量

    void Init(const char* host, int port,
              const char* user,const char* pwd, 
              const char* dbName, int connSize);//初始化，主机名，端口，用户名，密码，数据库名，和大小
    void ClosePool();

private:
    SqlConnPool();
    ~SqlConnPool();
    //创建一大堆的链接对象，   用户来了就可以直接用
    int MAX_CONN_;  //最大的连接数
    int useCount_;  //当前用户数
    int freeCount_; //空闲的用户数

    std::queue<MYSQL *> connQue_;       //队列，（MYSQL）的指针，保存在里面，
    std::mutex mtx_;                    //互斥锁
    sem_t semId_;                       //信号量
};


#endif // SQLCONNPOOL_H