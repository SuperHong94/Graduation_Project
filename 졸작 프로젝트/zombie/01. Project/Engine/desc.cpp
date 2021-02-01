#include "stdafx.h"
#include "global.h"

D3D12_RASTERIZER_DESC g_arrRSDesc[(UINT)RS_TYPE::END] =
{
	D3D12_RASTERIZER_DESC
	{
		  D3D12_FILL_MODE_SOLID // FillMode
		, D3D12_CULL_MODE_BACK	//CullMode
		, FALSE //FrontCounterClockwise
		, D3D12_DEFAULT_DEPTH_BIAS //DepthBias
		, D3D12_DEFAULT_DEPTH_BIAS_CLAMP //DepthBiasClamp
		, D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS	//SlopeScaledDepthBias
		, TRUE	//DepthClipEnable
		, FALSE	//MultisampleEnable
		, FALSE	//AntialiasedLineEnable
		, 0		//ForcedSampleCount
		, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF	//ConservativeRaster
	},
};

D3D12_BLEND_DESC g_arrBlendDesc[(UINT)BLEND_TYPE::END] = 
{
	D3D12_BLEND_DESC{
		FALSE ,  // AlphaToCoverageEnable
		FALSE , // IndependentBlendEnable
				
		D3D12_RENDER_TARGET_BLEND_DESC
		{
			FALSE,FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL,
		}, 		
	}
};