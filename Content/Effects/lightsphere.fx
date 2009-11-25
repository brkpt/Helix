#include "shared.fx"

struct LightSphereVertex_in
{
	float4 pos : POSITION;
};

struct LightSpherePixel_in
{
	float4 pos : POSITION;
};

LightSpherePixel_in LightSphereVertexShader(LightSphereVertex_in In)
{
	LightSpherePixel_in Out;

	float3 P = mul( In.pos ,WorldView );
	Out.pos = mul( float4(P,1), Projection );
	return Out;
}


float4 LightSpherePixelShader(LightSpherePixel_in In) : COLOR
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

technique10 DeferredRender
<
	string vertexDesc="pos3";
	string position="float3";
>
{
	pass P0
	{
		VertexShader = compile vs_4_0 LightSphereVertexShader();
		SetGeometryShader( NULL );
		PixelShader = compile ps_4_0 LightSpherePixelShader();
	}
}

