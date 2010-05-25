//Use:
//  fxc /T fx_4_0 /Vi /Zi /Fo texture.fxo texture.fx
#include "shared.fx"

float3		pointLoc;			// Point light location
float3		pointColor;			// Point light color
float		pointRadius;		// Point light radius
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
	float4 pos : SV_POSITION;
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
	
	return outVert;
}


float4 LightSpherePixelShader(LightSpherePixel_in inVert) : SV_Target
{
	// Get z/w from texture
	int3 samplePos = int3(int2(inVert.pos.xy), 0);
	float depth = depthTexture.Load(samplePos).x;
	
	// Convert our x,y back to NDC (inverse viewport transform)
	float halfWidth = imageWidth/2.0;
	float halfHeight = imageHeight/2.0;
	float clipX = (inVert.pos.x-halfWidth)/halfWidth;
	float clipY = (inVert.pos.y-halfHeight)/-halfHeight;
	float4 clipPos = float4(clipX, clipY, depth, 1.0);
	
	// View space position (inverse proj)
	float4 wPos = mul(clipPos,InvProj);
	float4 viewPos = float4(wPos.xyz/wPos.w,1.0);
	
	// world space position (inverse view)
	float4 worldPos = mul(viewPos,InvView);
	
	// Calculate lighting normal in world space
	float3 posToLight = pointLoc - worldPos.xyz;
	float3 posToLightNorm = normalize(posToLight);
	float3 fragNorm = normalTexture.Load(samplePos).xyz;
	float dotVal = dot(fragNorm,posToLightNorm);

	float3 albedoColor = albedoTexture.Sample(texSampler,samplePos.xy).rgb;
	float4 outColor = float4(albedoColor,1) * float4(pointColor*dotVal,1);
	
	outColor = dotVal; //float4(1.0,1.0,1.0,1.0);
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

