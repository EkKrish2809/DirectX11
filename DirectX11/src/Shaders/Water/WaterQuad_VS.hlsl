cbuffer ConstantBuffer : register(b0)
{
	float u_moveFactorOffset;
	float4x4 u_worldMatrix;
	float4x4 u_viewMatrix;
	float4x4 u_projectionMatrix;
	float3 u_cameraPosition;
	float3 u_lightPosition;
	float3 u_lightColor;
};

struct VOut
{
	float2 a_texcoord_out : TEXCOORD;
	float4 clipSpaceCoord : SV_POSITION;
	float3 toCameraVector : CAMVECTOR;
	float3 lightDirection : LIGHTDIR;
};


static const float tiling = 5.0;

VOut main(float2 a_position : POSITION)
{
	VOut output;

	float4 worldPosition = mul(u_worldMatrix, float4(a_position.x, 0.0, a_position.y, 1.0));

	output.clipSpaceCoord = mul(u_worldMatrix, float4(a_position.x, 0.0, a_position.y, 1.0));
	output.clipSpaceCoord = mul(u_viewMatrix, output.clipSpaceCoord);
	output.clipSpaceCoord = mul(u_projectionMatrix, output.clipSpaceCoord);

	output.a_texcoord_out = float2(a_position.x/2.0 + 0.5, a_position.y/2.0 + 0.5) * tiling;
	output.a_texcoord_out.x = clamp(output.a_texcoord_out.x, 0.0f, 1.0f);
	output.a_texcoord_out.y = clamp(output.a_texcoord_out.y, 0.0f, 1.0f);

	output.toCameraVector = u_cameraPosition - worldPosition.xyz;

	// calculate light direction
	output.lightDirection = worldPosition.xyz - u_lightPosition;

	return output;
}

