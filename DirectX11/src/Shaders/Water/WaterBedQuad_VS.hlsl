cbuffer ConstantBuffer_WaterBed
{
	float4x4 u_worldMatrix;
	float4x4 u_viewMatrix;
	float4x4 u_projectionMatrix;
}


// in vec4 a_position;
// //in vec4 a_color;
// in vec2 a_texcoord;

// uniform mat4 u_modelMatrix;
// uniform mat4 u_viewMatrix;
// uniform mat4 u_projectionMatrix;
struct VOut
{
	float4 a_position : SV_POSITION;
	// float4 a_color_out : COLOR;
	float2 a_texcoord_out : TEXCOORD;
};

VOut main(float4 a_position : POSITION, float2 a_texcoord : TEXCOORD)
{
	VOut output;

	float4 worldPosition = float4(0.0, 0.0, 0.0, 0.0);
	worldPosition = mul(worldPosition, float4(a_position.x, 0.0, a_position.y, 1.0));
	output.a_position = mul(worldPosition, u_worldMatrix);
	output.a_position = mul(output.a_position, u_viewMatrix);
	output.a_position = mul(output.a_position, u_projectionMatrix);

	// gl_Position = u_projectionMatrix * u_viewMatrix * u_modelMatrix * vec4(a_position.x, 0.0, a_position.y, 1.0);
	//a_color_out = a_color;
	output.a_texcoord_out = a_texcoord;
	return output;
}