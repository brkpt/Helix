shared cbuffer shaderCBuffer
{
float4x3	WorldView : WORLDVIEW;
float4x4	Projection : PROJECTION;
float4x4	View : VIEW;
float4x4	View3x3;					// upper 3x3 of the view matrix in a 4x4 matrix
float4x4	WorldViewIT;
float3		sunDir;						// Points toward sun
float3		sunColor;					// Sunlight color in RGB
float3		ambientColor;				// Ambient color in RGB
};

