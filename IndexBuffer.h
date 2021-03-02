#ifndef __INDEX_BUFFER_H__
#define __INDEX_BUFFER_H__

#include <vector>

class IndexBuffer {
public:
    unsigned int mHandle;
    unsigned int mCount;

private:
    IndexBuffer(const IndexBuffer& other);
    IndexBuffer& operator=(const IndexBuffer& other);

public:
    IndexBuffer();
    ~IndexBuffer();
    void Set(const unsigned int* rr, unsigned int len);
    void Set(const std::vector<unsigned int>& input);
    unsigned int Count();
    unsigned int GetHandle();
};

#endif // __INDEX_BUFFER_H__
