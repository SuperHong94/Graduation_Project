#pragma once


class CEntity
{
private:
	static UINT g_iID;

private:
	wstring		m_strName;
	UINT		m_iID;

public:
	void SetName(wstring _strName) { m_strName = _strName; }
	const wstring& GetName() { return m_strName; }
	UINT GetID() { return m_iID; }


public:
	CEntity& operator = (const CEntity& _other)
	{
		// ID 대입 방지
		m_strName = _other.m_strName;
		return *this;
	}

public:
	CEntity();
	CEntity(const CEntity& _other);
	virtual ~CEntity();
};

