#include "stdafx.h"
#include "Mesh.h"

#include "Device.h"
#include "PathMgr.h"
#include "StructuredBuffer.h"

CMesh::CMesh()
	: CResource(RES_TYPE::MESH)
	, m_pVB(nullptr)
	, m_tVtxView{}	
	, m_iVtxSize(0)
	, m_iVtxCount(0)	  
    , m_pVtxSysMem(nullptr)    
	, m_pBoneFrameData(nullptr)
	, m_pBoneOffset(nullptr)
{
}

CMesh::~CMesh()
{
	SAFE_DELETE(m_pVtxSysMem);

	for (int i = 0; i < m_vecIdxInfo.size(); ++i)
	{
		SAFE_DELETE(m_vecIdxInfo[i].pIdxSysMem);
	}
	
	SAFE_DELETE(m_pBoneFrameData);
	SAFE_DELETE(m_pBoneOffset);
}

void CMesh::Create(UINT _iVtxSize, UINT _iVtxCount, BYTE* _pVtxSysMem
	, DXGI_FORMAT _eIdxFormat, UINT _iIdxCount, BYTE* _pIdxSysMem)
{
	m_iVtxSize = _iVtxSize;
	m_iVtxCount = _iVtxCount;

	tIndexInfo tInfo = {};
	tInfo.eIdxFormat = _eIdxFormat;
	tInfo.iIdxCount = _iIdxCount;

	D3D12_HEAP_PROPERTIES tHeapProperty = {};
		
	tHeapProperty.Type = D3D12_HEAP_TYPE_UPLOAD;
	tHeapProperty.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	tHeapProperty.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	tHeapProperty.CreationNodeMask = 1;
	tHeapProperty.VisibleNodeMask = 1;
	
	D3D12_RESOURCE_DESC tResDesc = {};

	tResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	tResDesc.Alignment = 0;
	tResDesc.Width = m_iVtxSize * m_iVtxCount;
	tResDesc.Height = 1;
	tResDesc.DepthOrArraySize = 1;
	tResDesc.MipLevels = 1;
	tResDesc.Format = DXGI_FORMAT_UNKNOWN;
	tResDesc.SampleDesc.Count = 1;
	tResDesc.SampleDesc.Quality = 0;
	tResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	tResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	DEVICE->CreateCommittedResource(
		&tHeapProperty,
		D3D12_HEAP_FLAG_NONE,
		&tResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_pVB));

	// Copy the triangle data to the vertex buffer.
	UINT8* pVertexDataBegin = nullptr;
	D3D12_RANGE readRange{ 0, 0 }; // We do not intend to read from this resource on the CPU.	
	m_pVB->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
	memcpy(pVertexDataBegin, _pVtxSysMem, (tResDesc.Width * tResDesc.Height) );
	m_pVB->Unmap(0, nullptr);

	// Initialize the vertex buffer view.
	m_tVtxView.BufferLocation = m_pVB->GetGPUVirtualAddress();
	m_tVtxView.StrideInBytes = sizeof(VTX);
	m_tVtxView.SizeInBytes = (UINT)tResDesc.Width;
	   
	// IdxBuffer
	tHeapProperty = {};
	tHeapProperty.Type = D3D12_HEAP_TYPE_UPLOAD;
	tHeapProperty.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	tHeapProperty.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	tHeapProperty.CreationNodeMask = 1;
	tHeapProperty.VisibleNodeMask = 1;

	tResDesc = {};
	tResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	tResDesc.Alignment = 0;
	tResDesc.Width = GetSizeofFormat(tInfo.eIdxFormat) * tInfo.iIdxCount;
	tResDesc.Height = 1;
	tResDesc.DepthOrArraySize = 1;
	tResDesc.MipLevels = 1;
	tResDesc.Format = DXGI_FORMAT_UNKNOWN;
	tResDesc.SampleDesc.Count = 1;
	tResDesc.SampleDesc.Quality = 0;
	tResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	tResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	DEVICE->CreateCommittedResource(
		&tHeapProperty,
		D3D12_HEAP_FLAG_NONE,
		&tResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&tInfo.pIB));

	// Copy the Index data to the Index buffer.
	UINT8* pIdxDataBegin = nullptr;
	readRange = D3D12_RANGE{ 0, 0 }; // We do not intend to read from this resource on the CPU.	
	tInfo.pIB->Map(0, &readRange, reinterpret_cast<void**>(&pIdxDataBegin));
	memcpy(pIdxDataBegin, _pIdxSysMem, (tResDesc.Width * tResDesc.Height) );
	tInfo.pIB->Unmap(0, nullptr);

	// Initialize the Index buffer view.
	tInfo.tIdxView.BufferLocation = tInfo.pIB->GetGPUVirtualAddress();
	tInfo.tIdxView.Format = DXGI_FORMAT_R32_UINT;
	tInfo.tIdxView.SizeInBytes = (UINT)(tResDesc.Width * tResDesc.Height);

	// IndexInfo 에 Subset 추가
	m_vecIdxInfo.push_back(tInfo);

	CDevice::GetInst()->WaitForFenceEvent();
}


