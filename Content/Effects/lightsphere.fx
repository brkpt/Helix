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
	float3 vEyeToScreen: TEXCOORD0;
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
	
	float invW = 1.0/projPos.w;
	float4 wVal = float4(invW,invW,invW,invW);
	float4 clipPos = projPos * wVal;
	outVert.vEyeToScreen = float3(clipPos.x*viewAspect, clipPos.y, invTanHalfFOV);
	return outVert;
}


float4 LightSpherePixelShader(LightSpherePixel_in inVert) : SV_Target
{
	float4 outColor;// = float4(1.0f,1.0f,1.0f,1.0f);
	
	// float3 pixelPos = normalize(Input.vEyeToScreen) * G_Buffer.z;
	
	// Get our depth value
	float clipX = inVert.pos.x / imageWidth;
	float clipY = inVert.pos.y / imageHeight;
	float depthValue = depthTexture.Sample(texSampler,float2(clipX,clipY));
	float3 viewPos = normalize(inVert.vEyeToScreen) * depthValue;
	
	// Transform the light into view space
	float3 pointLocView = mul( float4(0,0,0,1), WorldView );
	float3 posToLight = pointLocView - viewPos;
	float3 posToLightNorm = normalize(posToLight);
	
	float3 posNorm = normalTexture.Sample(texSampler,float2(clipX,clipY));
	float distToLight = length(posToLight);

	float dotVal = dot(posNorm,posToLightNorm)/distToLight;

	float3 albedoColor = albedoTexture.Sample(texSampler,float2(clipX,clipY));
	outColor = float4(albedoColor,1) * float4(pointColor*dotVal,1);
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

