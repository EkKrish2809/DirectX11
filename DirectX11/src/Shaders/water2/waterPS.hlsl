// define outputs from vertex shader
struct VpOut
{
    float4 HPos : SV_POSITION;
    float4 Eye : TEXCOORD0;
    float4 Wave0 : TEXCOORD1;
    float2 Wave1 : TEXCOORD2;
    float2 Wave2 : TEXCOORD3;
    float2 Wave3 : TEXCOORD4;
    float4 ScreenPos : TEXCOORD5;
};

Texture2D tex0 : register(t0);
Texture2D tex1 : register(t1);
Texture2D tex2 : register(t2);
SamplerState SampleType;

// This assumes NdotL comes clamped
float Fresnel(float NdotL, float fresnelBias, float fresnelPow)
{
    float facing = (1.0 - NdotL);
    return max(fresnelBias + (1.0 - fresnelBias) * pow(facing, fresnelPow), 0.0);
}

// half4 main(float3 Eye : TEXCOORD0,
//            float4 Wave0 : TEXCOORD1, float2 Wave1 : TEXCOORD2,
//            float2 Wave2 : TEXCOORD3, float2 Wave3 : TEXCOORD4,
//            float4 ScreenPos : TEXCOORD5,
//            uniform sampler2D tex0, uniform sampler2D tex1, uniform sampler2D tex2) : COLOR
float4 main(VpOut input) : SV_TARGET
{

    float3 vEye = normalize(input.Eye);

    // Get bump layers
    //   float3 vBumpTexA = tex2D(tex0, input.Wave0.xy).xyz;
    //   float3 vBumpTexB = tex2D(tex0, input.Wave1.xy).xyz;
    //   float3 vBumpTexC = tex2D(tex0, input.Wave2.xy).xyz;
    //   float3 vBumpTexD = tex2D(tex0, input.Wave3.xy).xyz;
    float3 vBumpTexA = tex0.Sample(SampleType, input.Wave0.xy).xyz;
    float3 vBumpTexB = tex0.Sample(SampleType, input.Wave1.xy).xyz;
    float3 vBumpTexC = tex0.Sample(SampleType, input.Wave2.xy).xyz;
    float3 vBumpTexD = tex0.Sample(SampleType, input.Wave3.xy).xyz;

    // Average bump layers
    float3 vBumpTex = normalize(2.0 * (vBumpTexA.xyz + vBumpTexB.xyz + vBumpTexC.xyz + vBumpTexD.xyz) - 4.0);

    // Apply individual bump scale for refraction and reflection
    float3 vRefrBump = vBumpTex.xyz * float3(0.02, 0.02, 1.0);
    float3 vReflBump = vBumpTex.xyz * float3(0.1, 0.1, 1.0);

    // Compute projected coordinates
    float2 vProj = (input.ScreenPos.xy / input.ScreenPos.w);
    //   float4 vReflection = tex2D(tex2, vProj.xy+ vReflBump.xy);
    float4 vReflection = tex2.Sample(SampleType, vProj.xy + vReflBump.xy);
    //   float4 vRefrA = tex2D(tex1, vProj.xy + vRefrBump.xy);
    float4 vRefrA = tex1.Sample(SampleType, vProj.xy + vRefrBump.xy);
    //   float4 vRefrB = tex2D(tex1, vProj.xy);
    float4 vRefrB = tex1.Sample(SampleType, vProj.xy);

    // Mask occluders from refraction map
    float4 vRefraction = vRefrB * vRefrA.w + vRefrA * (1 - vRefrA.w);

    // Compute Fresnel term
    float NdotL = max(dot(vEye, vReflBump), 0);
    float facing = (1.0 - NdotL);
    float fresnel = Fresnel(NdotL, 0.2, 5.0);

    // Use distance to lerp between refraction and deep water color
    float fDistScale = saturate(10.0 / input.Wave0.w);
    float3 WaterDeepColor = (vRefraction.xyz * fDistScale + (1 - fDistScale) * float3(0, 0.1, 0.125));

    // Lerp between water color and deep water color
    float3 WaterColor = float3(0, 0.1, 0.15);
    float3 waterColor = (WaterColor * facing + WaterDeepColor * (1.0 - facing));
    float3 cReflect = fresnel * vReflection;

    // final water = reflection_color * fresnel + water_color
    return float4(cReflect + waterColor, 1);
}