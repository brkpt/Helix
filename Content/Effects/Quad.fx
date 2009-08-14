// Use:
//  fxc /T fx_4_0 /Vi /Zi /Fo texture.fxo texture.fx

Texture2D	textureImage;

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
	return textureImage.Sample(texSampler,inVert.texuv);
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

