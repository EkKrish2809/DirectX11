Texture2D shaderTexture: register(t0);
Texture2D depthMapTexture: register(t1);
Texture2D depthMapTexture2: register(t2);
Texture2D depthMapTexture3: register(t3);
SamplerState SampleTypeClamp: register(s0);
SamplerState SampleTypeWrap: register(s1);

struct PixelInputType
{
	float4 position: SV_POSITION;
	float2 tex: TEXCOORD0;
	float3 normal: NORMAL;
	float4 lightViewPosition: TEXCOORD1;
	float3 lightPos: TEXCOORD2;
	float4 lightViewPosition2: TEXCOORD3;
	float3 lightPos2: TEXCOORD4;
	float4 lightViewPosition3: TEXCOORD5;
	float3 lightPos3: TEXCOORD6;
};

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float4 diffuseColor2;
	float4 diffuseColor3;
	float bias;
	float3 padding;
};

float4 main(PixelInputType input): SV_TARGET
{
	float4 color;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float lightIntensity;
	float4 textureColor;

	color = ambientColor; 

	projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.f + 0.5f;
	projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.f + 0.5f;

	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r;

		lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

		lightDepthValue = lightDepthValue - bias;

		if (lightDepthValue < depthValue)
		{
			lightIntensity = saturate(dot(input.normal, input.lightPos));

			if(lightIntensity > 0.f)
			{
				color += (diffuseColor * lightIntensity);

				color = saturate(color);
			}
		}
	}
	
	projectTexCoord.x  = input.lightViewPosition2.x / input.lightViewPosition2.w / 2.f + 0.5f;
	projectTexCoord.y  = -input.lightViewPosition2.y / input.lightViewPosition2.w / 2.f + 0.5f;

	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		depthValue = depthMapTexture2.Sample(SampleTypeClamp, projectTexCoord).r;

		lightDepthValue = input.lightViewPosition2.z / input.lightViewPosition2.w;
		lightDepthValue = lightDepthValue - bias;

		if (lightDepthValue < depthValue)
		{
			lightIntensity = saturate(dot(input.normal, input.lightPos2));
			if (lightIntensity > 0.f)
			{
				color += (diffuseColor2 * lightIntensity);
			}
		}
	}

	projectTexCoord.x  = input.lightViewPosition3.x / input.lightViewPosition3.w / 2.f + 0.5f;
	projectTexCoord.y  = -input.lightViewPosition3.y / input.lightViewPosition3.w / 2.f + 0.5f;

	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		depthValue = depthMapTexture3.Sample(SampleTypeClamp, projectTexCoord).r;

		lightDepthValue = input.lightViewPosition3.z / input.lightViewPosition3.w;
		lightDepthValue = lightDepthValue - bias;

		if (lightDepthValue < depthValue)
		{
			lightIntensity = saturate(dot(input.normal, input.lightPos3));
			if (lightIntensity > 0.f)
			{
				color += (diffuseColor3 * lightIntensity);
			}
		}
	}

	color = saturate(color);

	textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);

	color = color * textureColor;
	
	return color;
}