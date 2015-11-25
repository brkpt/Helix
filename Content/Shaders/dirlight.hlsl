// Use:
//  fxc /T fx_4_0 /Vi /Zi /Fo texture.fxo texture.fx
#include "shared.hlsl"

Texture2D	albedoTexture : register(t0);
Texture2D	normalTexture : register(t1);

struct QuadVS_in
{
	float3 pos : POSITION;
	float2 texuv : TEXCOORD0;
};

struct QuadPS_in
{
	float4 pos : SV_Position;
	float2 texuv : TEXCOORD0;
};

SamplerState texSampler : register(s0) ;
	
QuadPS_in FullScreenQuadVS(QuadVS_in inVert)
{
	QuadPS_in outVert;

	outVert.pos = float4(inVert.pos,1.0);
	outVert.texuv = inVert.texuv;
	return outVert;
}

float4 FullScreenQuadPS(QuadPS_in inVert) : SV_TARGET0
{
	// Get our albedo color
	float3 color = (float3)(albedoTexture.Sample(texSampler,inVert.texuv));
	
	// Start with ambient
	float3 outColor = color*g_ambientColor.xyz;
	
	// Get our pixel normal
	float3 normal = (float3)(normalTexture.Sample(texSampler,inVert.texuv));
	float normLen = length(normal);

	// If we have a normal (ie: something was rendered at this pixel)
	if(normLen > 0)
	{
		// Compare with sun vector
		float3 sunVec = (float3)(mul( float4(g_sunDir.xyz,1), g_mView3x3));
		float dotProd = dot(normal,sunVec);

		// Clamp (0..1)		
		dotProd = clamp(dotProd,0, 1);
		
		// Add in color due to sunlight
		float3 sunVal = color*g_sunColor.xyz*dotProd;
		
		outColor = outColor + sunVal;
	}
	
	return float4(outColor,1);	
}

//technique10 FullScreenQuad
//{
//	pass P0
//	{
//		SetVertexShader( CompileShader( vs_4_0, FullScreenQuadVS() ) );
//		SetGeometryShader( NULL );
//		SetPixelShader( CompileShader( ps_4_0, FullScreenQuadPS() ) );
//	}
//}

