#ifndef __UNIFORM_H__
#define __UNIFORM_H__

#include <vector>

template <typename T>
class Uniform {
private:
	Uniform();
	Uniform(const Uniform&);
	Uniform& operator=(const Uniform&);
	~Uniform();
public:
	static void Set(unsigned int slot, const T& value);
	static void Set(unsigned int slot, const T* arr, unsigned int length);
	static void Set(unsigned int slot, const std::vector<T>& v);
};

#endif // __UNIFORM_H__
