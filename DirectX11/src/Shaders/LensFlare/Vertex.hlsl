struct VSToGS
{
    float4 Pos : SV_POSITION;
    nointerpolation uint VertexId : VERTEXID;
};

VSToGS main(uint vertexId : SV_VERTEXID)
{
    VSToGS output = (VSToGS) 0;
    output.Pos = float4(-0.8, 0.0, 1.0, 1.0);
    output.VertexId = vertexId;
    return output;
}
