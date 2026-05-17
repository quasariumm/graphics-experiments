#include "shader_resources.hlsli"
#include "meshlets.hlsli"

groupshared Payload sPayload;

[numthreads(AS_GROUP_SIZE, 1, 1)]
void main(uint tId : SV_DispatchThreadID, uint gtId : SV_GroupThreadID) 
{
    sPayload.MeshletIndices[gtId] = tId;

    DispatchMesh(AS_GROUP_SIZE, 1, 1, sPayload);
}