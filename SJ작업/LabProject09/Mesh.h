#pragma once



class CMesh
{

private:
	int m_nReferences = 0;
public:

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }
	void ReleaseUploadBuffers();
protected:

	UINT m_nVertices = 0;
	XMFLOAT3* m_pxmf3Positions = NULL;
	ID3D12Resource* m_pd3dVertexBuffer = NULL;
	ID3D12Resource* m_pd3dVertexUploadBuffer = NULL;

	UINT m_nVertexBufferViews = 0;
	D3D12_VERTEX_BUFFER_VIEW* m_pd3dVertexBufferViews; //�������ۺ�



	UINT m_nIndices = 0;  //�ε��� ���ۿ� ���ԵǴ� �ε����� �����̴�. 
	UINT* m_pnIndices = NULL;
	ID3D12Resource* m_pd3dIndexBuffer = NULL;
	ID3D12Resource* m_pd3dIndexUploadBuffer = NULL;
	D3D12_INDEX_BUFFER_VIEW m_d3dIndexBufferView; //�ε��� ���ۺ�


	D3D12_PRIMITIVE_TOPOLOGY m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;




	UINT m_nStartIndex = 0; //�ε��� ���ۿ��� �޽��� �׸��� ���� ���Ǵ� ���� �ε����̴�. 

	UINT m_nSlot = 0;

	UINT m_nStride = 0;
	UINT m_nOffset = 0;

	int m_nBaseVertex = 0; //�ε��� ������ �ε����� ������ �ε����̴�
public:
	CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName = NULL);
	virtual ~CMesh();
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList);

	void LoadMeshFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, char* pstrFileName);
};



