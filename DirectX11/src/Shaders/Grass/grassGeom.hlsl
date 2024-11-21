Texture2D myTexture2D;
SamplerState mySamplerState;

cbuffer constantBuffer : register(b0)
{
    float u_time;
    float u_windStrength;
    float4x4 worldMatrix;
    float4x4 ViewMatrix;
    float4x4 ProjectionMatrix;
    float3 u_cameraPosition;
};

struct vertex
{
    float4 position:SV_POSITION;
    // float2 texcoord:TEXCOORD;
};
// static struct vertex input_array[3];


struct geometry_output
{
    float4 position:SV_POSITION;
    float2 texCoord:TEXCOORD;
    float colorVariation:COLOR;
};

float4x4 rotationX(float angle)
{
    return float4x4(1.0,    0,              0,              0,   
                    0,      cos(angle),     sin(angle),     0,    
                    0,      -sin(angle),     cos(angle),     0,   
                    0,      0,              0,              1.0);
}

float4x4 rotationY(float angle)
{
    return float4x4(cos(angle),     0,      -sin(angle),     0,      
                    0,              1.0,	0,              0,    
                    sin(angle),    0,      cos(angle),     0,     
                    0,              0,      0,              1.0);
}

float4x4 rotationZ(float angle)
{
    return float4x4(cos(angle),  	sin(angle),	    0,	 0,
                    -sin(angle),	cos(angle),  	0,   0,
                    0,    			0,  			1,   0,
                    0,    			0,          	0,   1.0);
}

float random(float2 st)
{
    return frac(sin(dot(st.xy, float2(12.9898, 78.233))) * 43758.5453123);
}

float noise(float2 st)
{
    float2 i = floor(st);
    float2 f = frac(st);

    float a = random(i);
    float b = random(i + float2(1.0, 0.0));
    float c = random(i + float2(0.0, 1.0));
    float d = random(i + float2(1.0, 1.0));

    float2 u = f * f * (3.0 - 2.0 * f);

    return lerp(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float fbm(float2 st)
{
    float v = 0.0;
    float a = 0.5;
    float2 shift = float2(100.0, 100.0);
    float2x2 rotate = float2x2(cos(0.5), sin(0.5), -sin(0.5), cos(0.5));
    for (int i = 0; i < 5; i++)
    {
        v = v + a * noise(st);
        // st = rotate * st * 2.0 + shift;
        st = mul(st, rotate) * 2.0 + shift;
        a = a * 0.5;
    }
    return v;
}

// void createQuad(triangle vertex input[3], inout TriangleStream<geometry_output> triangleStream, float3 basePosition, float4x4 crossModel, float grass_size)
void createQuad(inout TriangleStream<geometry_output> triangleStream, triangle vertex input[3], float3 basePosition, float4x4 crossModel, float grass_size)
{
    geometry_output output;
    float4 vertices[4];
    float2 texcoords[4];
    float2 windDirection = float2(1.0, 1.0);
    float4x4 modelWindMatrix = float4x4(1.0, 0.0, 0.0, 0.0,
                                        0.0, 1.0, 0.0, 0.0,
                                        0.0, 0.0, 1.0, 0.0,
                                        0.0, 0.0, 0.0, 1.0);
    float PI = 3.141592653589793;

    vertices[0] = float4(-0.25, 0.0, 0.0, 0.0);	// Bottom Left
	vertices[1] = float4(0.25, 0.0, 0.0, 0.0);	// Bottom Right
	vertices[2] = float4(-0.25, 0.5, 0.0, 0.0);	// Top Left
	vertices[3] = float4(0.25, 0.5, 0.0, 0.0);	// Top Right

	texcoords[0] = float2(0.0, 0.0);	// Bottom Left
	texcoords[1] = float2(1.0, 0.0);	// Bottom Right
	texcoords[2] = float2(0.0, 1.0);	// Top Left
	texcoords[3] = float2(1.0, 1.0);	// Top Right

    float2 uv = (basePosition.xz / 10.0) + windDirection * u_windStrength * u_time;
    uv.x = fmod(uv.x, 1.0);
    uv.y = fmod(uv.y, 1.0);

    float4 wind = myTexture2D.SampleLevel(mySamplerState, uv, 0);
    float4x4 modelWind = rotationX(wind.x * PI * 0.75f - PI * 0.25f) * rotationZ(wind.y * PI * 0.75f - PI * 0.25f);

    float4x4 modelRandomYRotation = rotationY(random(basePosition.xz) * PI);

    for (int i = 0; i < 4; i++)
    {
        if (i == 2)
            modelWindMatrix = modelWind;

        output.position = mul(mul(mul((input[0].position + mul(mul(mul(((grass_size * vertices[i])), crossModel), modelRandomYRotation), modelWindMatrix)), worldMatrix), ViewMatrix), ProjectionMatrix);
        output.texCoord = texcoords[i];
        output.colorVariation = fbm(input[0].position.xz);
        triangleStream.Append(output);
    }
    triangleStream.RestartStrip();

}

void createGrass(inout TriangleStream<geometry_output> triangleStream, triangle vertex input[3], int numberOfQuads, float grass_size)
{
    float4x4 model0 = float4x4(1.0, 0.0, 0.0, 0.0,
                                0.0, 1.0, 0.0, 0.0,
                                0.0, 0.0, 1.0, 0.0,
                                0.0, 0.0, 0.0, 1.0);
    float4x4 model45 = rotationY(radians(45));
    float4x4 modelm45 = rotationY(-radians(45));

    if (numberOfQuads == 1)
    {
        createQuad(triangleStream, input, input[0].position.xyz, model0, grass_size);
    }
    else if (numberOfQuads == 2)
    {
        createQuad(triangleStream, input, input[0].position.xyz, model45, grass_size);
        createQuad(triangleStream, input, input[0].position.xyz, modelm45, grass_size);
    }
    else if (numberOfQuads == 3)
    {
        createQuad(triangleStream, input, input[0].position.xyz, model0, grass_size);
        createQuad(triangleStream, input, input[0].position.xyz, model45, grass_size);
        createQuad(triangleStream, input, input[0].position.xyz, modelm45, grass_size);
    }

}


[maxvertexcount(36)]

void main(triangle vertex input[3], inout TriangleStream<geometry_output> triangleStream)
{
    // geometry_output output;
    float c_min_size = 0.4;
    float LOD1 = 5.0;
    float LOD2 = 10.0;
    float LOD3 = 20.0;
    float3 cameraDistance = input[0].position.xyz - u_cameraPosition;
    float distanceLength = length(cameraDistance);
    float transitionFactor = 6.0;
    uint details = 3;
    
    // input_array = input;
    // triangleStream_out = triangleStream;
    float grassSize = random(input[0].position.xz) * (1.0 - c_min_size) + c_min_size;
    if (distanceLength == LOD2)
    {
        transitionFactor = transitionFactor * 1.5;
    }
    distanceLength = distanceLength + (random(input[0].position.xz) * transitionFactor - transitionFactor / 2.0f);

    if (distanceLength > LOD1)
    {
        details = 2;
    }
    if (distanceLength > LOD2)
    {
        details = 1;
    }
    if (distanceLength > LOD3)
    {
        details = 0;
    }

    if (details != 1 || (details == 1 && (uint(input[0].position.x * 10) % 1) == 0 || (uint(input[0].position.z * 10) % 1) == 0)
     || (details == 2 && (input[0].position.x * 5) % 1) == 0 || (uint(input[0].position.z * 5) % 1) == 0)
    {
        createGrass(triangleStream, input, details, grassSize);
    }
}
