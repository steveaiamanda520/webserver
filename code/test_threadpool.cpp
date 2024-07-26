
//测试： std::queue<std::function<void()>> tasks;这个的用法
// #include <iostream>
// #include <functional>
// #include <queue>

// // 示例函数1
// void foo() {
//     std::cout << "foo() is called." << std::endl;
// }

// // 示例函数2
// void bar(int x) {
//     std::cout << "bar(" << x << ") is called." << std::endl;
// }

// int main() {
//     // 创建一个存储函数对象的队列
//     std::queue<std::function<void()>> tasks;

//     // 将函数对象推入队列
//     tasks.push(foo);
//     tasks.push(std::bind(bar, 42)); // 使用 std::bind 绑定参数
//     // tasks.push(bar(42));

//     // 依次执行队列中的函数对象
//     while (!tasks.empty()) {
//         auto task = tasks.front();
//         tasks.pop();
//         task(); // 调用函数对象
//     }

//     return 0;
// }


//测试std::shared_ptr   实例化并测试
// #include <iostream>
// #include <memory>

/*std::shared_ptr 是 C++11 中引入的智能指针之一，它用于管理动态分配的对象。它的主要用途包括以下几个方面：

资源管理：std::shared_ptr 可以用来管理动态分配的对象，确保在不再需要时正确释放内存，从而避免内存泄漏。

共享所有权：与 std::unique_ptr 不同，std::shared_ptr 允许多个指针共享对同一对象的所有权。这意味着可以在多个地方使用同一个 std::shared_ptr 指向同一个对象，并且当所有 std::shared_ptr 对象超出作用域时，对象才会被销毁。

避免悬空指针：std::shared_ptr 使用引用计数来跟踪对象的引用数，当引用计数变为零时，对象被销毁。这意味着不会出现悬空指针（即指向已销毁对象的指针）的情况。

传递所有权：std::shared_ptr 可以通过 std::move 函数来传递所有权，从而允许对象的所有权在不同的 std::shared_ptr 之间转移。

循环引用：std::shared_ptr 对象可以处理循环引用问题，即两个对象相互持有对方的 std::shared_ptr 指针，导致对象无法被销毁的情况。当存在循环引用时，std::shared_ptr 会通过引用计数机制自动处理，确保对象能够被正确地销毁。

总之，std::shared_ptr 提供了一种方便且安全地管理动态分配的对象的方式，使得程序员能够更轻松地处理内存管理问题，同时提高代码的可维护性和安全性*/
// 示例类 Pool
// class Pool {
// public:
//     Pool() {
//         std::cout << "Pool created." << std::endl;
//     }
//     ~Pool() {
//         std::cout << "Pool destroyed." << std::endl;
//     }
//     void someFunction() {
//         std::cout << "Some function of Pool is called." << std::endl;
//     }
// };

// int main() {
//     // 实例化一个 std::shared_ptr<Pool>
//     std::shared_ptr<Pool> poolPtr(new Pool());

//     // 调用 Pool 类的成员函数
//     poolPtr->someFunction();

//     // poolPtr 离开作用域时，Pool 对象会被自动销毁

//     return 0;
// }


// #include <iostream>
// #include <mutex>
// #include <thread>

// class Pool {
// public:
//     std::mutex mtx; // 互斥量

//     void someFunction() {
//         std::lock_guard<std::mutex> locker(mtx); // 锁定互斥量
//         std::cout << "Inside someFunction()" << std::endl;
//         // 在这里进行线程安全的操作
//     }
// };

// int main() {
//     Pool pool;

//     // 创建两个线程，同时调用 Pool 类的成员函数
//     std::thread t1([&pool]() {
//         pool.someFunction();
//     });
//     std::cout << "********************" << std::endl;

//     std::thread t2([&pool]() {
//         pool.someFunction();
//     });
//     std::cout << "********************" << std::endl;

//     t1.join();
//     std::cout << "********************" << std::endl;
//     t2.join();

//     return 0;
// }

// #include <iostream>
// #include <thread>
// #include <mutex>

// std::mutex mtx;

// void threadFunction() {
//     std::lock_guard<std::mutex> locker(mtx); // 在函数内部上锁
//     std::cout << "Thread is running" << std::endl;
// }

// int main() {
//     std::thread t(threadFunction);

//     // 在主线程中做一些其他的事情
//     {
//         std::lock_guard<std::mutex> locker(mtx); // 在作用域内上锁
//         std::cout << "Main thread is doing something..." << std::endl;
//     } // 在作用域结束时自动解锁

//     t.join();

//     return 0;
// }

// #include <iostream>
// #include <thread>
// #include <mutex>

// std::mutex mtx;

// void threadFunction1(int id) {
    
//     std::lock_guard<std::mutex> locker(mtx); 
//     std::cout << "Thread " << id << " acquired lock" << std::endl;
//     // mtx.unlock(); // 释放锁
// }
// void threadFunction2(int id) {
//     // std::chrono::seconds(1);
//     std::this_thread::sleep_for(std::chrono::seconds(1));
//     std::unique_lock<std::mutex> locker(mtx); 
//     std::cout << "Thread " << id << " acquired lock" << std::endl;
//     // mtx.unlock(); // 释放锁
// }

