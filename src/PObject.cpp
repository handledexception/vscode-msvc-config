#include "PObject.h"
#include "PLogger.h"

#include <Windows.h>
#include <stdio.h>
#include <malloc.h>

namespace pas {

static uint64_t g_allocated_bytes = 0;
static uint64_t g_num_allocs = 0;
static uint64_t g_last_alloc_bytes = 0;

PObject::PObject()
:
m_alloc_size(0)
{
	m_refs = 1;
}

// PObject::operator=(const PObject& obj)
// {

// }

// PObject::PObject(const PObject& obj)
// {
// 	::InterlockedIncrement(&obj.m_refs);
// 	*this = obj;
// }

// PObject::PObject(const PObject&& obj)
// {
// 	*this = obj;
// }

void* PObject::operator new(size_t size) {
	void* ptr = malloc(size);
	::InterlockedExchangeSubtract(&g_last_alloc_bytes, g_last_alloc_bytes);
	::InterlockedAdd64((int64_t*)&g_last_alloc_bytes, size);
	::InterlockedAdd64((int64_t*)&g_allocated_bytes, size);
	::InterlockedIncrement(&g_num_allocs);
	PLogger::log(LOG_LEVEL::Info, L"%lld bytes allocated in %lld objects\n", g_allocated_bytes, g_num_allocs);
	return ptr;
}

void PObject::operator delete(void* ptr) {
	//TODO: figure out how to track object type to accurately decrement g_allocated_bytes
	::free(ptr);
	::InterlockedExchangeSubtract(&g_allocated_bytes, g_last_alloc_bytes);
	::InterlockedDecrement(&g_num_allocs);
}

void PObject::Acquire() const {
	::InterlockedIncrement(&m_refs);
}

void PObject::Release() const {
	::InterlockedDecrement(&m_refs);
	if (m_refs == 0)
		delete this;
}

};