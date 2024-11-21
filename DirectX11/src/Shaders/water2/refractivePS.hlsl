///////////////////////////////////////////////////////////////////////////////////////////////////
//  Proj : GPU GEMS 2 DEMOS
//  File : RefractiveFP.cg
//  Desc : Generic refraction fragment program. Simulates glass look (no lighting)
///////////////////////////////////////////////////////////////////////////////////////////////////

// define outputs from vertex shader
struct VpOut
{
    float4 HPos : SV_POSITION;
    float2 BaseUV : TEXCOORD0;
    float4 ScreenPos : TEXCOORD1;
    float3 Eye : TEXCOORD2;
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

// float4 main(float2 BaseUV : TEXCOORD0,
//            float4 ScreenPos : TEXCOORD1,
//            float3 Eye : TEXCOORD2,
//            uniform sampler2D tex0,
//            uniform sampler2D tex1,
//            uniform sampler2D tex2,
//            uniform sampler2D tex3) : COLOR
float4 main(VpOut input) : SV_TARGET
{
    float3 vEye = normalize(input.Eye.xyz);

    // Get bump and apply scale, then get diffuse
    //   float4 vBumpTex = 2.0 * tex2D(tex1, input.BaseUV.xy) - 1.0;
    float4 vBumpTex = 2.0 * tex1.Sample(SampleType, input.BaseUV.xy) - 1.0;
    float3 vBump = normalize(vBumpTex.xyz * float3(0.2, 0.2, 1.0));
    float4 vDiffuse = tex0.Sample(SampleType, input.BaseUV.xy);

    // Compute reflection vector
    float LdotN = dot(vBump.xyz, vEye.xyz);
    float3 vReflect = 2.0 * LdotN * vBump.xyz - vEye;

    // Reflection vector coordinates used for env. mapping
    //   float4 vEnvMap = tex2D(tex3, (vReflect.xy + 1.0) * 0.5);
    float4 vEnvMap = tex3.Sample(SampleType, (vReflect.xy + 1.0) * 0.5);

    // Compute projected coordinates and add perturbation
    float2 vProj = (input.ScreenPos.xy / input.ScreenPos.w);
    //   float4 vRefrA = tex2D(tex2, vProj.xy + vBump.xy);
    float4 vRefrA = tex2.Sample(SampleType, vProj.xy + vBump.xy);
    //   float4 vRefrB = tex2D(tex2, vProj.xy);
    float4 vRefrB = tex2.Sample(SampleType, vProj.xy);

    // Mask occluders from refraction map
    float4 vFinal = vRefrB * vRefrA.w + vRefrA * (1 - vRefrA.w);

    // Compute Fresnel term
    float fresnel = Fresnel(LdotN, 0.4, 5.0);

    // Lerp between 1 and diffuse for glass transparency
    vDiffuse.xyz = saturate(.1 + vDiffuse.xyz * 0.9);

    // Final output is blend between reflection and
    // refraction using Fresnel term
    return vDiffuse * vFinal * (1 - fresnel) + vEnvMap * fresnel;
}