const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

cbuffer ConstantBuffer
{
	float4x4 World;
	float4x4 View;
	float4x4 Projection;
	float4x4 finalBonesMatrices[MAX_BONES];
}

struct VOut {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

// VOut main(float4 pos : POSITION, float2 texcoord : TEXCOORD)
VOut main(float4 pos : POSITION, float4 norm : NORMALS, float2 texcoord : TEXCOORD, 
			float4 tangent : TANGENT, float4 bitTangent : BITTANGENT, float4 boneIds : BONEIDS, float4 weights : WEIGHTS)
{
	VOut output;

	output.pos = mul(pos, World);
	output.pos = mul(output.pos, View);
	output.pos = mul(output.pos, Projection);
	output.texcoord = texcoord;

	return output;
}

