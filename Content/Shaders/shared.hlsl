cbuffer VSPerFrameConstants : register(b0)
{
	matrix		g_mView			: packoffset(c0) ;
	matrix		g_mProjection	: packoffset(c4) ;
	matrix		g_mInvView		: packoffset(c8) ;
	matrix		g_mView3x3		: packoffset(c12);
	matrix		g_mInvViewProj	: packoffset(c16);
	matrix		g_mInvProj		: packoffset(c20);
}

cbuffer VSPerObjectConstants : register(b1)
{
	matrix		g_mWorldView			: packoffset(c0) ;
	matrix		g_mViewWorldIT			: packoffset(c4) ;
	matrix		g_mInvWorldViewProj		: packoffset(c8) ;
};

cbuffer PSPerFrameConstants : register(b2)
{
	float3	g_sunColor		: packoffset(c0) ;
	float3	g_sunDir		: packoffset(c4) ;
	float3	g_ambientColor	: packoffset(c8) ;
};


//shared cbuffer shaderCBuffer
//{
//	float4x3		WorldView : WORLDVIEW;
//	float4x4		Projection : PROJECTION;
//	float4x4		View : VIEW;
//	float4x4		InvView;
//	float4x4		View3x3;					// upper 3x3 of the view matrix in a 4x4 matrix
//	float4x4		WorldViewIT;
//	float4x4		InvWorldViewProj;
//	float4x4		InvViewProj;
//	float4x4		InvProj;
//	float3		sunDir;			// Points toward sun
//	float3		sunColor;			// Sunlight color in RGB
//	float3		ambientColor;		// Ambient color in RGB
//};

