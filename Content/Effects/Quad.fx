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
	float4 ambient = { 0.1, 0.1, 0.1, 1 };
	float4 color = albedoTexture.Sample(texSampler,inVert.texuv);
	
	float3 normal = normalTexture.Sample(texSampler,inVert.texuv);
	float normLen = length(normal);
	
	float3 sunVec = mul( float4(sunlight,1), View3x3);
	float4 outColor = color*ambient;
	
	if(normLen > 0)
	{
		float dotProd = dot(normal,sunVec);
		
		if(dotProd < 0)
		{
			dotProd = 0;
		}

		outColor = outColor + color*dotProd;
		//outColor = float4(sunVec,1);
	}
	
	return outColor;	
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

