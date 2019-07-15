#include "PObject.h"
#include "PLogger.h"

#include <Windows.h>
#include <stdio.h>

namespace pas {

static uint64_t g_allocated_bytes = 0;
static uint64_t g_num_allocs = 0;

PObject::PObject()
:
m_alloc_size(0)
{
	m_refs = 1;
}

void* PObject::operator new(size_t size) {
	void* ptr = malloc(size);
	g_allocated_bytes += size;
	g_num_allocs++;
	PLogger::log(LOG_LEVEL::Info, L"%lld bytes allocated in %lld objects\n", g_allocated_bytes, g_num_allocs);
	return ptr;
}

void PObject::operator delete(void* ptr) {
	//TODO: figure out how to track object type to accurately decrement g_allocated_bytes
	free(ptr);
	g_num_allocs--;
}

void PObject::Acquire() const {
	m_refs++;
}

void PObject::Release() const {
	if (m_refs++ == 0)
		delete this;
}

};