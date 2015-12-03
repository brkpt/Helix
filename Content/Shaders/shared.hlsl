// Constant buffers
//
// 15 buffers
// 1024 constants of 4 elements apiece
cbuffer VSPerFrameConstants : register(b0)
{
	matrix		g_mView			; // c0
	matrix		g_mProjection	; // c4
	matrix		g_mInvView		; // c8
	matrix		g_mView3x3		; // c12
	matrix		g_mInvViewProj	; // c16
	matrix		g_mInvProj		; // c20
}

cbuffer VSPerObjectConstants : register(b1)
{
	matrix		g_mWorldView		; // c0
	matrix		g_mViewWorldIT		; // c4
	matrix		g_mInvWorldViewProj	; // c8
	float		viewAspect			; // c12
};

cbuffer PSPerFrameConstants : register(b2)
{
	float4	g_sunColor		; // c0
	float4	g_sunDir		; // c1
	float4	g_ambientColor	; // c2;
	float	g_cameraNear	; // c3;
	float	g_cameraFar		; // c3;
	float	g_imageWidth	; // c3;
	float	g_imageHeight	; // c3;
	float	g_invTanHalfFOV	; // c4;
};

cbuffer PSPointLight : register(b3)
{
	float4	pointLoc		;
	float4	pointColor		;
	float	lightRadius		;
};


