// Use:
//  fxc [/LD] /T fx_4_0 /Vi /Zi /Fo texture.fxo texture.fx

#include "shared.hlsl"

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
	float2 depth : TEXCOORD1;
};

struct TexturePS_out
{
	float4 color : SV_TARGET0;
	float4 normal : SV_TARGET1;
	float depth : SV_TARGET2;
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

	Out.normal = In.normal;
	Out.texuv = In.texuv;
	float3 P = mul( float4(In.pos,1), g_mWorldView );
	Out.pos = mul( float4(P,1), g_mProjection );
	Out.depth.xy = Out.pos.zw;						// Send z and w separately to the pixel shader
	return Out;
}

TexturePS_out TexturePixelShader(TexturePS_in In) 
{
	TexturePS_out outValue;
	outValue.color = textureImage.Sample(texSampler, In.texuv);
	outValue.normal = float4(In.normal.xyz,0);
	outValue.depth = In.depth.x/In.depth.y;			// Store z/w in the depth buffer
	return outValue;
}

///technique10 DeferredRender 
//<
//	string vertexDesc="pos3_norm3_tex1";
//	string position="float3";
//	string normal="float3"; 
//	string texcoord0="float2";
//>
//{
//	pass P0
//	{
//		SetVertexShader( compile vs_4_0 TextureVertexShader() );
//		SetGeometryShader( NULL );
//		SetPixelShader( compile ps_4_0 TexturePixelShader() );
//	}
//}

