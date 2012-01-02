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
