#pragma once


class CEntity
{
private:
	wstring		m_strName;

public:
	void SetName(wstring _strName) { m_strName = _strName; }
	const wstring& GetName() { return m_strName; }

public:
	CEntity();
	virtual ~CEntity();
};

