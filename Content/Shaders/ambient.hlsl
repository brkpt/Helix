//Use:
//  fxc /T fx_4_0 /Vi /Zi /Fo texture.fxo texture.fx
#include "shared.hlsl"

Texture2D	albedoTexture : register(t0) ;

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

SamplerState texSampler : register(s0) ;
	
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
	float3 albedoColor = (float3)(albedoTexture.Sample(texSampler,inVert.texuv));
	
	float4 outColor = float4(albedoColor * g_ambientColor.xyz,1.0);
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