CMesh * CMesh::CreateFromContainer(CFBXLoader & _loader)
{
	const tContainer* container = &_loader.GetContainer(0);

	ComPtr<ID3D12Resource> pVB = nullptr;
	D3D12_VERTEX_BUFFER_VIEW tVtxView = {};
	UINT iVtxCount = (UINT)container->vecPos.size();
	UINT iVtxSize = sizeof(VTX);

	VTX* pSysMem = new VTX[iVtxCount];
	for (UINT i = 0; i < iVtxCount; ++i)
	{
		pSysMem[i].vPos = container->vecPos[i];
		pSysMem[i].vUV = container->vecUV[i];
		pSysMem[i].vColor = Vec4(1.f, 0.f, 1.f, 1.f);
		pSysMem[i].vNormal = container->vecNormal[i];
		pSysMem[i].vTangent = container->vecTangent[i];
		pSysMem[i].vBinormal = container->vecBinormal[i];
		pSysMem[i].vWeights = container->vecWeights[i];
		pSysMem[i].vIndices = container->vecIndices[i];
	}

	D3D12_HEAP_PROPERTIES tHeapProperty = {};
	tHeapProperty.Type = D3D12_HEAP_TYPE_UPLOAD;
	tHeapProperty.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	tHeapProperty.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	tHeapProperty.CreationNodeMask = 1;
	tHeapProperty.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC tResDesc = {};
	tResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	tResDesc.Alignment = 0;
	tResDesc.Width = iVtxSize * iVtxCount;
	tResDesc.Height = 1;
	tResDesc.DepthOrArraySize = 1;
	tResDesc.MipLevels = 1;
	tResDesc.Format = DXGI_FORMAT_UNKNOWN;
	tResDesc.SampleDesc.Count = 1;
	tResDesc.SampleDesc.Quality = 0;
	tResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	tResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	DEVICE->CreateCommittedResource(
		&tHeapProperty,
		D3D12_HEAP_FLAG_NONE,
		&tResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pVB));

	// Copy the triangle data to the vertex buffer.
	UINT8* pVertexDataBegin = nullptr;
	D3D12_RANGE readRange{ 0, 0 }; // We do not intend to read from this resource on the CPU.	
	pVB->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
	memcpy(pVertexDataBegin, pSysMem, (tResDesc.Width * tResDesc.Height));
	pVB->Unmap(0, nullptr);

	// Initialize the vertex buffer view.
	tVtxView.BufferLocation = pVB->GetGPUVirtualAddress();
	tVtxView.StrideInBytes = sizeof(VTX);
	tVtxView.SizeInBytes = (UINT)tResDesc.Width;	
	   
	CMesh* pMesh = new CMesh;
	pMesh->m_pVB = pVB;
	pMesh->m_iVtxCount = iVtxCount;
	pMesh->m_iVtxSize = iVtxSize;
	pMesh->m_tVtxView = tVtxView;
	pMesh->m_pVtxSysMem = pSysMem;

	// 인덱스 정보
	UINT iIdxBufferCount = (UINT)container->vecIdx.size();

	for (UINT i = 0; i < iIdxBufferCount; ++i)
	{
		tIndexInfo info = {};
		info.iIdxCount = (UINT)container->vecIdx[i].size();
		info.eIdxFormat = DXGI_FORMAT_R32_UINT;
		info.pIdxSysMem = malloc(GetSizeofFormat(info.eIdxFormat) * info.iIdxCount);
		memcpy(info.pIdxSysMem, &container->vecIdx[i][0], GetSizeofFormat(info.eIdxFormat) * info.iIdxCount);
		

		tHeapProperty = {};
		tHeapProperty.Type = D3D12_HEAP_TYPE_UPLOAD;
		tHeapProperty.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		tHeapProperty.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		tHeapProperty.CreationNodeMask = 1;
		tHeapProperty.VisibleNodeMask = 1;

		tResDesc = {};
		tResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		tResDesc.Alignment = 0;
		tResDesc.Width = GetSizeofFormat(info.eIdxFormat) * info.iIdxCount;
		tResDesc.Height = 1;
		tResDesc.DepthOrArraySize = 1;
		tResDesc.MipLevels = 1;
		tResDesc.Format = DXGI_FORMAT_UNKNOWN;
		tResDesc.SampleDesc.Count = 1;
		tResDesc.SampleDesc.Quality = 0;
		tResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		tResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		DEVICE->CreateCommittedResource(
			&tHeapProperty,
			D3D12_HEAP_FLAG_NONE,
			&tResDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&info.pIB));

		// Copy the Index data to the Index buffer.
		UINT8* pIdxDataBegin = nullptr;
		readRange = D3D12_RANGE{ 0, 0 }; // We do not intend to read from this resource on the CPU.	
		info.pIB->Map(0, &readRange, reinterpret_cast<void**>(&pIdxDataBegin));
		memcpy(pIdxDataBegin, info.pIdxSysMem, (tResDesc.Width * tResDesc.Height));
		info.pIB->Unmap(0, nullptr);

		// Initialize the Index buffer view.
		info.tIdxView.BufferLocation = info.pIB->GetGPUVirtualAddress();
		info.tIdxView.Format = info.eIdxFormat;
		info.tIdxView.SizeInBytes = (UINT)(tResDesc.Width * tResDesc.Height);

		// IndexInfo 에 Subset 추가
		pMesh->m_vecIdxInfo.push_back(info);
	}

	// Animation3D
	if (!container->bAnimation)
		return pMesh;

	vector<tBone*>& vecBone = _loader.GetBones();
	UINT iFrameCount = 0;
	for (UINT i = 0; i < vecBone.size(); ++i)
	{
		tMTBone bone = {};
		bone.iDepth = vecBone[i]->iDepth;
		bone.iParentIndx = vecBone[i]->iParentIndx;
		bone.matBone = GetMatrix(vecBone[i]->matBone);
		bone.matOffset = GetMatrix(vecBone[i]->matOffset);
		bone.strBoneName = vecBone[i]->strBoneName;

		for (UINT j = 0; j < vecBone[i]->vecKeyFrame.size(); ++j)
		{
			tMTKeyFrame tKeyframe = {};
			tKeyframe.dTime = vecBone[i]->vecKeyFrame[j].dTime;
			tKeyframe.iFrame = j;
			tKeyframe.vTranslate.x = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[0];
			tKeyframe.vTranslate.y = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[1];
			tKeyframe.vTranslate.z = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetT().mData[2];

			tKeyframe.vScale.x = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[0];
			tKeyframe.vScale.y = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[1];
			tKeyframe.vScale.z = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetS().mData[2];

			tKeyframe.qRot.x = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[0];
			tKeyframe.qRot.y = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[1];
			tKeyframe.qRot.z = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[2];
			tKeyframe.qRot.w = (float)vecBone[i]->vecKeyFrame[j].matTransform.GetQ().mData[3];

			bone.vecKeyFrame.push_back(tKeyframe);
		}

		iFrameCount = (UINT)max(iFrameCount, bone.vecKeyFrame.size());

		pMesh->m_vecBones.push_back(bone);
	}

	vector<tAnimClip*>& vecAnimClip = _loader.GetAnimClip();

	for (UINT i = 0; i < vecAnimClip.size(); ++i)
	{
		tMTAnimClip tClip = {};

		tClip.strAnimName = vecAnimClip[i]->strName;
		tClip.dStartTime = vecAnimClip[i]->tStartTime.GetSecondDouble();
		tClip.dEndTime = vecAnimClip[i]->tEndTime.GetSecondDouble();
		tClip.dTimeLength = tClip.dEndTime - tClip.dStartTime;

		tClip.iStartFrame = (int)vecAnimClip[i]->tStartTime.GetFrameCount(vecAnimClip[i]->eMode);
		tClip.iEndFrame = (int)vecAnimClip[i]->tEndTime.GetFrameCount(vecAnimClip[i]->eMode);
		tClip.iFrameLength = tClip.iEndFrame - tClip.iStartFrame;
		tClip.eMode = vecAnimClip[i]->eMode;

		pMesh->m_vecAnimClip.push_back(tClip);
	}

	// Animation 이 있는 Mesh 경우 BoneTexture 만들어두기
	if (pMesh->IsAnimMesh())
	{
		// BoneOffet 행렬
		vector<Matrix> vecOffset;
		vector<tFrameTrans> vecFrameTrans;
		vecFrameTrans.resize((UINT)pMesh->m_vecBones.size() * iFrameCount);

		for (size_t i = 0; i < pMesh->m_vecBones.size(); ++i)
		{
			vecOffset.push_back(pMesh->m_vecBones[i].matOffset);

			for (size_t j = 0; j < pMesh->m_vecBones[i].vecKeyFrame.size(); ++j)
			{
				vecFrameTrans[(UINT)pMesh->m_vecBones.size() * j + i]
					= tFrameTrans{ Vec4(pMesh->m_vecBones[i].vecKeyFrame[j].vTranslate)
					, Vec4(pMesh->m_vecBones[i].vecKeyFrame[j].vScale)
					, pMesh->m_vecBones[i].vecKeyFrame[j].qRot };
			}
		}
		pMesh->m_pBoneOffset = new CStructuredBuffer;
		pMesh->m_pBoneOffset->Create(sizeof(Matrix), (UINT)vecOffset.size(), vecOffset.data());

		pMesh->m_pBoneFrameData = new CStructuredBuffer;
		pMesh->m_pBoneFrameData->Create(sizeof(tFrameTrans)
			, (UINT)vecOffset.size() * iFrameCount
			, vecFrameTrans.data());
	}

	return pMesh;
}

