// Use:
//  fxc /T fx_4_0 /Vi /Zi /Fo texture.fxo texture.fx

#include "shared.fx"

Texture2D	albedoTexture;
Texture2D	normalTexture;

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
	return outVert;
}

float4 FullScreenQuadPS(QuadPS_in inVert) : SV_Target
{
	// Get our albedo color
	float3 color = albedoTexture.Sample(texSampler,inVert.texuv);
	
	// Start with ambient
	float3 outColor = color*ambientColor;
	
	// Get our pixel normal
	float3 normal = normalTexture.Sample(texSampler,inVert.texuv);
	float normLen = length(normal);

	// If we have a normal (ie: something was rendered at this pixel)
	if(normLen > 0)
	{
		// Get a vector from the position to the point light
		float3 lightVecWorld = pointLoc - inVert.pos;
		float lightDist = length(lightVecWorld);
		if(lightDist > 0)
		{
			float rcpSqrtLen = rsqrt(lightDist);
			float3 lightVecWorldNorm = normalize(lightVecWorld);
			
			// Compare with point vector
			float3 lightVecView = mul( float4(lightVecWorldNorm,1), View3x3);
			float dotProd = dot(normal,lightVecView);

			// Clamp (0..1)		
			dotProd = clamp(dotProd,0, 1);
			
			// Add in color due to sunlight
			float3 lightVal = color*pointColor*rcpSqrtLen*dotProd;
			outColor = outColor + lightVal;
		}
	}
	
	return float4(outColor,1);	
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

