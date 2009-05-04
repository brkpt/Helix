#ifndef CALLBACK_H
#define CALLBACK_H

namespace Helix {

// ****************************************************************************
// MemberCallback0
// 
// Holds a callback to a class member function that takes no parameters
// ****************************************************************************
template<class T> 
class MemberCallback0
{
public:
	MemberCallback0(T &object,void (T::*callbackFn)()) :
	m_object(object),
	m_callbackFn(callbackFn)
	{}

	void Call()
	{
		(m_object.*m_callbackFn)();
	}

private:
	T &	m_object;
	void (T::*m_callbackFn)(void);
};

// ****************************************************************************
// MemberCallback1
// 
// Holds a callback to a class member function that takes 1 parameter
// ****************************************************************************
template<class T, typename P> 
class MemberCallback1
{
public:
	MemberCallback1(T &object,void (T::*callbackFn)(P)) :
	m_object(object),
	m_callbackFn(callbackFn)
	{}

	void Call(P p)
	{
		(m_object.*m_callbackFn)(p);
	}

private:
	T &	m_object;
	void (T::*m_callbackFn)(P);
};

// ****************************************************************************
// MemberCallback2
// 
// Holds a callback to a class member function that takes 2 parameters
// ****************************************************************************
template<class T, typename P1, typename P2> 
class MemberCallback2
{
public:
	MemberCallback2(T &object,void (T::*callbackFn)(P1,P2)) :
	m_object(object),
	m_callbackFn(callbackFn)
	{}

	void Call(P1 p1,P2 p2)
	{
		(m_object.*m_callbackFn)(p1,p2);
	}

private:
	T &	m_object;
	void (T::*m_callbackFn)(P1,P2);
};

// ****************************************************************************
// StaticCallback0
// 
// Holds a callback to a function that isn't a class member (static member or
// regular C function) that takes 0 parameters.
// ****************************************************************************
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

// ****************************************************************************
// StaticCallback1
// 
// Holds a callback to a function that isn't a class member (static member or
// regular C function) that takes 1 parameter.
// ****************************************************************************
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

// ****************************************************************************
// StaticCallback2
// 
// Holds a callback to a function that isn't a class member (static member or
// regular C function) that takes 2 parameter.
// ****************************************************************************
template<typename P1, typename P2>
class StaticCallback2
{
public:
	StaticCallback2(void (*callbackFn)(P1,P2)) :
	m_callbackFn(callbackFn)
	{}

	void Call(P1 p1, P2 p2)
	{
		m_callbackFn(p1,p2);
	}
	
private:
	void (*m_callbackFn) (P1,P2);
};

} // namespace Helix
#endif // CALLBACK_H