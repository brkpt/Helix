#include "shared.fx"

struct DiffuseVertex_in
{
	float4 pos : POSITION;
	float4 color : COLOR0;
};

struct DiffuseVertex_out
{
	float4 pos : POSITION;
	float4 color : COLOR;
};

DiffuseVertex_out DiffuseVertexShader(DiffuseVertex_in In)
{
	DiffuseVertex_out Out;

	float3 P = mul( In.pos ,WorldView );
	Out.pos = mul( float4(P,1), Projection );
	Out.color = In.color;
	return Out;
}

struct DiffusePixelShader_in
{
	float4 color : COLOR;
};

float4 DiffusePixelShader(DiffusePixelShader_in In) : COLOR
{
	return In.color;
}

technique Diffuse
{
	pass P0
	{
		VertexShader = compile vs_3_0 DiffuseVertexShader();
		PixelShader = compile ps_3_0 DiffusePixelShader();
	}
}

