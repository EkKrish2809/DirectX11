struct VOut
{
	float4 a_position : SV_POSITION;
	// float4 a_color_out : COLOR;
	float2 a_texcoord_out : TEXCOORD;
};
// in vec4 a_color_out;
// in vec2 a_texcoord_out;
// out vec4 FragColor;
Texture2D u_textureSampler;
SamplerState SampleType;
float4 main(VOut in_) : SV_TARGET
{
	return u_textureSampler.Sample(SampleType, in_.a_texcoord_out);
}
