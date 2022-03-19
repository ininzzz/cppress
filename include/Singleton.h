#pragma once
#include <memory>
#include <mutex>


template <class T>
class Singleton {
public:
    typedef typename std::shared_ptr<T> handleType;

    Singleton(const Singleton &inst) = delete;
    Singleton &operator=(const Singleton &inst) = delete;
    
    static handleType &GetInstance() {
        static handleType m_inst(new T);
        return m_inst;
    }

private:
    static handleType m_inst;
};