void CMesh::render(UINT _iSubset)
{
	CDevice::GetInst()->UpdateTable();	
		
	CMDLIST->IASetVertexBuffers(0, 1, &m_tVtxView);
	CMDLIST->IASetIndexBuffer(&m_vecIdxInfo[_iSubset].tIdxView);	
	CMDLIST->DrawIndexedInstanced(m_vecIdxInfo[_iSubset].iIdxCount, 1, 0, 0, 0);
}

void CMesh::render_instancing(UINT _iInstanceCount, UINT _iSubset)
{
	CDevice::GetInst()->UpdateTable();

	CMDLIST->IASetVertexBuffers(0, 1, &m_tVtxView);
	CMDLIST->IASetIndexBuffer(&m_vecIdxInfo[_iSubset].tIdxView);
	CMDLIST->DrawIndexedInstanced(m_vecIdxInfo[_iSubset].iIdxCount, _iInstanceCount, 0, 0, 0);
}

void CMesh::Load(const wstring & _strFilePath)
{
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");

	// 키값
	SetName(LoadWString(pFile));

	// 상대경로
	SetPath(LoadWString(pFile));

	// 로드
	// 정점데이터
	fread(&m_iVtxSize, sizeof(int), 1, pFile);
	fread(&m_iVtxCount, sizeof(int), 1, pFile);

	UINT iByteSize = 0;
	fread(&iByteSize, sizeof(int), 1, pFile);

	m_pVtxSysMem = malloc(iByteSize);
	fread(m_pVtxSysMem, 1, iByteSize, pFile);

	// 읽은 정보로 VTX Bufffer 만들기
	D3D12_HEAP_PROPERTIES tHeapProperty = {};
	tHeapProperty.Type = D3D12_HEAP_TYPE_UPLOAD;
	tHeapProperty.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	tHeapProperty.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	tHeapProperty.CreationNodeMask = 1;
	tHeapProperty.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC tResDesc = {};
	tResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	tResDesc.Alignment = 0;
	tResDesc.Width = m_iVtxSize * m_iVtxCount;
	tResDesc.Height = 1;
	tResDesc.DepthOrArraySize = 1;
	tResDesc.MipLevels = 1;
	tResDesc.Format = DXGI_FORMAT_UNKNOWN;
	tResDesc.SampleDesc.Count = 1;
	tResDesc.SampleDesc.Quality = 0;
	tResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	tResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	DEVICE->CreateCommittedResource(
		&tHeapProperty,
		D3D12_HEAP_FLAG_NONE,
		&tResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_pVB));

	// Copy the triangle data to the vertex buffer.
	UINT8* pVertexDataBegin = nullptr;
	D3D12_RANGE readRange{ 0, 0 }; // We do not intend to read from this resource on the CPU.	
	m_pVB->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
	memcpy(pVertexDataBegin, m_pVtxSysMem, (tResDesc.Width * tResDesc.Height));
	m_pVB->Unmap(0, nullptr);

	// Initialize the vertex buffer view.
	m_tVtxView.BufferLocation = m_pVB->GetGPUVirtualAddress();
	m_tVtxView.StrideInBytes = sizeof(VTX);
	m_tVtxView.SizeInBytes = (UINT)tResDesc.Width;

	// 인덱스 정보
	UINT iMtrlCount = 0;
	fread(&iMtrlCount, sizeof(int), 1, pFile);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		tIndexInfo info = {};
		fread(&info, sizeof(tIndexInfo), 1, pFile);
		

		UINT iByteWidth = info.iIdxCount * GetSizeofFormat(info.eIdxFormat);

		void* pSysMem = malloc(iByteWidth);
		info.pIdxSysMem = pSysMem;
		fread(info.pIdxSysMem, iByteWidth, 1, pFile);

		tHeapProperty = {};
		tHeapProperty.Type = D3D12_HEAP_TYPE_UPLOAD;
		tHeapProperty.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		tHeapProperty.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		tHeapProperty.CreationNodeMask = 1;
		tHeapProperty.VisibleNodeMask = 1;

		tResDesc = {};
		tResDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		tResDesc.Alignment = 0;
		tResDesc.Width = GetSizeofFormat(info.eIdxFormat) * info.iIdxCount;
		tResDesc.Height = 1;
		tResDesc.DepthOrArraySize = 1;
		tResDesc.MipLevels = 1;
		tResDesc.Format = DXGI_FORMAT_UNKNOWN;
		tResDesc.SampleDesc.Count = 1;
		tResDesc.SampleDesc.Quality = 0;
		tResDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		tResDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		
		memset(&info, 0, sizeof(info.pIB));

		DEVICE->CreateCommittedResource(
			&tHeapProperty,
			D3D12_HEAP_FLAG_NONE,
			&tResDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&info.pIB));

		// Copy the Index data to the Index buffer.
		UINT8* pIdxDataBegin = nullptr;
		readRange = D3D12_RANGE{ 0, 0 }; // We do not intend to read from this resource on the CPU.	
		info.pIB->Map(0, &readRange, reinterpret_cast<void**>(&pIdxDataBegin));
		memcpy(pIdxDataBegin, info.pIdxSysMem, (tResDesc.Width * tResDesc.Height));
		info.pIB->Unmap(0, nullptr);

		// Initialize the Index buffer view.
		info.tIdxView.BufferLocation = info.pIB->GetGPUVirtualAddress();
		info.tIdxView.Format = info.eIdxFormat;
		info.tIdxView.SizeInBytes = (UINT)(tResDesc.Width * tResDesc.Height);

		m_vecIdxInfo.push_back(info);
	}

	// Animation3D 정보 읽기
	int iCount = 0;
	fread(&iCount, sizeof(int), 1, pFile);
	for (int i = 0; i < iCount; ++i)
	{
		tMTAnimClip tClip = {};

		tClip.strAnimName = LoadWString(pFile);
		fread(&tClip.dStartTime, sizeof(double), 1, pFile);
		fread(&tClip.dEndTime, sizeof(double), 1, pFile);
		fread(&tClip.dTimeLength, sizeof(double), 1, pFile);
		fread(&tClip.eMode, sizeof(int), 1, pFile);
		fread(&tClip.fUpdateTime, sizeof(float), 1, pFile);
		fread(&tClip.iStartFrame, sizeof(int), 1, pFile);
		fread(&tClip.iEndFrame, sizeof(int), 1, pFile);
		fread(&tClip.iFrameLength, sizeof(int), 1, pFile);

		m_vecAnimClip.push_back(tClip);
	}

	iCount = 0;
	fread(&iCount, sizeof(int), 1, pFile);
	m_vecBones.resize(iCount);

	UINT _iFrameCount = 0;
	for (int i = 0; i < iCount; ++i)
	{
		m_vecBones[i].strBoneName = LoadWString(pFile);
		fread(&m_vecBones[i].iDepth, sizeof(int), 1, pFile);
		fread(&m_vecBones[i].iParentIndx, sizeof(int), 1, pFile);
		fread(&m_vecBones[i].matBone, sizeof(Matrix), 1, pFile);
		fread(&m_vecBones[i].matOffset, sizeof(Matrix), 1, pFile);

		UINT iFrameCount = 0;
		fread(&iFrameCount, sizeof(int), 1, pFile);
		m_vecBones[i].vecKeyFrame.resize(iFrameCount);
		_iFrameCount = (UINT)max(_iFrameCount, iFrameCount);
		for (UINT j = 0; j < iFrameCount; ++j)
		{
			fread(&m_vecBones[i].vecKeyFrame[j], sizeof(tMTKeyFrame), 1, pFile);
		}
	}

	// Animation 이 있는 Mesh 경우 Bone StructuredBuffer 만들기
	if (m_vecAnimClip.size() > 0 && m_vecBones.size() > 0)
	{
		wstring strBone = GetName();

		// BoneOffet 행렬
		vector<Matrix> vecOffset;
		vector<tFrameTrans> vecFrameTrans;
		vecFrameTrans.resize((UINT)m_vecBones.size() * _iFrameCount);

		for (size_t i = 0; i < m_vecBones.size(); ++i)
		{
			vecOffset.push_back(m_vecBones[i].matOffset);

			for (size_t j = 0; j < m_vecBones[i].vecKeyFrame.size(); ++j)
			{
				vecFrameTrans[(UINT)m_vecBones.size() * j + i]
					= tFrameTrans{ Vec4(m_vecBones[i].vecKeyFrame[j].vTranslate)
					, Vec4(m_vecBones[i].vecKeyFrame[j].vScale)
					, Vec4(m_vecBones[i].vecKeyFrame[j].qRot) };
			}
		}
		m_pBoneOffset = new CStructuredBuffer;
		m_pBoneOffset->Create(sizeof(Matrix), (UINT)vecOffset.size(), vecOffset.data());

		m_pBoneFrameData = new CStructuredBuffer;
		m_pBoneFrameData->Create(sizeof(tFrameTrans)
			, (UINT)vecOffset.size() * (UINT)_iFrameCount
			, vecFrameTrans.data());
	}

	fclose(pFile);
}

