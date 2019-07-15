#pragma once

#include <cstdint>

#define EXPORT __declspec( dllexport )

class EXPORT PObject {
public:
	PObject();
	~PObject();
	void* operator new(size_t);
	void operator delete(void*);
	void Acquire() const;
	void Release() const;
	size_t alloc_size;
private:
	mutable int32_t references;
};
