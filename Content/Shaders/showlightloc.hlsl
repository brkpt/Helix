//Use:
//  fxc /T fx_4_0 /Vi /Zi /Fo texture.fxo texture.fx
#include "shared.hlsl"

float3		pointLoc;			// Point light location
float3		pointColor;			// Point light color
float		cameraNear;
float		cameraFar;
float		imageWidth;
float		imageHeight;
float		viewAspect;
float		invTanHalfFOV;
Texture2D	albedoTexture;
Texture2D	normalTexture;
Texture2D	depthTexture;

struct QuadVS_in
{
	float3 pos : POSITION;
};

struct QuadPS_in
{
	float4 pos : SV_Position;
};

SamplerState texSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};
	
QuadPS_in FullScreenQuadVS(QuadVS_in inVert)
{
	QuadPS_in outVert;

	float3 P = mul( float4(inVert.pos,1), g_mWorldView );
	outVert.pos = mul( float4(P,1), g_mProjection );
	return outVert;
}

float4 FullScreenQuadPS(QuadPS_in inVert) : SV_Target
{
	float4 outColor = float4(1.0, 1.0, 1.0, 1.0);
	
	return outColor;
}

//technique10 FullScreenQuad
//{
//	pass P0
//	{
//		SetVertexShader( CompileShader( vs_4_0, FullScreenQuadVS() ) );
//		SetGeometryShader( NULL );
//		SetPixelShader( CompileShader( ps_4_0, FullScreenQuadPS() ) );
//	}
//}

