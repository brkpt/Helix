shared float4x3 WorldView : WORLDVIEW;
shared float4x4 Projection : PROJECTION;

#ifdef DUMMY_SHADER

float4 dummy_vs(float4 pos : POSITION) : POSITION
{
	return pos;
}

float4 dummy_ps(float4 color : COLOR) : COLOR
{
	return color;
}

technique DummyTechnique
{
	pass P0
	{
		VertexShader = compile vs_3_0 dummy_vs();
		PixelShader = compile ps_3_0 dummy_ps();
	}
}
#endif 
