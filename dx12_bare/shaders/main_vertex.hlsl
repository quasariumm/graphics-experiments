struct VSIn
{
    float4 Position : POSITION;
};

struct VSOut
{
    float4 Position : SV_Position;
};

VSOut main(VSIn IN) 
{
    VSOut OUT;
    OUT.Position = IN.Position;
    return OUT;
}