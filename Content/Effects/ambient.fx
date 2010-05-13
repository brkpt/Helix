//Use:
//  fxc /T fx_4_0 /Vi /Zi /Fo texture.fxo texture.fx
#include "shared.fx"

Texture2D	albedoTexture;

struct AmbientVS_in
{
	float3 pos : POSITION;
	float2 texuv : TEXCOORD0;
};

struct AmbientPS_in
{
	float4 pos : SV_Position;
	float2 texuv : TEXCOORD;
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

float4 AmbientPShader(AmbientPS_in inVert) : SV_Target
{
	// Get our depth value
	float albedoColor = albedoTexture.Sample(texSampler,inVert.texuv);
	
	float4 outColor = float4(albedoColor * ambientColor,1.0);
	return outColor;
}

technique10 DeferredRender
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, AmbientVShader() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, AmbientPShader() ) );
	}
}

