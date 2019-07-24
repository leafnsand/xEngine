struct VS_INPUT
{
    float3 aPosition : POSITION;
    float2 aTexcoord0 : TEXCOORD;
    float3 aNormal : NORMAL;
};

struct VS_OUTPUT
{
    float4 Position : SV_Position;
    float2 Texcoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 PixelPosition : POSITION;
};

#define PS_INPUT VS_OUTPUT

struct PS_OUTPUT
{
    float4 Color : SV_Target;
};

float4x4 uModel;
float4x4 uView;
float4x4 uProjection;

Texture2D uTexture;
SamplerState uTexture_sampler;

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    // float3 aBinormal = cross(input.aTangent, input.aNormal);
    // float3 T = normalize(mul(uModel, float4(input.aTangent,   0.0)).xyz);
    // float3 B = normalize(mul(uModel, float4(aBinormal,        0.0)).xyz);
    // float3 N = normalize(mul(uModel, float4(input.aNormal,    0.0)).xyz);
    // float3x3 TBN = float3x3(T, B, N);
    
    output.PixelPosition = mul(uModel, float4(input.aPosition, 1.0)).xyz;
    output.Position = mul(uProjection, mul(uView, float4(output.PixelPosition, 1.0)));
    output.Normal = mul(uModel, float4(input.aNormal, 0.0)).xyz;
    output.Texcoord = input.aTexcoord0;
    
    return output;
}

PS_OUTPUT PS(PS_INPUT input)
{
    PS_OUTPUT output;

    float ambientStrength = 0.1;
    float diffuseStrength = 0.5;
    float specularStrength = 0.4;

    float3 lightPos = float3(0.0, 0.0, 10.0);
    float3 viewPos = float3(0.0, 0.0, 10.0);
    float3 lightColor = float3(1.0, 1.0, 1.0);

    float3 viewDir = normalize(viewPos - input.PixelPosition.xyz);
    float3 lightDir = normalize(lightPos - input.PixelPosition.xyz);
    float3 normal = normalize(input.Normal);

    float3 ambient = ambientStrength * lightColor;
    float3 diffuse = diffuseStrength * max(dot(normal, lightDir), 0.0) * lightColor;
    float3 specular = specularStrength * pow(max(dot(viewDir, reflect(-lightDir, normal)), 0.0), 32) * lightColor;

    output.Color = float4(ambient + diffuse + specular, 1.0) * uTexture.Sample(uTexture_sampler, input.Texcoord);

    return output;
}
