cbuffer ConstBufferV : register(b0)
{
    float3 viewPos;
    float4x4 projection;
    float4x4 view;
    float4x4 model;
    float3 lightPos;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float3 FragPos : FRAGPOS;
    float2 TexCoords : TEXCOORD;
    float3 TangentLightPos : LIGHTPOS;
    float3 TangentViewPos : VIEWPOS;
    float3 TangentFragPos : TFRAGPOS;
};

VS_OUT main(float3 aPos : POSITION, float3 aNormal : NORMAL, float2 aTexCoords : TEXCOORD, float3 aTangent : TANGENT, float3 aBitangent : BITANGENT)
{
    VS_OUT vs_out;

    float4 fragPos = mul(model, float4(aPos, 1.0));
    // fragPos = mul(view, fragPos);
    vs_out.FragPos = float3(fragPos.x, fragPos.y, fragPos.z);

    vs_out.TexCoords = aTexCoords;

    float4 Tt = mul(float4(aTangent, 1.0), model);
    // Tt = mul(view, Tt);
    float3 T = normalize(float3(Tt.x, Tt.y, Tt.z));

    float4 Tb = mul(float4(aBitangent, 1.0), model);
    // Tb = mul(view, Tb);
    float3 B = normalize(float3(Tb.x, Tb.y, Tb.z));

    float4 Tn = mul(float4(aNormal, 1.0), model);
    // Tn = mul(view, Tn);
    float3 N = normalize(float3(Tn.x, Tn.y, -Tn.z));

    float3x3 TBN = transpose(float3x3(T, B, N));
    vs_out.TangentLightPos = mul(TBN, lightPos);
    vs_out.TangentViewPos = mul(TBN, viewPos);
    vs_out.TangentFragPos = mul(TBN, vs_out.FragPos);

    vs_out.position = mul(model, float4(aPos, 1.0));
    vs_out.position = mul(view, vs_out.position);
    vs_out.position = mul(projection, vs_out.position);

    return vs_out;
}