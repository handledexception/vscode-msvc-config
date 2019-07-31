#pragma once

#include <cstdint>

#define UTILS_EXPORT __declspec( dllexport )

namespace pas {

class UTILS_EXPORT PObject {
public:
	PObject();
	// PObject(const PObject& obj);
	// PObject(const PObject&& obj);
	~PObject() {};

	void* operator new(size_t);
	void operator delete(void*);
	// PObject operator=(const PObject& obj);

	void Acquire() const;
	void Release() const;
	
private:
	mutable uint32_t m_refs;
	size_t m_alloc_size;
};

};