#ifndef BUFFER_H
#define BUFFER_H


#include<sys/uio.h>
#include<vector>
#include<string>
#include<functional>
#include<unistd.h>

class Buffer {
public:
    Buffer();
    
    int ReadFrom(int fd);
    std::string GetBuf();
    
    int WriteTo(int fd);
    void Append(const std::string &str);
    
    void Clear();
    int Readable();
    int Writeable();
private:
    void EnsureSpace(int len);
    std::vector<char> buf;
    int rpos, wpos;
};


#endif