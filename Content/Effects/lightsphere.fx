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

struct LightSphereVertex_in
{
	float3 pos : POSITION;
};

struct LightSpherePixel_in
{
	float4 pos : POSITION;
	//float3 vEyeToScreen: TEXCOORD0;
};

SamplerState texSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

LightSpherePixel_in LightSphereVertexShader(LightSphereVertex_in inVert)
{
	LightSpherePixel_in outVert;

	float3 viewPos = mul( float4(inVert.pos,1), WorldView );
	float4 projPos = mul( float4(viewPos,1), Projection );
	outVert.pos = projPos;
	
//	float2 clipPos = projPos.xy / projPos.ww;
//	float2 screenPos;
//	screenPos.x = (1024.0/2.0) * clipPos.x + (1024.0/2.0);
//	screenPos.y = (768.0/2.0) * clipPos.y + (768.0/2.0);
//	outVert.vEyeToScreen = float3(screenPos.x*viewAspect, screenPos.y, invTanHalfFOV);
	return outVert;
}


float4 LightSpherePixelShader(LightSpherePixel_in inVert) : SV_Target
{
	float4 outColor;// = float4(1.0f,1.0f,1.0f,1.0f);

	// Convert back to NDC
	float halfWidth = imageWidth/2.0;
	float halfHeight = imageHeight/2.0;
	float4 clipPos = float4((inVert.pos.x-halfWidth)/halfWidth, (inVert.pos.y-halfHeight)/-halfHeight, 0, 1.0);
	
	// Back into post-projected space
	float4 postProj = clipPos * inVert.pos.wwww;
	
	// Back into view space
	float4 viewPos4 = mul(postProj,InvProj);
	
	// Get our depth value
	float2 samplePos = float2(inVert.pos.x / imageWidth, inVert.pos.y / imageHeight);
	float depthValue = depthTexture.Load(int3((int2)inVert.pos.xy,0)).x;

	// Build full view pos of pixel	
	float3 viewPos = float3(viewPos4.xy,depthValue);
		
	// Transform the light into view space
	float3 pointLocView = mul( float4(0,0,0,1), WorldView );
	float3 posToLight = pointLocView - viewPos;
	float3 posToLightNorm = normalize(posToLight);
	float3 posNorm = normalTexture.Load(int3((int2)samplePos.xy,0)).xyz;
	float dotVal = dot(posNorm,posToLightNorm);

	float3 albedoColor = albedoTexture.Sample(texSampler,samplePos).xyz;
	outColor = float4(albedoColor,1) * float4(pointColor*dotVal,1);
	//outColor = float4(1.0,1.0,1.0,1.0);
	return outColor;
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

