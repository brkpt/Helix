// Performs all lighting for the scene

// To compile offline:
//  fxc /T fx_5_0 /Vi /Zi /Fo texture.fxo texture.fx

#include "shared.hlsl"

// ****************************************************************************
// Vertex/pixel definitions
// ****************************************************************************
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

// ****************************************************************************
// Textures/samplers
// ****************************************************************************
Texture2D	albedoTexture : register(t0) ;
Texture2D	normalTexture :	register(t1);
Texture2D	depthTexture : register(t2) ;

SamplerState colorSampler : register(s0) ;
SamplerState depthSampler : register(s1) ;
SamplerState normalSampler : register(s2) ;

// ****************************************************************************
// Vertex shader
// ****************************************************************************
QuadPS_in FullScreenQuadVS(QuadVS_in inVert)
{
	QuadPS_in outVert;

	outVert.pos = float4(inVert.pos,1.0);
	outVert.texuv = inVert.texuv;
	//out.vEyetoScreen= float3(Input.ScreenPos.x * ViewAspect, Input.ScreenPos.y, invTanHalfFOV)
	outVert.vEyeToScreen = float3(inVert.pos.x*viewAspect, inVert.pos.y, g_invTanHalfFOV);
	return outVert;
}


// ****************************************************************************
// Computes the ambient layer
// ****************************************************************************
float4 Ambient(float2 clip, QuadPS_in inVert)
{
	// Get our depth value
	float3 albedoColor = (float3)(albedoTexture.Sample(colorSampler, clip));
	
	float4 outColor = float4(albedoColor * g_ambientColor.xyz,1.0);
	return outColor;
}

// ****************************************************************************
// lighting due to directional sunlight only
// ****************************************************************************
float4 Sunlight(float2 clip, QuadPS_in inVert)
{

	// Get our albedo color
	float3 color = (float3)(albedoTexture.Sample(colorSampler,clip));

	// Default to albedo color if no normal exists
	float3 outColor = color;
	
	// Start with ambient
//	float3 outColor = color*g_ambientColor.xyz;
	
	// Get our pixel normal
	float3 normal = (float3)(normalTexture.Sample(normalSampler,clip));
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

// ****************************************************************************
// Renders a single point light
// ****************************************************************************
float4 Pointlight(float2 clip, QuadPS_in inVert)
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

// ****************************************************************************
// ****************************************************************************
float4 ShowGBuffer(float2 clip, QuadPS_in inVert)
{
	float3 albedoColor = (float3)(albedoTexture.Sample(colorSampler, clip));
	return float4(albedoColor,1);
}

// ****************************************************************************
// Deferred pixel shader
//
// Renders a single quad to the screen using the GBuffer, normal buffer, and
// depth buffer to calculate lighting at each screen position.
//
// The pixel position input should use the SV_Position semantic to denote that
// the value should be in screen space. This causes the hardware to call the
// pixel shader with x/y values that range between 0..image width/height.
// ****************************************************************************
float4 FullScreenQuadPS(QuadPS_in inVert) : SV_Target
{
	// Output type
	// GBuffer only					= 0
	// Ambient only lighting		= 1
	// Sunlight only lighting		= 2
	// Sunlight + Ambient lighting	= 3
	// Pointlight					= 4
	int lightingType = 3;

	// Default to a purple for debugging purposes
	float4 outColor = float4(0.5f, 0.0f, 0.5f, 1.0f);

	// Convert screen coords back into clip space coords
	float clipX = inVert.pos.x / g_imageWidth;
	float clipY = inVert.pos.y / g_imageHeight;
	float2 clip = float2(clipX, clipY);

	switch(lightingType)
	{
		case 0:
		{
			outColor = ShowGBuffer(clip, inVert);
		}
		break;

		case 1:
		{
			outColor = Ambient(clip, inVert);
		}
		break;

		case 2:
		{
			outColor = Sunlight(clip, inVert);
		}
		break;
		
		case 3:
		{
			outColor = Ambient(clip, inVert);
			outColor += Sunlight(clip, inVert);
		}
		break;

		case 4:
		{
			outColor = Pointlight(clip, inVert);
		}
		break;
	}

	return outColor;
}


