#pragma once
#include <iostream>

template<class T>
class Singleton
{
public:
    Singleton()                             = default;
    virtual ~Singleton()                    = default;
    Singleton(Singleton  &&)                = delete;
    Singleton(const Singleton  &)           = delete;
    void operator = (const Singleton  &)    = delete;
    T *operator &()                         = delete;

    static T *GetInstance()
    {
        static T* instance;
        if (instance == nullptr)
        {
            instance = new T();
        }
        return instance;
    }
    
};