#pragma once
#include "Entity.h"


class CConstantBuffer :
	public CEntity
{
private:
	ComPtr<ID3D12Resource>			m_pBuffer;		// 실제 상수버퍼(리소스)
	ComPtr<ID3D12DescriptorHeap>	m_pCBV;			// 상수버퍼 설명자 힙(View 역할)
	D3D12_CPU_DESCRIPTOR_HANDLE		m_hCBV;			// 설명자 힙 핸들
	UINT							m_iBufferSize;  // 상수버퍼 개당 크기
	UINT							m_iMaxCount;	// 상수버퍼 최대 개수
	UINT							m_iCurCount;	// 상수버퍼에 Add 된 현재 데이터 개수
	CONST_REGISTER					m_eRegisterNum; // 상수버퍼 대응 레지스터 번호	
	BYTE*							m_pData;		// 상수버퍼와 맵핑된 주소

	D3D12_DESCRIPTOR_HEAP_DESC		m_tHeapDesc;	// 상수버퍼와 설명자 힙을 연결하는 정보
	
public:
	void Create(UINT _iBufferSize, UINT _iMaxCount, CONST_REGISTER _eRegisterNum);
	//void SetData(void* _pSrc){memcpy(m_pData, _pSrc, m_iBufferSize);}
	//void SetData(void* _pSrc, UINT _iSize) { memcpy(m_pData, _pSrc, _iSize); }
	//void SetData(void* _pSrc, UINT _iSize, UINT _iOffsetPos);
	UINT AddData(void* _pSrc);
	void Clear() { m_iCurCount = 0; }

	ComPtr<ID3D12DescriptorHeap> GetCBV() { return m_pCBV; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetHandle() {return m_hCBV;}
	CONST_REGISTER GetRegisterNum() { return m_eRegisterNum; }
	
public:
	CConstantBuffer();
	virtual ~CConstantBuffer();
};

