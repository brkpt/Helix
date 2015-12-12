// ****************************************************************************
// Constant buffers
//
// Pixel Shader model 5:
// 15 buffers
// 1024 constants of 4 elements apiece
// ****************************************************************************
cbuffer VSPerFrameConstants : register(b0)
{
	matrix		g_mView			;
	matrix		g_mProjection	;
	matrix		g_mInvView		;
	matrix		g_mView3x3		;
	matrix		g_mInvViewProj	;
	matrix		g_mInvProj		;
	float4		g_sunColor		;
	float4		g_sunDir		;
	float4		g_ambientColor	;
	float		g_cameraNear	;
	float		g_cameraFar		;
	float		g_imageWidth	;
	float		g_imageHeight	;
	float		g_invTanHalfFOV	;
	float		g_viewAspect	;
}

cbuffer VSPerObjectConstants : register(b1)
{
	matrix		g_mWorldView		; // c0
	matrix		g_mViewWorldIT		; // c4
	matrix		g_mInvWorldViewProj	; // c8
};

cbuffer PSPointLight : register(b2)
{
	float4	g_pointLoc		;
	float4	g_pointColor		;
	float	g_lightRadius		;
};
