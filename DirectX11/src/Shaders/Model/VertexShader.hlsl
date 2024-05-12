cbuffer ConstantBuffer
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
}

struct VOut {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

VOut main(float4 pos : POSITION, float2 texcoord : TEXCOORD)
{
	VOut output;

	output.pos = mul(pos, World);
	output.pos = mul(output.pos, View);
	output.pos = mul(output.pos, Projection);
	output.texcoord = texcoord;

	return output;
}

