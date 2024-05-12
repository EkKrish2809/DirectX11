Texture2D diffTexture;
SamplerState SampleType;

struct VOut {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

// float4 main(float4 pos : SV_POSITION, float2 texcoord : TEXCOORD) : SV_TARGET
float4 main(VOut in_) : SV_TARGET
{
	float4 textureColor = diffTexture.Sample(SampleType, in_.texcoord);
	// float4 textureColor = float4(1.0, 0.0, 0.0, 1.0);

	return textureColor;
}
