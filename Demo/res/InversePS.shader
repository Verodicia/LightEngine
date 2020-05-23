+GLSL // GLSL PIXEL SHADER //
#version 450 core

out vec4 FSOutFragmentColor;

in vec2 VSOutTexCoords;

uniform sampler2D u_Texture; // #BINDING_0

void main()
{
    FSOutFragmentColor = vec4(1.0f - texture(u_Texture, VSOutTexCoords).rgb, 1.0);
}
-GLSL


+HLSL // HLSL PIXEL SHADER //
Texture2D frameTexture : register(t0);
SamplerState linearSampler : register(s0)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 main(float2 TexCoords : TEXCOORDS) : SV_TARGET
{
	return float4(1.0f - frameTexture.Sample(linearSampler, float2(TexCoords.x, 1.0f - TexCoords.y)).rgb, 1.0f);
}
-HLSL