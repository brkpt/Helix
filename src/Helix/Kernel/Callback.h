#ifndef CALLBACK_H
#define CALLBACK_H

namespace Helix {

// ****************************************************************************
// ****************************************************************************
class Callback
{
public:
	virtual ~Callback() = 0;
	virtual void Call() = 0;
};

inline Callback::~Callback() {}

// ****************************************************************************
// MemberCallback0
// 
// Holds a callback to a class member function that takes no parameters
// ****************************************************************************
template<class T> 
class MemberCallback0 : public Callback
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
class MemberCallback1 : public Callback
{
public:
	MemberCallback1(T &object,void (T::*callbackFn)(P), P param) :
	m_object(object),
	m_callbackFn(callbackFn),
	m_param(param)
	{}

	void Call()
	{
		(m_object.*m_callbackFn)(m_param);
	}

private:
	T &	m_object;
	P	m_param;

	void (T::*m_callbackFn)(P);
};

// ****************************************************************************
// MemberCallback2
// 
// Holds a callback to a class member function that takes 2 parameters
// ****************************************************************************
template<class T, typename P1, typename P2> 
class MemberCallback2 : public Callback
{
public:
	MemberCallback2(T &object,void (T::*callbackFn)(P1,P2),P1 p1, P2 p2) :
	m_object(object),
	m_callbackFn(callbackFn),
	m_param1(p1),
	m_param2(p2)
	{}

	void Call()
	{
		(m_object.*m_callbackFn)(m_param1, m_param2);
	}

private:
	T &	m_object;
	P1	m_param1;
	P2	m_param2;
	void (T::*m_callbackFn)(P1,P2);
};

// ****************************************************************************
// StaticCallback0
// 
// Holds a callback to a function that isn't a class member (static member or
// regular C function) that takes 0 parameters.
// ****************************************************************************
class StaticCallback0 : public Callback
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
class StaticCallback1 : public Callback
{
public:
	StaticCallback1(void (*callbackFn)(P), P param) :
	m_callbackFn(callbackFn),
	m_param(param)
	{}

	void Call()
	{
		m_callbackFn(m_param);
	}
	
private:
	P	m_param;
	void (*m_callbackFn) (P);
};

// ****************************************************************************
// StaticCallback2
// 
// Holds a callback to a function that isn't a class member (static member or
// regular C function) that takes 2 parameter.
// ****************************************************************************
template<typename P1, typename P2>
class StaticCallback2 : public Callback
{
public:
	StaticCallback2(void (*callbackFn)(P1,P2),P1 param1, P2 param2) :
	m_callbackFn(callbackFn),
	m_param1(param1),
	m_param2(param2)
	{}

	void Call()
	{
		m_callbackFn(m_param1, m_param2);
	}
	
private:
	P1	m_param1;
	P2	m_param2;
	void (*m_callbackFn) (P1,P2);
};


} // namespace Helix
#endif // CALLBACK_H