#include "PObject.h"
#include "PLogger.h"

#include <Windows.h>
#include <stdio.h>

static uint64_t g_allocated_bytes = 0;
static uint64_t g_num_allocs = 0;

PObject::PObject() {
	this->references = 1;
}

void* PObject::operator new(size_t size) {
	void* ptr = malloc(size);
	g_allocated_bytes += size;
	g_num_allocs += 1;
	PLogger::log(LOG_LEVEL::Info, L"%lld bytes allocated in %lld objects\n", g_allocated_bytes, g_num_allocs);
	return ptr;
}

void PObject::operator delete(void* ptr) {
	//TODO: figure out how to track object type to accurately decrement g_allocated_bytes
	free(ptr);
	g_num_allocs -= 1;
}

void PObject::Acquire() const {
	this->references++;
}

void PObject::Release() const {
	this->references--;
	if (references > 0)
		return;

	delete this;
}