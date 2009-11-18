#ifndef CALLBACK_H
#define CALLBACK_H

namespace Helix {

// ****************************************************************************
// ****************************************************************************
class Callback0
{
public:
	virtual void operator()() const = 0;
};

// ****************************************************************************
// ****************************************************************************
template<typename P1>
class Callback1
{
public:
	virtual void operator()(P1 param) const = 0;
};

// ****************************************************************************
// ****************************************************************************
template<typename P1, typename P2>
class Callback2
{
public:
	virtual void operator()(P1 param1, P2 param2) const = 0;
};

// ****************************************************************************
// ****************************************************************************
template<typename P1, typename P2, typename P3>
class Callback3
{
public:
	virtual void operator()(P1 param1, P2 param2, P3 param3) const = 0;
};

// ****************************************************************************
// MemberCallback0
// 
// Holds a callback to a class member function that takes no parameters
// ****************************************************************************
template<class T> 
class MemberCallback0 : public Callback0
{
public:
	MemberCallback0(T &object,void (T::*callbackFn)()) :
	m_object(object),
	m_callbackFn(callbackFn)
	{}

	void operator()() const
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
template<class T, typename P1> 
class MemberCallback1 : public Callback1<P1>
{
public:
	MemberCallback1(T &object,void (T::*callbackFn)(P1)) :
	m_object(object),
	m_callbackFn(callbackFn)
	{}

	void operator()(P1 param) const
	{
		(m_object.*m_callbackFn)(param);
	}

private:
	T &	m_object;

	void (T::*m_callbackFn)(P1);
};

// ****************************************************************************
// MemberCallback2
// 
// Holds a callback to a class member function that takes 2 parameters
// ****************************************************************************
template<class T, typename P1, typename P2> 
class MemberCallback2 : public Callback2<P1,P2>
{
public:
	MemberCallback2(T &object,void (T::*callbackFn)(P1,P2)) :
	m_object(object),
	m_callbackFn(callbackFn)
	{}

	void operator()(P1 param1, P2 param2) const
	{
		(m_object.*m_callbackFn)(param1, param2);
	}

private:
	T &	m_object;
	void (T::*m_callbackFn)(P1,P2);
};

// ****************************************************************************
// MemberCallback2
// 
// Holds a callback to a class member function that takes 2 parameters
// ****************************************************************************
template<class T, typename P1, typename P2, typename P3> 
class MemberCallback3 : public Callback3<P1,P2,P3>
{
public:
	MemberCallback3(T &object,void (T::*callbackFn)(P1,P2,P3)) :
	m_object(object),
	m_callbackFn(callbackFn)
	{}

	void operator()(P1 param1, P2 param2, P3 param3) const
	{
		(m_object.*m_callbackFn)(param1, param2, param3);
	}

private:
	T &	m_object;
	void (T::*m_callbackFn)(P1,P2,P3);
};

// ****************************************************************************
// StaticCallback0
// 
// Holds a callback to a function that isn't a class member (static member or
// regular C function) that takes 0 parameters.
// ****************************************************************************
class StaticCallback0 : public Callback0
{
public:
	StaticCallback0(void (*callbackFn)()) :
	m_callbackFn(callbackFn)
	{}

	void operator()() const
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
class StaticCallback1 : public Callback1<P>
{
public:
	StaticCallback1(void (*callbackFn)(P)) :
	m_callbackFn(callbackFn)
	{}

	void operator()(P param) const
	{
		m_callbackFn(param);
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
class StaticCallback2 : public Callback2<P1,P2>
{
public:
	StaticCallback2(void (*callbackFn)(P1,P2)) :
	m_callbackFn(callbackFn)
	{}

	void operator()(P1 param1, P2 param2) const
	{
		m_callbackFn(param1, param2);
	}
	
private:
	void (*m_callbackFn) (P1,P2);
};

// ****************************************************************************
// StaticCallback3
// 
// Holds a callback to a function that isn't a class member (static member or
// regular C function) that takes 2 parameter.
// ****************************************************************************
template<typename P1, typename P2, typename P3>
class StaticCallback3 : public Callback3<P1,P2,P3>
{
public:
	StaticCallback3(void (*callbackFn)(P1,P2,P3)) :
	m_callbackFn(callbackFn)
	{}

	void operator()(P1 param1, P2 param2, P3 param3) const
	{
		m_callbackFn(param1, param2, param3);
	}
	
private:
	void (*m_callbackFn) (P1,P2,P3);
};

} // namespace Helix
#endif // CALLBACK_H