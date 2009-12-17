//Use:
//  fxc /T fx_4_0 /Vi /Zi /Fo texture.fxo texture.fx
#include "shared.fx"

Texture2D	normalTexture;

struct QuadVS_in
{
	float3 pos : POSITION;
	float2 texuv : TEXCOORD0;
};

struct QuadPS_in
{
	float4 pos : SV_Position;
	float2 texuv : TEXCOORD0;
};

SamplerState texSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};
	
QuadPS_in ShowNormalsVS(QuadVS_in inVert)
{
	QuadPS_in outVert;

	outVert.pos = float4(inVert.pos,1.0);
	outVert.texuv = inVert.texuv;
	return outVert;
}

float4 ShowNormalsPS(QuadPS_in inVert) : SV_Target
{
	// Get our depth value
	float3 normValue = normalTexture.Sample(texSampler,inVert.texuv);
	
	float4 outColor = float4(abs(normValue),1);
	return outColor;
}

technique10 DeferredRender
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, ShowNormalsVS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, ShowNormalsPS() ) );
	}
}

