#pragma once

#include <cstdint>

#define EXPORT __declspec( dllexport )

namespace pas {

class EXPORT PObject {
public:
	PObject();
	~PObject() {};

	void* operator new(size_t);
	void operator delete(void*);

	void Acquire() const;
	void Release() const;
	
private:
	mutable int32_t m_refs;
	mutable size_t m_alloc_size;
};

};