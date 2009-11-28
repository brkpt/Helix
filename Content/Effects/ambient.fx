//Use:
//  fxc /T fx_4_0 /Vi /Zi /Fo texture.fxo texture.fx
#include "shared.fx"

float3	ambientColor;
Texture2D	albedoTexture;


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

	outVert.pos = float4(inVert.pos,1.0);
	return outVert;
}

float4 FullScreenQuadPS(QuadPS_in inVert) : SV_Target
{
	// Get our depth value
	float clipX = inVert.pos.x / imageWidth;
	float clipY = inVert.pos.y / imageHeight;
	float albedoColor = albedoTexture.Sample(texSampler,float2(clipX,clipY));
	
	float4 outColor = albedoColor * ambientColor;
	return outColor;
}

technique10 DeferredRender
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, FullScreenQuadVS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, FullScreenQuadPS() ) );
	}
}

