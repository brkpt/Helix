// Use:
//  fxc [/LD] /T fx_2_0 /Vi /Zi /Fo texture.fxo texture.fx

#include "shared.fx"

texture	textureImage;

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

sampler texSampler = 
sampler_state
{
	Texture = <textureImage>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
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

float4 TexturePixelShader(TexturePixelShader_in In) : COLOR
{
	return tex2D(texSampler,In.texuv);
}

technique SingleTexture
{
	pass P0
	{
		VertexShader = compile vs_3_0 TextureVertexShader();
		PixelShader = compile ps_3_0 TexturePixelShader();
		
		CullMode = CCW;
	}
}

