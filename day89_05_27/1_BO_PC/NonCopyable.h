#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

//将其写为抽象类
class NonCopyable
{
/* public: */
protected:
    NonCopyable(){}
    ~NonCopyable() {}

    NonCopyable(const NonCopyable &rhs) = delete;
    NonCopyable &operator=(const NonCopyable &rhs) = delete;

private:

};

#endif
