#ifndef __ATTRIBUTE_H__
#define __ATTRIBUTE_H__

#include <vector>

template <typename T>
class Attribute {
protected:
    unsigned int mHandle;
    unsigned int mCount;
private:
    Attribute(const Attribute& other) = delete;
    Attribute& operator=(const Attribute& other) = delete;

    void SetAttribPointer(unsigned int slot);

public:
    Attribute();
    ~Attribute();

    void Set(const T* inputArray, unsigned int arrayLength);
    void Set(const std::vector<T>& input);
    void BindTo(unsigned int slot);
    void UnBindFrom(unsigned int slot);
    unsigned int Count();
    unsigned int GetHandle();
};

#endif // __ATTRIBUTE_H__
