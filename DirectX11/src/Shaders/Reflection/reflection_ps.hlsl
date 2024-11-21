Texture2D shaderTexture : register(t0);
Texture2D reflectionTexture : register(t1);
Texture2D u_textureWaterDUDVMap : register(t2);
Texture2D normalTexture : register(t3);
SamplerState SampleType : register(s0);

cbuffer ReflectionBuffer
{
    float moveFactorOffset;
    matrix reflectionMatrix;
    float4 ld;
    float4 kd;
    float4 lightPosition;
    float4 cameraPosition;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 reflectionPosition : TEXCOORD1;
    float3 toCameraVector : CAMVECTOR;
	float3 lightDirection : LIGHTDIR;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 main(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float2 reflectTexCoord;
    float4 reflectionColor;
    float2 refractTexCoord;
    float4 refractionColor;
    float4 color;
    float u_moveFactorOffset = 0.4;
    float waveStrength = 0.02; // 0.08
    float shininess = 20.0;// 5.0
    float reflectivity = 0.5; // 0.2

    // Sample the texture pixel at this location.
    // textureColor = shaderTexture.Sample(SampleType, input.tex);

    // Calculate the projected reflection texture coordinates.
    reflectTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w / 2.0f + 0.5f;
    reflectTexCoord.y = -input.reflectionPosition.y / input.reflectionPosition.w / 2.0f + 0.5f;

    // calculate total distortion = version 2
	float2 distortedTexcoords = u_textureWaterDUDVMap.Sample(SampleType, float2(input.tex.x + moveFactorOffset, input.tex.y)).rg;// * 0.1;
	distortedTexcoords = input.tex + float2(distortedTexcoords.x, distortedTexcoords.y + moveFactorOffset);
	float2 totalDistortions = (u_textureWaterDUDVMap.Sample(SampleType, distortedTexcoords).rg * 2.0 - 1.0) * waveStrength;

    // Sample the texture pixel from the reflection texture using the projected texture coordinates.
    reflectTexCoord = reflectTexCoord + totalDistortions;
    reflectionColor = reflectionTexture.Sample(SampleType, reflectTexCoord);

    refractTexCoord = float2(reflectTexCoord.x, -reflectTexCoord.y) + totalDistortions;
    // reflectionColor = reflectionTexture.Sample(SampleType, reflectTexCoord);
    textureColor = shaderTexture.Sample(SampleType, refractTexCoord);

    // normal mapping
    float3 viewVector = normalize(float3(input.toCameraVector.y, input.toCameraVector.y, input.toCameraVector.y));
    float4 normalMapColor = normalTexture.Sample(SampleType, totalDistortions);
    float3 normal = float3(normalMapColor.r * 2.0 -1.0, normalMapColor.b * 3.0, normalMapColor.g * 2.0 - 1.0);
	normal = normalize(normal);

    float3 reflectedLight = reflect(normalize(input.lightDirection), normal);
	float specular = max(dot(reflectedLight, viewVector), 0.0);
	specular = pow(specular,shininess);
	float3 specularHighlights = float3(1.0,1.0,1.0) * specular * reflectivity;

    // FRESNEL 
    float3 waterSurfaceNormal = normalTexture.Sample(SampleType, totalDistortions).xyz;
	float refractiveFactor = dot(viewVector, waterSurfaceNormal);
	refractiveFactor = pow(refractiveFactor, 2.0); // 
	refractiveFactor = clamp(refractiveFactor, 0.0, 1.0);

    // Do a linear interpolation between the two textures for a blend effect.
    color = lerp(textureColor, reflectionColor, refractiveFactor);
    color =  lerp(color, float4(0.0, 0.0, 0.0, 0.0), 0.2) + float4(specularHighlights, 0.0);
    // color = reflectionColor;

    return color;
}
