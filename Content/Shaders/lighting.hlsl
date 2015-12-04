// Performs all lighting for the scene

// To compile offline:
//  fxc /T fx_5_0 /Vi /Zi /Fo texture.fxo texture.fx

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
Texture2D	normalTexture :	register(t1);
Texture2D	depthTexture : register(t2) ;

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

float4 ShowGBuffer(QuadPS_in inVert)
{
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

float4 ShowNormal(QuadPS_in inVert)
{
	// Get our pixel normal
	float3 normal = (float3)(normalTexture.Sample(colorSampler,inVert.texuv));
	float normLen = length(normal);
}

float4 ShowDepth(QuadPS_in inVert)
{
	float4 outColor = float4(0.5f, 0.0f, 0.5f, 1.0f);
	return outColor;
}

float4 Ambient(QuadPS_in inVert)
{
	// Get our depth value
	float3 albedoColor = (float3)(albedoTexture.Sample(colorSampler,inVert.texuv));
	
	float4 outColor = float4(albedoColor * g_ambientColor.xyz,1.0);
	return outColor;
}

// lighting due to directional sunlight only
float4 Sunlight(QuadPS_in inVert)
{
	// Convert screen coords back into clip space coords
	float clipX = inVert.pos.x / g_imageWidth;
	float clipY = inVert.pos.y / g_imageHeight;

	// Get our albedo color
	float3 color = (float3)(albedoTexture.Sample(colorSampler,float2(clipX, clipY)));

	// Default to albedo color if no normal exists
	float3 outColor = color;
	
	// Start with ambient
//	float3 outColor = color*g_ambientColor.xyz;
	
	// Get our pixel normal
	float3 normal = (float3)(normalTexture.Sample(normalSampler,float2(clipX, clipY)));
	float normLen = length(normal);

	// If we have a normal (ie: something was rendered at this pixel)
	if(normLen > 0)
	{
		// Compare with sun vector
		//float3 sunVec = (float3)(mul( float4(g_sunDir.xyz,1), g_mView3x3));
		float dotProd = dot(normal,(float3)g_sunDir);

		// Clamp (0..1)		
		dotProd = clamp(dotProd,0, 1);
	
		// Color due to sunlight
		outColor = color*g_sunColor.xyz*dotProd;

// 		// Add in color due to sunlight
// 		float3 sunVal = color*g_sunColor.xyz*dotProd;
// 		
// 		float3 outColor = outColor + sunVal;
	}
	
	return float4(outColor,1);	
}

float4 PointLighting(QuadPS_in inVert)
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
	float4 outColor = float4(0.5f, 0.0f, 0.5f, 1.0f);
	return outColor;
}

float4 FullScreenQuadPS(QuadPS_in inVert) : SV_Target
{
	// Output type
	// GBuffer only = 0
	// Ambient only lighting = 1
	// Sunlight only lighting = 2
	// Pointligh only lighting = 3
	//
	// Debugging 
	// Normal buffer = 100
	// Depth buffer = 101
	int lightingType = 2;

	// Default to a purple for debugging purposes
	float4 outColor = float4(0.5f, 0.0f, 0.5f, 1.0f);

	switch(lightingType)
	{
		case 0:
		{
			outColor = ShowGBuffer(inVert);
		}
		break;

		case 1:
		{
			outColor = Ambient(inVert);
		}
		break;

		case 2:
		{
			outColor = Sunlight(inVert);
		}
		break;

		case 100:
		{
			outColor = ShowNormal(inVert);
		}
		break;
	}

	return outColor;
}


