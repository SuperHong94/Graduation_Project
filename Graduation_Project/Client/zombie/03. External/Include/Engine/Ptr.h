#pragma once

template<typename T>
class Ptr;

template<typename T>
bool operator == (void* _pointer, const Ptr<T>& _ptr)
{
	if (_pointer == _ptr.m_pRes)
		return true;
	return false;
}

template<typename T>
bool operator != (void* _pointer, const Ptr<T>& _ptr)
{
	if (_pointer == _ptr.m_pRes)
		return false;
	return true;
}


template<typename T>
class Ptr
{
private:
	T*			m_pRes;

public:
	Ptr()
		: m_pRes(nullptr)
	{
	}

	Ptr(T* _pRes)
		: m_pRes(_pRes)
	{
		if(nullptr != _pRes)
			m_pRes->AddRef();
	}

	Ptr(const Ptr<T>& _pPtr)
		: m_pRes(_pPtr.m_pRes)
	{
		if (nullptr != m_pRes)
			m_pRes->AddRef();
	}

	~Ptr()
	{
		if(nullptr != m_pRes)
			m_pRes->SubRef();
	}

public:
	void operator = (T* _pRes)
	{
		if (nullptr != m_pRes)
			m_pRes->SubRef();

		m_pRes = _pRes;

		if (nullptr != m_pRes)
			m_pRes->AddRef();
	}

	void operator = (const Ptr<T>& _pointer)
	{
		if (nullptr != m_pRes)
			m_pRes->SubRef();

		m_pRes = _pointer.m_pRes;

		if (nullptr != m_pRes)
			m_pRes->AddRef();
	}

	bool operator == (const T* _pResource)
	{
		if (m_pRes == _pResource)
			return true;
		return false;
	}

	bool operator != (const T* _pResource)
	{
		if (m_pRes == _pResource)
			return false;
		return true;
	}

	T* operator->()
	{
		return m_pRes;
	}

	T* GetPointer()
	{
		return m_pRes;
	}
	
	/*operator T* ()
	{
		return m_pRes;
	}*/

	template<typename T>
	friend bool operator == (void*, const Ptr<T>&);

	template<typename T>
	friend bool operator != (void*, const Ptr<T>&);
};
