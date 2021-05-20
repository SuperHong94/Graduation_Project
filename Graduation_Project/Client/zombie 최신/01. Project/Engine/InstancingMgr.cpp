#include "stdafx.h"
#include "InstancingMgr.h"

CInstancingMgr::CInstancingMgr()
	: m_iInstIdx(0)
{
}

CInstancingMgr::~CInstancingMgr()
{
	for (size_t i = 0; i < m_vecInstingBuffer.size(); ++i)
	{
		SAFE_DELETE(m_vecInstingBuffer[i]);
	}
}

void CInstancingMgr::init()
{
	CInstancingBuffer* pInstBuffer = new CInstancingBuffer;
	pInstBuffer->init();
	m_vecInstingBuffer.push_back(pInstBuffer);
}

void CInstancingMgr::clear()
{
	m_iInstIdx = 0;
	for (size_t i = 0; i < m_vecInstingBuffer.size(); ++i)
	{
		m_vecInstingBuffer[i]->Clear();
	}
}

void CInstancingMgr::AddInstancingData(tInstancingData & _tData, bool _bAnim)
{
	m_vecInstingBuffer[m_iInstIdx]->AddInstancingData(_tData, _bAnim);
}

CInstancingBuffer * CInstancingMgr::GetInstancingBuffer(long long _iBufferID)
{
	for (auto& pBuffer : m_vecInstingBuffer)
	{
		if (pBuffer->GetBufferID() == _iBufferID)
			return pBuffer;
	}
	return nullptr;
}

CInstancingBuffer* CInstancingMgr::AllocBuffer(long long _bufferid)
{
	if (m_iInstIdx >= (UINT)m_vecInstingBuffer.size())
	{
		CInstancingBuffer* pInstBuffer = new CInstancingBuffer;
		pInstBuffer->init();
		m_vecInstingBuffer.push_back(pInstBuffer);
	}

	m_vecInstingBuffer[m_iInstIdx]->SetBufferID(_bufferid);

	return m_vecInstingBuffer[m_iInstIdx];
}

void CInstancingMgr::SetData()
{
	m_vecInstingBuffer[m_iInstIdx]->SetData();	
	++m_iInstIdx;
}
