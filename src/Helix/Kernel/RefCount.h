#ifndef REFCOUNT_H
#define REFCOUNT_H

namespace Helix
{
	class ReferenceCountable
	{
	public:
		ReferenceCountable()
		: m_refCount(0)
		{}

		LONG Increment()
		{
			InterlockedIncrement(&m_refCount);
		}

		LONG Decrement()
		{
			InterlockedDecrement(&m_refCount);
		}

	private:
		LONG	m_refCount;
	};
}

#endif //REFCOUNT_H