//Use:
//  fxc /T fx_4_0 /Vi /Zi /Fo texture.fxo texture.fx

#include "shared.hlsl"

struct QuadVS_in
{
	float3 pos : POSITION;
	float2 texuv : TEXCOORD0;
};

struct QuadPS_in
{
	float4 pos : SV_Position;
	float2 texuv : TEXCOORD0;
	float3 vEyeToScreen: TEXCOORD1;
};

Texture2D	albedoTexture : register(t0) ;
Texture2D	depthTexture : register(t1) ;
Texture2D	normalTexture :	register(t2);

SamplerState colorSampler : register(s0) ;
SamplerState depthSampler : register(s1) ;
SamplerState normalSampler : register(s2) ;

QuadPS_in FullScreenQuadVS(QuadVS_in inVert)
{
	QuadPS_in outVert;

	outVert.pos = float4(inVert.pos,1.0);
	outVert.texuv = inVert.texuv;
	//out.vEyetoScreen= float3(Input.ScreenPos.x * ViewAspect, Input.ScreenPos.y, invTanHalfFOV)
	outVert.vEyeToScreen = float3(inVert.pos.x*viewAspect, inVert.pos.y, g_invTanHalfFOV);
	return outVert;
}

float4 FullScreenQuadPS(QuadPS_in inVert) : SV_Target
{
//	float4 outColor;
//	
//	// float3 pixelPos = normalize(Input.vEyeToScreen) * G_Buffer.z;
//	
//	// Get our depth value
//	float clipX = inVert.pos.x / imageWidth;
//	float clipY = inVert.pos.y / imageHeight;
//	float depthValue = depthTexture.Sample(depthSampler,float2(clipX,clipY)).x;
//	float3 viewPos = normalize(inVert.vEyeToScreen) * depthValue;
//	
//	// Transform the light into view space
//	float3 pointLocView = (float3)(mul( pointLoc, g_mWorldView ));
//	float3 posToLight = pointLocView - viewPos;
//	float3 posToLightNorm = normalize(posToLight);
//	
//	float3 posNorm = normalTexture.Sample(normalSampler,float2(clipX,clipY)).xyz;
//	float distToLight = length(posToLight);
//
//	float dotVal = dot(posNorm,posToLightNorm);
//
//	float3 albedoColor = albedoTexture.Sample(colorSampler,float2(clipX,clipY)).xyz;
//	outColor = float4(albedoColor,1) * (pointColor*dotVal)/distToLight;
//	return outColor;

	// Incoming x/y is in screen space of width x height
	// Convert to clip space coordinates
	float clipX = inVert.pos.x / g_imageWidth;
	float clipY = inVert.pos.y / g_imageHeight;

	// Sample albedo texture
	float3 color = albedoTexture.Sample(colorSampler, float2(clipX, clipY)).xyz;
	float4 outColor;
	outColor = float4(color,1.0f);
	return outColor;
}


