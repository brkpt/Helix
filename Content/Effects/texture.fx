// Use:
//  fxc [/LD] /T fx_4_0 /Vi /Zi /Fo texture.fxo texture.fx

#include "shared.fx"

Texture2D	textureImage;

struct TextureVS_in
{
	float3 pos : POSITION;
	float2 texuv : TEXCOORD0;
};

struct TexturePS_in
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
	
TexturePS_in TextureVertexShader(TextureVS_in In)
{
	TexturePS_in Out;

	float3 P = mul( float4(In.pos,1), WorldView );
	Out.pos = mul( float4(P,1), Projection );
	Out.texuv = In.texuv;
	return Out;
}

float4 TexturePixelShader(TexturePS_in In) : SV_TARGET
{
	return textureImage.Sample(texSampler, In.texuv);
}

technique10 SingleTexture
{
	pass P0
	{
		SetVertexShader( compile vs_4_0 TextureVertexShader() );
		SetGeometryShader( NULL );
		SetPixelShader( compile ps_4_0 TexturePixelShader() );
	}
}

