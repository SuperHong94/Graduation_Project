#include "stdafx.h"


/*버퍼 리소스를 생성하는 함수이다. 버퍼의 힙 유형에 따라 버퍼 리소스를 생성하고 초기화 데이터가 있으면 초기화
한다.*/

ID3D12Resource* CreateBufferResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType,
	D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource** ppd3dUploadBuffer)
{
	//1. 힙 타입에 맞는 리소스를 만들어준다. 
	ID3D12Resource* pd3dBuffer = NULL;
	D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc; //힙속성
	::ZeroMemory(&d3dHeapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapPropertiesDesc.Type = d3dHeapType; //힙타입을알려준다.
	d3dHeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN; //힙에 대한 CPU페이지 속성은 알아서하라고 한다.
	d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN; //힙에대한 메모리풀도 알아서하게 한다.
	d3dHeapPropertiesDesc.CreationNodeMask = 1;  //단일 GPU어댑터로 한다. (단일GPU니깐 0또는 1로
	d3dHeapPropertiesDesc.VisibleNodeMask = 1;  //단일GPU니깐 0또는 1로
	D3D12_RESOURCE_DESC d3dResourceDesc;
	::ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; //
	d3dResourceDesc.Alignment = 0;  //메모리정렬기준 64Kb
	d3dResourceDesc.Width = nBytes;//가로크기
	d3dResourceDesc.Height = 1; //세로크기
	d3dResourceDesc.DepthOrArraySize = 1;//3차원 z크기
	d3dResourceDesc.MipLevels = 1; //밉맵레벨
	d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN; //리소스형식 알아서 하셈
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR; //매핑방법은 행우선
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;  //어떤용도로 쓸지는 알아서 하셈
	D3D12_RESOURCE_STATES d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST; //리소스 상태는 복사연산의 목표로 사용할때이다.( 쓰기만 기능)
	if (d3dHeapType == D3D12_HEAP_TYPE_UPLOAD) //힙타입이 업로드 힙이면 리소스 초기상태를 업로드힙의 초기상태(리드만가능)로 만든다.
		d3dResourceInitialStates = D3D12_RESOURCE_STATE_GENERIC_READ;
	else if (d3dHeapType == D3D12_HEAP_TYPE_READBACK) //힙탕입이 리드백(읽기위한힙)이면 리소스초기상태를/리소스 상태는 복사연산의 목표로 만든다.(쓰기만가능)
		d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;

	HRESULT hResult = pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, d3dResourceInitialStates, NULL,
		__uuidof(ID3D12Resource), (void**)&pd3dBuffer); //리소스 만들기 committed니깐 CPU와 GPU둘다 만듬
	if (pData)
	{
		switch (d3dHeapType)
		{
		case D3D12_HEAP_TYPE_DEFAULT: //힙타입이 디폴트 힙일때 업로드 버퍼를 만들어서 업로드 버퍼의 내용을 디폴트 버퍼에 복사한다.(디폴트힙은 CPU에 접근 못함)
		{
			if (ppd3dUploadBuffer)
			{
				//업로드 버퍼를 생성한다. 
				d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
				pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL,
					__uuidof(ID3D12Resource), (void**)ppd3dUploadBuffer);

				//업로드 버퍼를 매핑하여 초기화 데이터를 업로드 버퍼에 복사한다. 
				D3D12_RANGE d3dReadRange = { 0, 0};
				UINT8* pBufferDataBegin = NULL;
				(*ppd3dUploadBuffer)->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
				memcpy(pBufferDataBegin, pData, nBytes);
				(*ppd3dUploadBuffer)->Unmap(0, NULL);
				//업로드 버퍼의 내용을 디폴트 버퍼에 복사한다.
				pd3dCommandList->CopyResource(pd3dBuffer, *ppd3dUploadBuffer);
				D3D12_RESOURCE_BARRIER d3dResourceBarrier;
				::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
				d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				d3dResourceBarrier.Transition.pResource = pd3dBuffer;
				d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
				d3dResourceBarrier.Transition.StateAfter = d3dResourceStates;
				d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
			}
			break;
		}
		case D3D12_HEAP_TYPE_UPLOAD: //업로드힙일때에는 
		{
			D3D12_RANGE d3dReadRange = { 0, 0 };
			UINT8* pBufferDataBegin = NULL;
			pd3dBuffer->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
			memcpy(pBufferDataBegin, pData, nBytes);
			pd3dBuffer->Unmap(0, NULL);
			break;
		}
		case D3D12_HEAP_TYPE_READBACK:
			break;
		}
	}
	return(pd3dBuffer);
}