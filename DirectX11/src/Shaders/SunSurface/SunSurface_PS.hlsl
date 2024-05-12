Texture2D myTexture2D;
SamplerState mySamplerState;
cbuffer ConstantBuffer
{
    float4x4 u_modelMatrix;
	float4x4 u_viewMatrix;
	float4x4 u_projectionMatrix;
	float u_scale;
	float4 u_lightPosition;
	int u_lightingEnabled;
}


struct vs_out
{
    float4 position : SV_POSITION;
	float3 mc_position : MCPOSITION;
    float lightIntensity : LIGHT;
};


float4 main(vs_out _in) : SV_TARGET
{
    float4 skyColor = float4(0.8, 0.7, 0.0, 0.0);
    float4 sunColor = float4(0.6, 0.1, 0.0, 0.0);
    float4 color;

    if (u_lightingEnabled == 1)
    {
        float4 noisevec = myTexture2D.Sample(mySamplerState, _in.mc_position);
        float intensity = abs(noisevec[0]-0.25) + abs(noisevec[1]-0.125) + abs(noisevec[2]-0.0625) + abs(noisevec[3]-0.03125);
        intensity = clamp(intensity * 0.20, 0.0, 1.0);
        color = lerp(skyColor, sunColor, intensity) * _in.lightIntensity;
    }
    else
    {
        color = myTexture2D.Sample(mySamplerState, _in.mc_position);
    }

    return color;
    // return float4(1.0f, 0.0f, 0.0f, 0.0f);
}