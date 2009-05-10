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

		LONG AddReference()
		{
			return InterlockedIncrement(&m_refCount);
		}

		LONG RemoveReference()
		{
			return InterlockedDecrement(&m_refCount);
		}

		LONG ReferenceCount()
		{
			return m_refCount;
		}

	private:
		LONG	m_refCount;
	};
}

#endif //REFCOUNT_H