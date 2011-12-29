//Use:
//  fxc /T fx_4_0 /Vi /Zi /Fo texture.fxo texture.fx
cbuffer cbPerFrame 
{
	float3	g_sunColor;
	float3	g_ambientColor;
	float3	g_sunDir;
};

cbuffer cbPerObject
{
	float4x3		g_mWorldView;
	float4x4		g_mProjectin;
	float4x4		g_mView;
	float4x4		g_mInvView;
	float4x4		g_mView3x3;
	float4x4		g_mViewWorldIT;
	float4x4		g_mInvWorldViewProj;
	float4x4		g_mInvViewProj;
	float4x4		g_mInvProj;
};

Texture2D	albedoTexture;

struct AmbientVS_in
{
	float3 pos : POSITION;
	float2 texuv : TEXCOORD0;
};

struct AmbientPS_in
{
	float4 pos : POSITION;
	float2 texuv : TEXCOORD0;
};

SamplerState texSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};
	
AmbientPS_in AmbientVShader(AmbientVS_in inVert)
{
	AmbientPS_in outVert;

	outVert.pos = float4(inVert.pos,1.0);
	outVert.texuv = inVert.texuv;
	return outVert;
}

float4 AmbientPShader(AmbientPS_in inVert) : SV_TARGET0
{
	// Get our depth value
	float3 albedoColor = albedoTexture.Sample(texSampler,inVert.texuv);
	
	float4 outColor = float4(albedoColor * g_ambientColor,1.0);
	return outColor;
}

//technique10 DeferredRender
//{
//	pass P0
//	{
//		SetVertexShader( CompileShader( vs_4_0, AmbientVShader() ) );
//		SetGeometryShader( NULL );
//		SetPixelShader( CompileShader( ps_4_0, AmbientPShader() ) );
//	}
//}

