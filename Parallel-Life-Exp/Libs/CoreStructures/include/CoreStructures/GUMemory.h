
//
// GUMemory.h
//

// define C++ memory handling functions and macros to simplify the creation, validation and tracking of heap allocated memory during debugging.  GUMemory defines the gu_memAssert(v) macro to validate allocated memory.  If v==nullptr then a message is logged to stdout and the host process is aborted.  Memory tracking versions of malloc, calloc and free are also defined along with memory tracking overrides of new, new[], delete and delete[].  To override the standard malloc, calloc and free calls with the memory tracking versions __GU_DEBUG_MEMORY__ must be defined in the host application or framework

#pragma once

#include <cstddef>
#include <cstdlib>
#include <new>
#include <memory>

#ifdef __GU_DEBUG_MEMORY__

// malloc, calloc and free overriding defines

#define	malloc		gu_malloc
#define	calloc		gu_calloc
#define	free		gu_free 


// override new and delete operators to track memory allocation

void *operator new(std::size_t size);
void operator delete(void *ptr);

void *operator new[](std::size_t size);
void operator delete[](void *ptr);

#endif


// memory validation macros and functions

void gu_memAssertFail(const char *ptrString, const char *fnString);

// gu_memAssert determines if v==nullptr and if so logs an error message to the console (stdout) and aborts the host process
#define gu_memAssert(v)				if (!(v)) gu_memAssertFail(#v, __FUNCTION__)



// memory tracking functions

void *gu_malloc(std::size_t memreq);
void *gu_calloc(std::size_t num, std::size_t size);
void gu_free(void *ptr);


// memory reporting functions

unsigned long gu_memory_allocations();
unsigned long gu_memory_deallocations();
unsigned long gu_memory_error();
void gu_memory_report();


// memory counter adjustment functions

void compensate_malloc_count(unsigned long c);
void compensate_free_count(unsigned long c);


