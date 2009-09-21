#ifndef HEAPALLOC_H
#define HEAPALLOC_H

class HeapAllocator
{
public:
	void *	Alloc(size_t size)	{ return new unsigned char[size]; }
	void	Free(void *ptr)		{ delete [] ptr; }
	void	FreeAll()			{ }
};

#endif // HEAPALLOC_H