cbuffer cbPerObject
{
	matrix		g_mWorldView;
	matrix		g_mView;
	matrix		g_mInvView;
	matrix		g_mView3x3;
	matrix		g_mViewWorldIT;
	matrix		g_mInvWorldViewProj;
	matrix		g_mInvViewProj;
};

struct TextureVS_in
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texuv : TEXCOORD0;
};

TexturePS_in TextureVertexShader(TextureVS_in In)
{
	TexturePS_in Out;

	Out.normal = In.normal;
	Out.texuv = In.texuv;
	float3 P = mul( float4(In.pos,1), WorldView );
	Out.pos = mul( float4(P,1), Projection );
	Out.depth.xy = Out.pos.zw;						// Send z and w separately to the pixel shader
	return Out;
}
