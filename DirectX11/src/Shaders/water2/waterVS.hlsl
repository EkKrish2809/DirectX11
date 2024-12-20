///////////////////////////////////////////////////////////////////////////////////////////////////
//  Proj : GPU GEMS 2 DEMOS
//  File : waterVP.cg
//  Desc : water simulation demo
///////////////////////////////////////////////////////////////////////////////////////////////////

cbuffer WaterCB : register(b0)
{
    float4x4 ModelViewProj;
    float4 vCameraPos;
    float4 vWaveParams;
};

// define inputs from application
struct VpIn
{
    float4 Pos       : POSITION;
    float2 TexCoord0 : TEXCOORD0;      
    float3 Normal    : TEXCOORD1;  
    float3 Tangent   : TEXCOORD2; 
};

// define outputs from vertex shader
struct VpOut
{
    float4 HPos       : SV_POSITION;    
    float4 Eye        : TEXCOORD0;
    float4 Wave0      : TEXCOORD1;
    float2 Wave1      : TEXCOORD2;
    float2 Wave2      : TEXCOORD3;
    float2 Wave3      : TEXCOORD4;        
    float4 ScreenPos  : TEXCOORD5;            
};

float3x3 GetTangentSpaceBasis(float3 T, float3 N)
{
   float3x3 objToTangentSpace;
   
   objToTangentSpace[0]=T;           // tangent
   objToTangentSpace[1]=cross(T, N); // binormal
   objToTangentSpace[2]=N;           // normal  
   
   return objToTangentSpace;
}

// vertex program
// VpOut main(VpIn IN, uniform float4x4 ModelViewProj, uniform float4 vCameraPos, uniform float4 vWaveParams)
VpOut main(VpIn IN)
{
    VpOut OUT;
    
    float4 vPos = IN.Pos;
    OUT.HPos = mul(ModelViewProj, vPos);
    
    float2 fTranslation = float2(fmod(vWaveParams.w, 100)*0.01, 0);
    float2 vTexCoords = vPos.xy*0.001;
    
    // Output bump layers texture coordinates    
    float fSinTranslation = sin(fTranslation*100)*0.005;
    float2 vTranslation0 = fTranslation+fSinTranslation;
    float2 vTranslation1 = fTranslation-fSinTranslation;
    float2 vTranslation2 = fTranslation;
    
    // Scale texture coordinates to get mix of low/high frequency details
    OUT.Wave0.xy = vTexCoords.xy+fTranslation*2.0;
    OUT.Wave1.xy = vTexCoords.xy*2.0+fTranslation*4.0;
    OUT.Wave2.xy = vTexCoords.xy*4.0+fTranslation*2.0;
    OUT.Wave3.xy = vTexCoords.xy*8.0+fTranslation;                
                  
    // perspective corrected projection      
    float4 vHPos = mul(ModelViewProj, vPos);         	
    OUT.Wave0.zw=vHPos.w;
    		  
	  vHPos.y = -vHPos.y;
    OUT.ScreenPos.xy = (vHPos.xy + vHPos.w)*0.5;    
    OUT.ScreenPos.zw =  float2(1, vHPos.w);      
        
    // get tangent space basis    
    float3x3 objToTangentSpace = GetTangentSpaceBasis(IN.Tangent.xyz, IN.Normal.xyz);
            
    float3 EyeVec = vCameraPos.xyz-vPos;        
    OUT.Eye.xyz = mul(objToTangentSpace, EyeVec);
                                                 
    return OUT;
}