void CMesh::Save(const wstring & _strPath)
{
	wstring strFileName = CPathMgr::GetResPath();
	strFileName += _strPath;
	SetPath(_strPath);

	FILE* pFile = nullptr;
	errno_t err = _wfopen_s(&pFile, strFileName.c_str(), L"wb");

	assert(pFile);

	// 키값
	SaveWString(pFile, GetName().c_str());

	// 상대 경로
	SaveWString(pFile, GetPath().c_str());

	// 정점 데이터 저장	
	fwrite(&m_iVtxSize, sizeof(int), 1, pFile);
	fwrite(&m_iVtxCount, sizeof(int), 1, pFile);

	int iByteSize = m_iVtxSize * m_iVtxCount;
	fwrite(&iByteSize, sizeof(int), 1, pFile);
	fwrite(m_pVtxSysMem, iByteSize, 1, pFile);
	
	// 인덱스 정보
	UINT iMtrlCount = (UINT)m_vecIdxInfo.size();
	fwrite(&iMtrlCount, sizeof(int), 1, pFile);

	UINT iIdxBuffSize = 0;
	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		fwrite(&m_vecIdxInfo[i], sizeof(tIndexInfo), 1, pFile);
		fwrite(m_vecIdxInfo[i].pIdxSysMem
			, m_vecIdxInfo[i].iIdxCount * GetSizeofFormat(m_vecIdxInfo[i].eIdxFormat)
			, 1, pFile);
	}

	// Animation3D 정보 
	UINT iCount = (UINT)m_vecAnimClip.size();
	fwrite(&iCount, sizeof(int), 1, pFile);
	for (UINT i = 0; i < iCount; ++i)
	{
		SaveWString(pFile, m_vecAnimClip[i].strAnimName.c_str());
		fwrite(&m_vecAnimClip[i].dStartTime, sizeof(double), 1, pFile);
		fwrite(&m_vecAnimClip[i].dEndTime, sizeof(double), 1, pFile);
		fwrite(&m_vecAnimClip[i].dTimeLength, sizeof(double), 1, pFile);
		fwrite(&m_vecAnimClip[i].eMode, sizeof(int), 1, pFile);
		fwrite(&m_vecAnimClip[i].fUpdateTime, sizeof(float), 1, pFile);
		fwrite(&m_vecAnimClip[i].iStartFrame, sizeof(int), 1, pFile);
		fwrite(&m_vecAnimClip[i].iEndFrame, sizeof(int), 1, pFile);
		fwrite(&m_vecAnimClip[i].iFrameLength, sizeof(int), 1, pFile);
	}

	iCount = (UINT)m_vecBones.size();
	fwrite(&iCount, sizeof(int), 1, pFile);

	for (UINT i = 0; i < iCount; ++i)
	{
		SaveWString(pFile, m_vecBones[i].strBoneName.c_str());
		fwrite(&m_vecBones[i].iDepth, sizeof(int), 1, pFile);
		fwrite(&m_vecBones[i].iParentIndx, sizeof(int), 1, pFile);
		fwrite(&m_vecBones[i].matBone, sizeof(Matrix), 1, pFile);
		fwrite(&m_vecBones[i].matOffset, sizeof(Matrix), 1, pFile);

		int iFrameCount = (int)m_vecBones[i].vecKeyFrame.size();
		fwrite(&iFrameCount, sizeof(int), 1, pFile);

		for (int j = 0; j < m_vecBones[i].vecKeyFrame.size(); ++j)
		{
			fwrite(&m_vecBones[i].vecKeyFrame[j], sizeof(tMTKeyFrame), 1, pFile);
		}
	}

	fclose(pFile);
}