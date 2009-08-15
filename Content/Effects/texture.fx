// Use:
//  fxc [/LD] /T fx_4_0 /Vi /Zi /Fo texture.fxo texture.fx

#include "shared.fx"

Texture2D	textureImage;

struct TextureVS_in
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texuv : TEXCOORD0;
};

struct TexturePS_in
{
	float4 pos : POSITION;
	float3 normal : NORMAL;
	float2 texuv : TEXCOORD0;
};

struct TexturePS_out
{
	float4 color : SV_TARGET0;
	float4 normal : SV_TARGET1;
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
	float3 N = mul( float4(In.normal,1), WorldView );
	Out.pos = mul( float4(P,1), Projection );
	Out.normal = N;
	Out.texuv = In.texuv;
	return Out;
}

TexturePS_out TexturePixelShader(TexturePS_in In) 
{
	TexturePS_out outValue;
	outValue.color = textureImage.Sample(texSampler, In.texuv);
	outValue.normal.xyz = In.normal.xyz;
	return outValue;
}

technique10 ForwardRender 
<
	string vertexDesc="pos3_tex1";
	string position="float3"; 
	string texcoord0="float2";
>
{
	pass P0
	{
		SetVertexShader( compile vs_4_0 TextureVertexShader() );
		SetGeometryShader( NULL );
		SetPixelShader( compile ps_4_0 TexturePixelShader() );
	}
}

