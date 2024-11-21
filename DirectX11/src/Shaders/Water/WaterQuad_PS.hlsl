Texture2D u_textureReflectionSampler : register(t0);
Texture2D u_textureRefractionSampler : register(t1);
Texture2D u_textureWaterDUDVMap : register(t2);
Texture2D u_waterNormalMapTexture : register(t3);
SamplerState SampleType;

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
	float2 ndTexcoords : TEXCOORD1;
	float3 WorldPos : TEXCOORD2;
};

static const float waveStrength = 0.02; // 0.08
static const float shininess = 20.0;// 5.0
static const float reflectivity = 0.5; // 0.2


float4 main(VOut in_):SV_TARGET
{
	// Convert Clip Space Co-ordinates to Screen Space Co-ordinates
	// float2 screenSpaceCoords;
	// screenSpaceCoords.x = in_.clipSpaceCoord.x / in_.clipSpaceCoord.w;
	// screenSpaceCoords.y = in_.clipSpaceCoord.y / in_.clipSpaceCoord.w;

	// convert screen-space coordinates to Normalize Device Coordinates
	float2 ndcCoords;
	// ndcCoords.x = (in_.clipSpaceCoord.x * 0.5 + 0.5);
	// ndcCoords.y = (in_.clipSpaceCoord.y * 0.5 + 0.5);
	// ndcCoords.x = (2.0 * (screenSpaceCoords.x / 1920)) -1.0;
	// ndcCoords.y = 1.0 - (2.0 * (screenSpaceCoords.y / 1080));
	// ndcCoords = (screenSpaceCoords / 2.0 + 0.5) * float2(1920, 1080);
	ndcCoords = (in_.clipSpaceCoord.xy/in_.clipSpaceCoord.w) * 0.5f + float2(0.5f, 0.5f);

	// float2 refractTexcoords = (in_.clipSpaceCoord.xy / in_.clipSpaceCoord.w) * 0.5 + 0.5;
	// float2 reflectTexcoords =  float2(refractTexcoords.x, 1.0f - refractTexcoords.y);

	// // calculate total distortion
	// float2 distortion1 = (u_textureWaterDUDVMap.Sample(SampleType, float2(in_.a_texcoord_out.x + u_moveFactorOffset, in_.a_texcoord_out.y)).rg * 2.0 - 1.0) * waveStrength;
	// float2 distortion2 = (u_textureWaterDUDVMap.Sample(SampleType, float2(-in_.a_texcoord_out.x + u_moveFactorOffset, in_.a_texcoord_out.y + u_moveFactorOffset)).rg * 2.0 - 1.0) * waveStrength;
	// float2 totalDistortions = distortion1 + distortion2;

	// calculate total distortion = version 2
	float2 distortedTexcoords = u_textureWaterDUDVMap.Sample(SampleType, float2(in_.a_texcoord_out.x + u_moveFactorOffset, in_.a_texcoord_out.y)).rg;// * 0.1;
	distortedTexcoords = in_.a_texcoord_out + float2(distortedTexcoords.x, distortedTexcoords.y + u_moveFactorOffset);
	float2 totalDistortions = (u_textureWaterDUDVMap.Sample(SampleType, distortedTexcoords).rg * 2.0 - 1.0) * waveStrength;

	float2 reflectTexcoords =  float2(ndcCoords.x, -ndcCoords.y);
	reflectTexcoords = reflectTexcoords + totalDistortions;
	reflectTexcoords.x = clamp(reflectTexcoords.x, 0.001, 0.999);
	reflectTexcoords.y = clamp(reflectTexcoords.y, -0.999, -0.001);
	reflectTexcoords = saturate(reflectTexcoords);
	float4 reflectColor = u_textureReflectionSampler.Sample(SampleType, reflectTexcoords);

	float2 refractTexcoords =  float2(ndcCoords.x, ndcCoords.y);
	refractTexcoords = refractTexcoords + totalDistortions;
	refractTexcoords = clamp(refractTexcoords, 0.001, 0.999);
	// refractTexcoords.y = clamp(refractTexcoords.y, 0.001, 0.999);
	refractTexcoords = saturate(refractTexcoords);
	float4 refractColor = u_textureRefractionSampler.Sample(SampleType, refractTexcoords);
	
	// For FRESNEL EFFECT
	float3 viewVector = normalize(in_.toCameraVector);
	// float3 waterSurfaceNormal = float3(0.0, 1.0, 0.0);
	float3 waterSurfaceNormal = u_waterNormalMapTexture.Sample(SampleType, totalDistortions).xyz;
	float refractiveFactor = dot(viewVector, waterSurfaceNormal);
	refractiveFactor = pow(refractiveFactor, 2.0); // 
	refractiveFactor = clamp(refractiveFactor, 0.0, 1.0);

	float4 color = lerp(reflectColor, refractColor, refractiveFactor);
	// float4 color = lerp(reflectColor, refractColor, 0.5);

	// for Specular Highlights
	float4 normalMapColor = u_waterNormalMapTexture.Sample(SampleType, totalDistortions);
	// float4 normalMapColor = u_waterNormalMapTexture.Sample(SampleType, float2(0.0, 0.0));
	float3 normal = float3(normalMapColor.r * 2.0 -1.0, normalMapColor.b * 3.0, normalMapColor.g * 2.0 - 1.0);
	normal = normalize(normal);

	// float3 halfVector = normalize(normalize(in_.toCameraVector) + normalize(in_.lightDirection));
    // float specular = max(dot(halfVector, normal), 0.0f);
	float3 reflectedLight = reflect(normalize(in_.lightDirection), normal);
	float specular = max(dot(reflectedLight, viewVector), 0.0);
	specular = pow(specular,shininess);
	float3 specularHighlights = u_lightColor * specular * reflectivity;
	// float3 specularHighlights = float3(1.0, 1.0, 1.0) * specular * reflectivity;

	// return lerp(color, float4(0.0, 0.3, 0.5, 0.0), 0.2) + float4(specularHighlights, 0.0);
	return lerp(color, float4(0.22, 0.82, 1.0, 1.0), 0.2) + float4(specularHighlights, 0.0);
	// return color;
	// return float4(reflectColor.rgb, 1.0f);
	// return reflectColor + float4(specularHighlights, 0.0);
}
