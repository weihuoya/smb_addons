#pragma once

template <typename T>
class Singleton
{
public:
    static T *instance()
    {
        static T obj;
        create_object_.do_nothing();
        return &obj;
    }

private:
    struct object_creator
    {
        object_creator() 
        {
            Singleton<T>::instance(); 
        }
        inline void do_nothing() const {}
    };

    static object_creator create_object_;
};


template <typename T>
typename Singleton<T>::object_creator Singleton<T>::create_object_;