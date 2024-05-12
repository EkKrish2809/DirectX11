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

vs_out main(float4 a_position:POSITION, float3 a_normal:NORMAL)
{
    vs_out output;

    if (u_lightingEnabled == 1)
    {
        float4 eyeCoordinates = mul(u_modelMatrix, a_position);
        eyeCoordinates = mul(u_viewMatrix, eyeCoordinates);
        eyeCoordinates = mul(u_modelMatrix, eyeCoordinates);
		output.mc_position =  a_position.xyz *u_scale;
		float3x3 normalMatrix = (float3x3)(mul(u_viewMatrix, u_modelMatrix));
		float3 transformedNormals = normalize(mul(normalMatrix, a_normal));
		output.lightIntensity = dot(normalize((float3)(u_lightPosition) - (float3)(eyeCoordinates)), transformedNormals);
		output. lightIntensity *= 1.5;
    }
    output.position = mul(u_modelMatrix, a_position);
    output.position = mul(u_viewMatrix, output.position);
    output.position = mul(u_projectionMatrix, output.position);

    return output;
}