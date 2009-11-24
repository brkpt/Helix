//Use:
//  fxc /T fx_4_0 /Vi /Zi /Fo texture.fxo texture.fx
#include "shared.fx"

float3		pointLoc;			// Point light location
float3		pointColor;			// Point light color
float		cameraNear;
float		cameraFar;
float		imageWidth;
float		imageHeight;
float		viewAspect;
float		invTanHalfFOV;
Texture2D	albedoTexture;
Texture2D	normalTexture;
Texture2D	depthTexture;

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

SamplerState texSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};
	
QuadPS_in FullScreenQuadVS(QuadVS_in inVert)
{
	QuadPS_in outVert;

	outVert.pos = float4(inVert.pos,1.0);
	outVert.texuv = inVert.texuv;
	//out.vEyetoScreen= float3(Input.ScreenPos.x * ViewAspect, Input.ScreenPos.y, invTanHalfFOV)
	outVert.vEyeToScreen = float3(inVert.pos.x*viewAspect, inVert.pos.y, invTanHalfFOV);
	return outVert;
}

float4 FullScreenQuadPS(QuadPS_in inVert) : SV_Target
{
	float4 outColor;
	
	// float3 pixelPos = normalize(Input.vEyeToScreen) * G_Buffer.z;
	
	// Get our depth value
	float clipX = inVert.pos.x / imageWidth;
	float clipY = inVert.pos.y / imageHeight;
	float depthValue = depthTexture.Sample(texSampler,float2(clipX,clipY));
	float3 viewPos = normalize(inVert.vEyeToScreen) * depthValue;
	
	// Transform the light into view space
	float3 pointLocView = mul( float4(pointLoc,1), WorldView );
	float3 posToLight = pointLocView - viewPos;
	float3 posToLightNorm = normalize(posToLight);
	
	float3 posNorm = normalTexture.Sample(texSampler,float2(clipX,clipY));
	float distToLight = length(posToLight);

	float dotVal = dot(posNorm,posToLightNorm)/distToLight;

	float3 albedoColor = albedoTexture.Sample(texSampler,float2(clipX,clipY));
	return outColor = float4(albedoColor,1) * float4(pointColor*dotVal,1);
	
}

technique10 FullScreenQuad
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, FullScreenQuadVS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, FullScreenQuadPS() ) );
	}
}