// int main() {
//     std::thread t2(threadFunction2, 2);
//     std::thread t1(threadFunction1, 1);
    

//     t1.join();
//     t2.join();

//     return 0;
// }




//std::move   的用法
/*移动语义的主要好处在于提高程序的性能和效率。具体来说，移动操作相比拷贝操作有以下几个优势：

减少资源开销：移动操作通常比拷贝操作更高效，特别是对于动态分配的内存、大型数据结构或者资源（如文件句柄、网络连接等）。移动可以直接将资源的所有权从一个对象转移到另一个对象，而无需复制数据，从而节省了时间和内存开销。

避免不必要的复制：在一些情况下，对象的拷贝操作可能是不必要的，例如临时对象的创建、返回值优化（RVO）等。通过使用移动语义，可以避免不必要的复制，从而提高程序的效率。

提高容器性能：在使用容器（如 std::vector、std::string、std::unique_ptr 等）存储大量对象时，移动语义可以显著提高容器的性能。在插入、删除或者重新分配容器中的元素时，移动语义可以避免不必要的数据复制，提高了容器的操作效率。

支持资源所有权转移：移动语义允许将资源的所有权从一个对象转移到另一个对象，这在实现资源管理和转移语义时非常有用。例如，可以在拥有动态分配内存的对象之间高效地传递所有权，而不需要手动管理内存释放的细节。

总之，移动语义可以有效地减少不必要的数据复制和资源开销，提高程序的性能和效率，特别是在处理大型数据结构或者资源管理时。*/
// #include <iostream>
// #include <string>
// #include <utility>

// class MyClass {
// public:
//     MyClass(const std::string& str) : data(str) {
//         std::cout << "Constructor called." << std::endl;
//     }

//     MyClass(const MyClass& other) : data(other.data) {
//         std::cout << "Copy constructor called." << std::endl;
//     }

//     MyClass(MyClass&& other) noexcept : data(std::move(other.data)) {
//         std::cout << "Move constructor called." << std::endl;
//     }

// private:
//     std::string data;
// };

// int main() {
//     std::string str = "Hello";
//     MyClass obj1(str); // 调用构造函数

//     MyClass obj2(std::move(obj1)); // 调用移动构造函数
//     // 这里 obj1 不再有效

//     return 0;
// }





// #include <iostream>
// #include <thread>
// #include <mutex>
// #include <condition_variable>

// std::mutex mtx;
// std::condition_variable cv;
// bool dataReady = false;

// void producer() {
//     std::this_thread::sleep_for(std::chrono::seconds(1)); // 模拟生产者准备数据
//     {
//         std::cout<<"++++++++++++"<<std::endl;
//         std::lock_guard<std::mutex> lk(mtx);
//         std::cout<<"dhwaidhawi"<<std::endl;
//         dataReady = true;
//     }
//     cv.notify_one(); // 通知等待的消费者
// }

// void consumer() {
//     std::unique_lock<std::mutex> lk(mtx);
//     std::cout<<"***************"<<std::endl;
//     cv.wait(lk, [] { return dataReady; }); // 等待数据准备就绪
//     std::cout << "Data is ready for processing." << std::endl;
// }

// int main() {
//     std::thread producerThread(producer);
//     std::thread consumerThread(consumer);

//     producerThread.join();
//     consumerThread.join();

//     return 0;
// }

//cv.wait() 函数在等待条件变量时会自动释放互斥量的锁
/*在 C++ 的标准库中，互斥锁（std::mutex）的所有权是排他的，也就是说，在任意时刻只能有一个线程拥有互斥锁的所有权。一旦一个线程获得了互斥锁的所有权，
其他线程必须等待该线程释放互斥锁后才能获得所有权。

在同一时刻，只能有一个线程拥有互斥锁的所有权，这是互斥锁的基本特性之一，用于确保在同一时间内只有一个线程可以访问共享资源，从而防止数据竞争和并发访问问题。

因此，两个线程不能同时拥有互斥锁的所有权。如果一个线程已经获得了互斥锁的所有权，那么其他线程必须等待该线程释放锁后才能获取。*/



/*producer 函数模拟数据的生产过程，它会等待一段时间后将 dataReady 设置为 true，然后通过条件变量 cv 通知消费者。
consumer 函数是消费者线程的入口函数，它会等待条件变量的通知，一旦收到通知，就会输出数据准备就绪的信息。

这里使用了 std::unique_lock 来锁定互斥量 mtx，并传递给 cv.wait 函数，以等待条件变量的通知。
cv.wait 函数会在等待期间释放锁，并在收到通知后重新获取锁。这样可以防止在等待期间其他线程无法访问互斥量，
从而提高了程序的并发性能。

在 main 函数中，创建了一个生产者线程和一个消费者线程，它们分别执行生产者和消费者的逻辑，最后等待线程执行结束。*/


