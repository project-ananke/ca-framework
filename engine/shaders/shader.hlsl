struct VSInput
{
    float3 position_local : POSITION;
    float4 color : COLOR;
};

struct VSOutput
{
    float4 position_clip : SV_POSITION;
    float4 color : COLOR;
};

VSOutput vs_main(VSInput input)
{
    VSOutput output = (VSOutput)0;

    output.position_clip = float4(input.position_local, 1.0);
    output.color = float4(input.color);

    return output;
}

float4 ps_main(VSOutput input) : SV_TARGET
{
    return input.color;
}