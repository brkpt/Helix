//Use:
//  fxc /T fx_4_0 /Vi /Zi /Fo texture.fxo texture.fx
#include "shared.hlsl"

Texture2D	albedoTexture;
Texture2D	normalTexture;
Texture2D	depthTexture;

SamplerState colorSampler : register(s0) ;
SamplerState depthSampler : register(s1) ;
SamplerState normalSampler : register(s2) ;

struct QuadVS_in
{
	float3 pos : POSITION;
};

struct QuadPS_in
{
	float4 pos : SV_Position;
};

SamplerState texSampler : register(s0) ;
	
QuadPS_in FullScreenQuadVS(QuadVS_in inVert)
{
	QuadPS_in outVert;

	float3 P = (float3)(mul( float4(inVert.pos,1), g_mWorldView ));
	outVert.pos = mul( float4(P,1), g_mProjection );
	return outVert;
}

float4 FullScreenQuadPS(QuadPS_in inVert) : SV_Target
{
	float4 outColor = float4(1.0, 1.0, 1.0, 1.0);
	
	return outColor;
}