// #include <mutex>
// #include <condition_variable>
// #include <queue>
// #include <thread>
// #include <functional>
// #include<assert.h>
// #include<iostream>
// #include <chrono> // For std::chrono::milliseconds
// class ThreadPool {
// public:
//     explicit ThreadPool(size_t threadCount = 8): pool_(std::make_shared<Pool>()) {
//             assert(threadCount > 0);
//             for(size_t i = 0; i < threadCount; i++) {
//                 std::thread([pool = pool_] {
//                     std::unique_lock<std::mutex> locker(pool->mtx);
//                     while(true) {
//                         if(!pool->tasks.empty()) {
//                             auto task = std::move(pool->tasks.front());
//                             pool->tasks.pop();
//                             locker.unlock();
//                             task();
//                             locker.lock();
//                         } 
//                         else if(pool->isClosed) break;
//                         else pool->cond.wait(locker);
//                     }
//                 }).detach();
//             }
//     }

//     ThreadPool() = default;

//     ThreadPool(ThreadPool&&) = default;
    
//     ~ThreadPool() {
//         if(static_cast<bool>(pool_)) {
//             {
//                 std::lock_guard<std::mutex> locker(pool_->mtx);
//                 pool_->isClosed = true;
//             }
//             pool_->cond.notify_all();
//         }
//     }

//     template<class F>
//     void AddTask(F&& task) {
//         {
//             std::lock_guard<std::mutex> locker(pool_->mtx);
//             pool_->tasks.emplace(std::forward<F>(task));
//         }
//         pool_->cond.notify_one();
//     }

// private:
//     struct Pool {
//         std::mutex mtx;
//         std::condition_variable cond;
//         bool isClosed;
//         std::queue<std::function<void()>> tasks;
//     };
//     std::shared_ptr<Pool> pool_;
// };


// void thread_i(int i){
//     std::cout<<"thread ID "<<i<<std::endl;
// }


// int main() {
//     ThreadPool pool(4); // 创建具有 4 个线程的线程池

//     // 添加任务到线程池
//     for (int i = 0; i < 8; ++i) {
//         pool.AddTask([i]() {
//             std::cout << "Task " << i << " is executing in thread: " << std::this_thread::get_id() << std::endl;
//             std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 模拟任务执行时间
//         });
//     }

//     // 等待一段时间，确保所有任务都执行完毕
//     std::this_thread::sleep_for(std::chrono::seconds(1));

//     return 0;
// }
/*std::shared_ptr 是一个智能指针，用于管理动态分配的资源，并在适当的时候自动释放这些资源。
std::make_shared 是一个函数模板，用于创建指向动态分配的对象的 std::shared_ptr。它们之间的主要区别如下：

使用方式：

std::shared_ptr：直接使用构造函数或者拷贝构造函数来创建智能指针对象。
std::make_shared：通过调用 std::make_shared 函数模板来创建智能指针对象。
效率：

std::make_shared 通常比直接构造 std::shared_ptr 对象更高效。这是因为 std::make_shared 
在单个堆分配中同时分配控制块和对象内存，从而减少了动态内存分配的开销，并且减少了引用计数的开销。
异常安全性：

std::make_shared 提供了更强的异常安全性。如果在动态内存分配或构造对象时抛出异常，std::make_shared 
可以确保在此过程中没有资源泄漏，因为它在一次动态内存分配中同时分配控制块和对象内存，并将它们放在同一个资源管理块中。
而直接构造 std::shared_ptr 对象时，如果在动态内存分配后、但对象构造之前抛出异常，则可能导致资源泄漏。
综上所述，推荐优先使用 std::make_shared 来创建 std::shared_ptr 对象，因为它更加高效和安全。*/


#include <iostream>
#include <memory> // For std::shared_ptr, std::make_shared

class MyClass {
public:
    MyClass(int value) : value_(value) {
        std::cout << "Constructor called. Value: " << value_ << std::endl;
    }

    ~MyClass() {
        std::cout << "Destructor called. Value: " << value_ << std::endl;
    }

    void SetValue(int value) {
        value_ = value;
    }

    int GetValue() const {
        return value_;
    }

private:
    int value_;
};

int main() {
    // 使用 std::make_shared 创建对象并封装在 std::shared_ptr 中
    auto ptr1 = std::make_shared<MyClass>(42);
    
    // 创建第二个 std::shared_ptr，与第一个共享相同的对象
    auto ptr2 = ptr1;

    // 输出第一个 shared_ptr 指向的对象的值
    std::cout << "Value of ptr1: " << ptr1->GetValue() << std::endl;

    // 修改第二个 shared_ptr 指向的对象的值
    ptr2->SetValue(100);

    // 输出第一个 shared_ptr 指向的对象的值
    std::cout << "Value of ptr1: " << ptr1->GetValue() << std::endl;

    // 在 main 函数结束时，ptr1 和 ptr2 指向的对象会自动销毁

    return 0;
}
