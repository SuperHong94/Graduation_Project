#include "stdafx.h"


/*���� ���ҽ��� �����ϴ� �Լ��̴�. ������ �� ������ ���� ���� ���ҽ��� �����ϰ� �ʱ�ȭ �����Ͱ� ������ �ʱ�ȭ
�Ѵ�.*/

ID3D12Resource* CreateBufferResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType,
	D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource** ppd3dUploadBuffer)
{
	//1. �� Ÿ�Կ� �´� ���ҽ��� ������ش�. 
	ID3D12Resource* pd3dBuffer = NULL;
	D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc; //���Ӽ�
	::ZeroMemory(&d3dHeapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapPropertiesDesc.Type = d3dHeapType; //��Ÿ�����˷��ش�.
	d3dHeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN; //���� ���� CPU������ �Ӽ��� �˾Ƽ��϶�� �Ѵ�.
	d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN; //�������� �޸�Ǯ�� �˾Ƽ��ϰ� �Ѵ�.
	d3dHeapPropertiesDesc.CreationNodeMask = 1;  //���� GPU����ͷ� �Ѵ�. (����GPU�ϱ� 0�Ǵ� 1��
	d3dHeapPropertiesDesc.VisibleNodeMask = 1;  //����GPU�ϱ� 0�Ǵ� 1��
	D3D12_RESOURCE_DESC d3dResourceDesc;
	::ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; //
	d3dResourceDesc.Alignment = 0;  //�޸����ı��� 64Kb
	d3dResourceDesc.Width = nBytes;//����ũ��
	d3dResourceDesc.Height = 1; //����ũ��
	d3dResourceDesc.DepthOrArraySize = 1;//3���� zũ��
	d3dResourceDesc.MipLevels = 1; //�Ӹʷ���
	d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN; //���ҽ����� �˾Ƽ� �ϼ�
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR; //���ι���� ��켱
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;  //��뵵�� ������ �˾Ƽ� �ϼ�
	D3D12_RESOURCE_STATES d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST; //���ҽ� ���´� ���翬���� ��ǥ�� ����Ҷ��̴�.( ���⸸ ���)
	if (d3dHeapType == D3D12_HEAP_TYPE_UPLOAD) //��Ÿ���� ���ε� ���̸� ���ҽ� �ʱ���¸� ���ε����� �ʱ����(���常����)�� �����.
		d3dResourceInitialStates = D3D12_RESOURCE_STATE_GENERIC_READ;
	else if (d3dHeapType == D3D12_HEAP_TYPE_READBACK) //�������� �����(�б�������)�̸� ���ҽ��ʱ���¸�/���ҽ� ���´� ���翬���� ��ǥ�� �����.(���⸸����)
		d3dResourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;

	HRESULT hResult = pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, d3dResourceInitialStates, NULL,
		__uuidof(ID3D12Resource), (void**)&pd3dBuffer); //���ҽ� ����� committed�ϱ� CPU�� GPU�Ѵ� ����
	if (pData)
	{
		switch (d3dHeapType)
		{
		case D3D12_HEAP_TYPE_DEFAULT: //��Ÿ���� ����Ʈ ���϶� ���ε� ���۸� ���� ���ε� ������ ������ ����Ʈ ���ۿ� �����Ѵ�.(����Ʈ���� CPU�� ���� ����)
		{
			if (ppd3dUploadBuffer)
			{
				//���ε� ���۸� �����Ѵ�. 
				d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
				pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL,
					__uuidof(ID3D12Resource), (void**)ppd3dUploadBuffer);

				//���ε� ���۸� �����Ͽ� �ʱ�ȭ �����͸� ���ε� ���ۿ� �����Ѵ�. 
				D3D12_RANGE d3dReadRange = { 0, 0};
				UINT8* pBufferDataBegin = NULL;
				(*ppd3dUploadBuffer)->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
				memcpy(pBufferDataBegin, pData, nBytes);
				(*ppd3dUploadBuffer)->Unmap(0, NULL);
				//���ε� ������ ������ ����Ʈ ���ۿ� �����Ѵ�.
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
		case D3D12_HEAP_TYPE_UPLOAD: //���ε����϶����� 
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