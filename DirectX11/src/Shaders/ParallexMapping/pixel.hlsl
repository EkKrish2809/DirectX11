cbuffer ConstBufferP : register(b1)
{
    int keyPresed;
    float heightScale;
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

Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
Texture2D depthMap : register(t2);

SamplerState textureSampler;

float2 ParallaxMapping(float2 texCoords, float3 viewDir)
{ 
    const float minLayers = 8;
    const float maxLayers = 52;
    float numLayers = lerp(maxLayers, minLayers, abs(dot(float3(0.0, 0.0, 1.0), viewDir)));

    float layerDepth = 1.0 / numLayers;

    float currentLayerDepth = 0.0;

    float2 P = viewDir.xy / viewDir.z * 0.2;//heightScale; 
    float2 deltaTexCoords = P / numLayers;

    float2  currentTexCoords     = texCoords;
    float currentDepthMapValue = depthMap.Sample(textureSampler, currentTexCoords).r;

    [unroll(36)]
    while(currentLayerDepth <= currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = depthMap.Sample(textureSampler, currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }

    float2 prevTexCoords = currentTexCoords + deltaTexCoords;

    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = depthMap.Sample(textureSampler, prevTexCoords).r - currentLayerDepth + layerDepth;

    float weight = afterDepth / (afterDepth - beforeDepth);
    float2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

float4 main(VS_OUT fs_in) : SV_TARGET
{
    float4 FragColor;

    float3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    float2 texCoords = fs_in.TexCoords;
    texCoords = ParallaxMapping(fs_in.TexCoords,  viewDir);
    // if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
    // {
        // discard;
    // }

    float3 normal = normalMap.Sample(textureSampler, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    // normal = float3(normal.x, -normal.y, normal.z); // inverting Y / Green channel

    float3 color = diffuseMap.Sample(textureSampler, texCoords).rgb;

    float3 ambient = 0.1 * color;

    float3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    float3 diffuse = diff * color;

    float3 reflectDir = reflect(-lightDir, normal);
    float3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    float3 specular = float3(0.2, 0.2, 0.2) * spec;
    FragColor = float4(ambient + diffuse + specular, 1.0);

    return FragColor;
}
