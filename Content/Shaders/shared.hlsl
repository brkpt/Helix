cbuffer VSPerFrameConstants
{
	float4x4		g_mView;
	float4x4		g_mProjection;
	float4x4		g_mInvView;
	float4x4		g_mView3x3;
	float4x4		g_mInvViewProj;
	float4x4		g_mInvProj;
}

cbuffer VSPerObjectConstants
{
	float4x3		g_mWorldView;
	float4x4		g_mViewWorldIT;
	float4x4		g_mInvWorldViewProj;
};

cbuffer PSPerFrameConstants
{
	float3	g_sunColor;
	float3	g_sunDir;
	float3	g_ambientColor;
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

