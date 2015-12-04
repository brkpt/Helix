//Use:
//  fxc /T fx_4_0 /Vi /Zi /Fo texture.fxo texture.fx

#include "shared.hlsl"

Texture2D	albedoTexture :	register(t0) ;
Texture2D	normalTexture :	register(t1) ;
Texture2D	depthTexture  :	register(t2) ;


SamplerState texSampler : register(s0) ;

struct LightSphereVertex_in
{
	float3 pos : POSITION;
};

struct LightSpherePixel_in
{
	float4 pos : SV_POSITION;
	//float3 vEyeToScreen: TEXCOORD0;
};

LightSpherePixel_in LightSphereVertexShader(LightSphereVertex_in inVert)
{
	LightSpherePixel_in outVert;

	float3 viewPos = (float3)(mul( float4(inVert.pos,1), g_mWorldView ));
	float4 projPos = mul( float4(viewPos,1), g_mProjection );
	outVert.pos = projPos;
	
	return outVert;
}


float4 LightSpherePixelShader(LightSpherePixel_in inVert) : SV_Target
{
	// Get z/w from texture
	float3 texPos = int3(int2(inVert.pos.xy), 0);
	float3 samplePos;
	samplePos.x = texPos.x/g_imageWidth;
	samplePos.y = texPos.y/g_imageHeight;
	samplePos.z = 0.0;
	float depth = depthTexture.Load(samplePos).x;
	
	// Convert our x,y back to NDC (inverse viewport transform)
	float halfWidth = g_imageWidth/2.0;
	float halfHeight = g_imageHeight/2.0;
	float clipX = (inVert.pos.x-halfWidth)/halfWidth;
	float clipY = (inVert.pos.y-halfHeight)/-halfHeight;
	float4 clipPos = float4(clipX, clipY, depth, 1.0);
	
	// View space position (inverse proj)
	float4 wPos = mul(clipPos,g_mInvProj);
	float4 viewPos = float4(wPos.xyz/wPos.w,1.0);
	
	// world space position (inverse view)
	float4 worldPos = mul(viewPos,g_mInvView);
	
	// Calculate lighting normal in world space
	float3 posToLight = g_pointLoc.xyz - worldPos.xyz;
	float distToLightSquared = dot(posToLight,posToLight);
	float3 posToLightNorm = normalize(posToLight);
	float3 fragNorm = normalTexture.Load(samplePos).xyz;
	float dotVal = dot(fragNorm,posToLightNorm);

	float3 albedoColor3 = albedoTexture.Sample(texSampler,samplePos.xy).rgb;
	float4 albedoColor4 = float4(albedoColor3,1);
	float3 diffuse3 = (float3)((g_pointColor*dotVal)/distToLightSquared);
	float4 diffuse4 = float4(diffuse3,1);
	
	float4 outColor = albedoColor4*diffuse4;
	
	return outColor;
}

