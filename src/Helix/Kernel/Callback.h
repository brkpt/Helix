#ifndef CALLBACK_H
#define CALLBACK_H

namespace Helix {

template<class T> 
class MemberCallback0
{
public:
	MemberCallback0(T &object,void (T::*callbackFn)()) :
	m_object(object),
	m_callbackFn(callbackFn)
	{
	}

	void Call()
	{
		(m_object.*m_callbackFn)();
	}

private:
	T &	m_object;
	void (T::*m_callbackFn)(void);
};

template<class T, typename P> 
class MemberCallback1
{
public:
	MemberCallback1(T &object,void (T::*callbackFn)(P)) :
	m_object(object),
	m_callbackFn(callbackFn)
	{
	}

	void Call(P p)
	{
		(m_object.*m_callbackFn)(p);
	}
private:
	T &	m_object;
	void (T::*m_callbackFn)(P);
};

template<class T, typename P1, typename P2> 
class MemberCallback2
{
public:
	MemberCallback2(T &object,void (T::*callbackFn)(P1,P2)) :
	m_object(object),
	m_callbackFn(callbackFn)
	{
	}

	void Call(P1 p1,P2 p2)
	{
		(m_object.*m_callbackFn)(p1,p2);
	}
private:
	T &	m_object;
	void (T::*m_callbackFn)(P1,P2);
};

class StaticCallback0
{
public:
	StaticCallback0(void (*callbackFn)()) :
	m_callbackFn(callbackFn)
	{}

	void Call()
	{
		m_callbackFn();
	}
	
private:
	void (*m_callbackFn) ();
};

template<typename P>
class StaticCallback1
{
public:
	StaticCallback1(void (*callbackFn)(P)) :
	m_callbackFn(callbackFn)
	{}

	void Call(P p)
	{
		m_callbackFn(p);
	}
	
private:
	void (*m_callbackFn) (P);
};

} // namespace Helix
#endif // CALLBACK_H