// Use:
//  fxc [/LD] /T fx_4_0 /Vi /Zi /Fo texture.fxo texture.fx

#include "shared.fx"

Texture2D	textureImage;

struct TextureVertex_in
{
	float4 pos : POSITION;
	float2 texuv : TEXCOORD0;
};

struct TextureVertex_out
{
	float4 pos : POSITION;
	float2 texuv : TEXCOORD0;
};

sampler2D texSampler  = sampler_state
{
	Texture = (textureImage);
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};
	
TextureVertex_out TextureVertexShader(TextureVertex_in In)
{
	TextureVertex_out Out;

	float3 P = mul( In.pos ,WorldView );
	Out.pos = mul( float4(P,1), Projection );
	Out.texuv = In.texuv;
	return Out;
}

struct TexturePixelShader_in
{
	float4	pos : POSITION;
	float2	texuv	 : TEXCOORD0;
};

float4 TexturePixelShader(TexturePixelShader_in In) : COLOR0
{
	return tex2D(texSampler,In.texuv);
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

