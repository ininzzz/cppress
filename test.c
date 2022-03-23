// #include<pthread.h>
// #include<unistd.h>
// #include<iomanip>
// #include<iostream>
// #include<mutex>
// #include<thread>
// #include<condition_variable>
// #include<queue>
// #include<chrono>
// #include<future>
// #include<algorithm>

// using namespace std;

// // pthread_create和pthread_self
// void test1() {
//     // 创建的线程...
//     auto func = [](void *)->void * {
//         std::cout << std::hex << "my tid: " << pthread_self() << std::endl;
//         std::cout << std::dec << "my pid: " << getpid() << std::endl;
//         return ((void *)0);
//     };
    
//     // 主线程
//     std::cout << std::hex << "my tid: " << pthread_self() << std::endl;
//     std::cout << std::dec << "my pid: " << getpid() << std::endl;
//     pthread_t newthread;
//     // pthread_create创建线程，创建成功返回0，否则返回err
//     int err = pthread_create(&newthread, NULL, func, NULL);
//     pthread_join(newthread, NULL);
// }
// // pthread_join
// void test2() {
//     auto func1 = [](void *)->void * {
//         std::cout << "thread1 running...\n";
//         return (void *)1;
//     };
//     auto func2 = [](void *)->void * {
//         std::cout << "thread2 exiting...\n";
//         pthread_exit((void *)2);
//     };
//     pthread_t tid1, tid2;
//     void *ret;
//     pthread_create(&tid1, NULL, func1, NULL);
//     pthread_create(&tid2, NULL, func2, NULL);
//     // pthread_join获取返回值
//     pthread_join(tid1, &ret);
//     printf("thead1 exit with code %lld\n", (long long)ret);
//     pthread_join(tid2, &ret);
//     printf("thead2 exit with code %lld\n", (long long)ret);
// }
// // pthraed_join返回结构体
// struct foo {
//     int a, b, c;
// };
// void printfoo(const char *s, const foo *fp) {
//     printf("%s\n", s);
//     printf("foo.a = %d\n", fp->a);
//     printf("foo.b = %d\n", fp->b);
//     printf("foo.c = %d\n", fp->c);
// }
// void test3() {
//     auto func = [](void *)->void * {
//         foo fp = { 1,2,3 };
//         printfoo("thread 1: ", &fp);
//         pthread_exit((void *)&fp);
//     };
//     pthread_t tid;
//     foo *ret;
//     pthread_create(&tid, NULL, func, NULL);
//     // pthread_join返回结构体，由于此时fp已经结束生命周期，所以下面的printfoo是不确定的
//     pthread_join(tid, (void **)&ret);
//     printfoo("parent: ", ret);
// }
// // 条件变量


// void test4() {
//     int num = 150;
//     std::future<int> ans = std::async([](int &a)->int {
//         std::this_thread::sleep_for(std::chrono::seconds(2));
//         a++;
//         return a;
//     }, std::ref(num));
//     // while (true) {
//     //     std::this_thread::sleep_for(std::chrono::seconds(1));
//     //     printf("running...\n");
//     // }
//     std::packaged_task<int()> pp([]()->int {
//         return 200;
//     });
//     std::cout << pp.get_future().get() << std::endl;
//     std::cout << num << std::endl;
//     // std::cout << sum << std::endl;
// }

// class Foo {
// public:
//     std::mutex mtx;
//     std::unique_lock<mutex> f, s;
//     Foo() {
//         f.lock();
//         s.lock();
//     }
// };

// int main() {
//     vector<string> ans = sol.letterCombinations("23");
//     for (auto str : ans) {
//         cout << str << endl;
//     }
// }

// #include<bits/stdc++.h>
#include<unistd.h>
#include<stdio.h>
#include<malloc.h>
int main() {
    // fork()
    // void *init = sbrk(0);
    // // brk(init + 0x200);
    // void *init2 = sbrk(0);
    
    // printf("init:    %p\n", init);
    // printf("init2:   %p\n", init2);

    void *now = malloc(1);
    printf("pointer: %p\n", now);
    // brk(now + 80);
    // void *init3 = sbrk(0);
    // printf("init3:   %p\n", init3);
    // printf("XXXXX:   %d\n", 128*1024);

    // for (int i = 0;i < init3 - init2;i++) {
    //     malloc(2);
    // }
    // printf("pointer: %p\n", now2);
    // printf("pointer: %p\n", now3);

    // void *init4 = sbrk(0);
    // printf("init4:   %p\n", init4);

    // printf("value: %d\n", *(y));
    // printf("value: %d\n", *b);
    // printf("x_pointer: %p\n", x);
    // printf("y_pointer: %p\n", y);
}
