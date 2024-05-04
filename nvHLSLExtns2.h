R"( /************************************************************************************************************************************\
|*                                                                                                                                    *|
|*     Copyright © 2012 NVIDIA Corporation.  All rights reserved.                                                                     *|
|*                                                                                                                                    *|
|*  NOTICE TO USER:                                                                                                                   *|
|*                                                                                                                                    *|
|*  This software is subject to NVIDIA ownership rights under U.S. and international Copyright laws.                                  *|
|*                                                                                                                                    *|
|*  This software and the information contained herein are PROPRIETARY and CONFIDENTIAL to NVIDIA                                     *|
|*  and are being provided solely under the terms and conditions of an NVIDIA software license agreement.                             *|
|*  Otherwise, you have no rights to use or access this software in any manner.                                                       *|
|*                                                                                                                                    *|
|*  If not covered by the applicable NVIDIA software license agreement:                                                               *|
|*  NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOFTWARE FOR ANY PURPOSE.                                            *|
|*  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.                                                           *|
|*  NVIDIA DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,                                                                     *|
|*  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.                       *|
|*  IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,                               *|
|*  OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT,                         *|
|*  NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.            *|
|*                                                                                                                                    *|
|*  U.S. Government End Users.                                                                                                        *|
|*  This software is a "commercial item" as that term is defined at 48 C.F.R. 2.101 (OCT 1995),                                       *|
|*  consisting  of "commercial computer  software"  and "commercial computer software documentation"                                  *|
|*  as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995) and is provided to the U.S. Government only as a commercial end item.     *|
|*  Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through 227.7202-4 (JUNE 1995),                                          *|
|*  all U.S. Government End Users acquire the software with only those rights set forth herein.                                       *|
|*                                                                                                                                    *|
|*  Any use of this software in individual and commercial software must include,                                                      *|
|*  in the user documentation and internal comments to the code,                                                                      *|
|*  the above Disclaimer (as applicable) and U.S. Government End Users Notice.                                                        *|
|*                                                                                                                                    *|
 \************************************************************************************************************************************/
//----------------------------------------------------------------------------//
//------------------------------ WaveMultiPrefix functions -------------------//
//----------------------------------------------------------------------------//

// Following are the WaveMultiPrefix functions for different operations (Add, Bitand, BitOr, BitXOr) for different datatypes (uint, uint2, uint4) 
// This is a set of functions which implement multi-prefix operations among the set of active lanes in the current wave (WARP). 
// A multi-prefix operation comprises a set of prefix operations, executed in parallel within subsets of lanes identified with the provided bitmasks. 
// These bitmasks represent partitioning of the set of active lanes in the current wave into N groups (where N is the number of unique masks across all lanes in the wave). 
// N prefix operations are then performed each within its corresponding group. 
// The groups are assumed to be non-intersecting (that is, a given lane can be a member of one and only one group), 
// and bitmasks in all lanes belonging to the same group are required to be the same.
// There are 2 type of functions - Exclusive and Inclusive prefix operations.
// e.g. For NvWaveMultiPrefixInclusiveAdd(val, mask) operation - For each of the groups (for which mask input is same) following is the expected output : 
// i^th thread in a group has value = sum(values of threads 0 to i)
// For Exclusive version of same opeartion - 
// i^th thread in a group has value = sum(values of threads 0 to i-1)  and 0th thread in a the Group has value 0 

// Extensions for Add 
uint NvWaveMultiPrefixInclusiveAdd(uint val, uint mask)
{
    uint temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint nextLane = firstbithigh(remainingThreads);
    for (uint i = 0; i < NV_WARP_SIZE_LOG2; i++)
    {
        temp = NvShfl(val, nextLane);
        uint laneValid;
        // As remainingThreads only has threads in group with smaller thread ids than its own thread-id nextLane can never be 31 for any thread in the group except the smallest one
        // For smallest thread in the group, remainingThreads is 0 -->  nextLane is ~0 (i.e. considering last 5 bits its 31)
        // So passing maskClampValue=30 to __NvShflGeneric, it will return laneValid=false for the smallest thread in the group. So update val and nextLane based on laneValid.
        uint newLane = asuint(__NvShflGeneric(nextLane, nextLane, 30, laneValid));
        if (laneValid) // if nextLane's nextLane is valid
        {
            val = val + temp;
            nextLane = newLane;
        }
    }
    return val;
}

uint NvWaveMultiPrefixExclusiveAdd(uint val, uint mask)
{
    uint temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint lane = firstbithigh(remainingThreads);
    temp = NvShfl(val, lane);
    val = remainingThreads != 0 ? temp : 0;
    return NvWaveMultiPrefixInclusiveAdd(val, mask);
}

uint2 NvWaveMultiPrefixInclusiveAdd(uint2 val, uint mask)
{
    uint2 temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint nextLane = firstbithigh(remainingThreads);
    for (uint i = 0; i < NV_WARP_SIZE_LOG2; i++)
    {
        temp = NvShfl(val, nextLane);
        uint laneValid;
        uint newLane = asuint(__NvShflGeneric(nextLane, nextLane, 30, laneValid));
        if (laneValid) // if nextLane's nextLane is valid
        {
            val = val + temp;
            nextLane = newLane;
        }
    }
    return val;
}

uint2 NvWaveMultiPrefixExclusiveAdd(uint2 val, uint mask)
{
    uint2 temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint lane = firstbithigh(remainingThreads);
    temp = NvShfl(val, lane);
    val = remainingThreads != 0 ? temp : uint2(0, 0);
    return NvWaveMultiPrefixInclusiveAdd(val, mask);
}

uint4 NvWaveMultiPrefixInclusiveAdd(uint4 val, uint mask)
{
    uint4 temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint nextLane = firstbithigh(remainingThreads);
    for (uint i = 0; i < NV_WARP_SIZE_LOG2; i++)
    {
        temp = NvShfl(val, nextLane);
        uint laneValid;
        uint newLane = asuint(__NvShflGeneric(nextLane, nextLane, 30, laneValid));
        if (laneValid) // if nextLane's nextLane is valid
        {
            val = val + temp;
            nextLane = newLane;
        }
    }
    return val;
}

uint4 NvWaveMultiPrefixExclusiveAdd(uint4 val, uint mask)
{
    uint4 temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint lane = firstbithigh(remainingThreads);
    temp = NvShfl(val, lane);
    val = remainingThreads != 0 ? temp : uint4(0, 0, 0, 0);
    return NvWaveMultiPrefixInclusiveAdd(val, mask);
}

// MultiPrefix extensions for Bitand
uint NvWaveMultiPrefixInclusiveAnd(uint val, uint mask)
{
    uint temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint nextLane = firstbithigh(remainingThreads);
    for (uint i = 0; i < NV_WARP_SIZE_LOG2; i++)
    {
        temp = NvShfl(val, nextLane);
        uint laneValid;
        uint newLane = asuint(__NvShflGeneric(nextLane, nextLane, 30, laneValid));
        if (laneValid) // if nextLane's nextLane is valid
        {
            val = val & temp;
            nextLane = newLane;
        }
    }
    return val;
}

uint NvWaveMultiPrefixExclusiveAnd(uint val, uint mask)
{
    uint temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint lane = firstbithigh(remainingThreads);
    temp = NvShfl(val, lane);
    val = remainingThreads != 0 ? temp : ~0;
    return NvWaveMultiPrefixInclusiveAnd(val, mask);
}

uint2 NvWaveMultiPrefixInclusiveAnd(uint2 val, uint mask)
{
    uint2 temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint nextLane = firstbithigh(remainingThreads);
    for (uint i = 0; i < NV_WARP_SIZE_LOG2; i++)
    {
        temp = NvShfl(val, nextLane);
        uint laneValid;
        uint newLane = asuint(__NvShflGeneric(nextLane, nextLane, 30, laneValid));
        if (laneValid) // if nextLane's nextLane is valid
        {
            val = val & temp;
            nextLane = newLane;
        }
    }
    return val;
}

uint2 NvWaveMultiPrefixExclusiveAnd(uint2 val, uint mask)
{
    uint2 temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint lane = firstbithigh(remainingThreads);
    temp = NvShfl(val, lane);
    val = remainingThreads != 0 ? temp : uint2(~0, ~0);
    return NvWaveMultiPrefixInclusiveAnd(val, mask);
}


uint4 NvWaveMultiPrefixInclusiveAnd(uint4 val, uint mask)
{
    uint4 temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint nextLane = firstbithigh(remainingThreads);
    for (uint i = 0; i < NV_WARP_SIZE_LOG2; i++)
    {
        temp = NvShfl(val, nextLane);
        uint laneValid;
        uint newLane = asuint(__NvShflGeneric(nextLane, nextLane, 30, laneValid));
        if (laneValid) // if nextLane's nextLane is valid
        {
            val = val & temp;
            nextLane = newLane;
        }
    }
    return val;
}

uint4 NvWaveMultiPrefixExclusiveAnd(uint4 val, uint mask)
{
    uint4 temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint lane = firstbithigh(remainingThreads);
    temp = NvShfl(val, lane);
    val = remainingThreads != 0 ? temp : uint4(~0, ~0, ~0, ~0);
    return NvWaveMultiPrefixInclusiveAnd(val, mask);
}


// MultiPrefix extensions for BitOr
uint NvWaveMultiPrefixInclusiveOr(uint val, uint mask)
{
    uint temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint nextLane = firstbithigh(remainingThreads);
    for (uint i = 0; i < NV_WARP_SIZE_LOG2; i++)
    {
        temp = NvShfl(val, nextLane);
        uint laneValid;
        uint newLane = asuint(__NvShflGeneric(nextLane, nextLane, 30, laneValid));
        if (laneValid) // if nextLane's nextLane is valid
        {
            val = val | temp;
            nextLane = newLane;
        }
    }
    return val;
}

uint NvWaveMultiPrefixExclusiveOr(uint val, uint mask)
{
    uint temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint lane = firstbithigh(remainingThreads);
    temp = NvShfl(val, lane);
    val = remainingThreads != 0 ? temp : 0;
    return NvWaveMultiPrefixInclusiveOr(val, mask);
}

uint2 NvWaveMultiPrefixInclusiveOr(uint2 val, uint mask)
{
    uint2 temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint nextLane = firstbithigh(remainingThreads);
    for (uint i = 0; i < NV_WARP_SIZE_LOG2; i++)
    {
        temp = NvShfl(val, nextLane);
        uint laneValid;
        uint newLane = asuint(__NvShflGeneric(nextLane, nextLane, 30, laneValid));
        if (laneValid) // if nextLane's nextLane is valid
        {
            val = val | temp;
            nextLane = newLane;
        }
    }
    return val;
}

uint2 NvWaveMultiPrefixExclusiveOr(uint2 val, uint mask)
{
    uint2 temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint lane = firstbithigh(remainingThreads);
    temp = NvShfl(val, lane);
    val = remainingThreads != 0 ? temp : uint2(0, 0);
    return NvWaveMultiPrefixInclusiveOr(val, mask);
}


uint4 NvWaveMultiPrefixInclusiveOr(uint4 val, uint mask)
{
    uint4 temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint nextLane = firstbithigh(remainingThreads);
    for (uint i = 0; i < NV_WARP_SIZE_LOG2; i++)
    {
        temp = NvShfl(val, nextLane);
        uint laneValid;
        uint newLane = asuint(__NvShflGeneric(nextLane, nextLane, 30, laneValid));
        if (laneValid) // if nextLane's nextLane is valid
        {
            val = val | temp;
            nextLane = newLane;
        }
    }
    return val;
}

uint4 NvWaveMultiPrefixExclusiveOr(uint4 val, uint mask)
{
    uint4 temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint lane = firstbithigh(remainingThreads);
    temp = NvShfl(val, lane);
    val = remainingThreads != 0 ? temp : uint4(0, 0, 0, 0);
    return NvWaveMultiPrefixInclusiveOr(val, mask);
}


// MultiPrefix extensions for BitXOr
uint NvWaveMultiPrefixInclusiveXOr(uint val, uint mask)
{
    uint temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint nextLane = firstbithigh(remainingThreads);
    for (uint i = 0; i < NV_WARP_SIZE_LOG2; i++)
    {
        temp = NvShfl(val, nextLane);
        uint laneValid;
        uint newLane = asuint(__NvShflGeneric(nextLane, nextLane, 30, laneValid));
        if (laneValid) // if nextLane's nextLane is valid
        {
            val = val ^ temp;
            nextLane = newLane;
        }
    }
    return val;
}
)" R"(

uint NvWaveMultiPrefixExclusiveXOr(uint val, uint mask)
{
    uint temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint lane = firstbithigh(remainingThreads);
    temp = NvShfl(val, lane);
    val = remainingThreads != 0 ? temp : 0;
    return NvWaveMultiPrefixInclusiveXOr(val, mask);
}

uint2 NvWaveMultiPrefixInclusiveXOr(uint2 val, uint mask)
{
    uint2 temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint nextLane = firstbithigh(remainingThreads);
    for (uint i = 0; i < NV_WARP_SIZE_LOG2; i++)
    {
        temp = NvShfl(val, nextLane);
        uint laneValid;
        uint newLane = asuint(__NvShflGeneric(nextLane, nextLane, 30, laneValid));
        if (laneValid) // if nextLane's nextLane is valid
        {
            val = val ^ temp;
            nextLane = newLane;
        }
    }
    return val;
}

uint2 NvWaveMultiPrefixExclusiveXOr(uint2 val, uint mask)
{
    uint2 temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint lane = firstbithigh(remainingThreads);
    temp = NvShfl(val, lane);
    val = remainingThreads != 0 ? temp : uint2(0, 0);
    return NvWaveMultiPrefixInclusiveXOr(val, mask);
}


uint4 NvWaveMultiPrefixInclusiveXOr(uint4 val, uint mask)
{
    uint4 temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint nextLane = firstbithigh(remainingThreads);
    for (uint i = 0; i < NV_WARP_SIZE_LOG2; i++)
    {
        temp = NvShfl(val, nextLane);
        uint laneValid;
        uint newLane = asuint(__NvShflGeneric(nextLane, nextLane, 30, laneValid));
        if (laneValid) // if nextLane's nextLane is valid
        {
            val = val ^ temp;
            nextLane = newLane;
        }
    }
    return val;
}

uint4 NvWaveMultiPrefixExclusiveXOr(uint4 val, uint mask)
{
    uint4 temp;
    uint a = NvActiveThreads();
    uint remainingThreads = a & __NvGetSpecial(NV_SPECIALOP_THREADLTMASK) & mask;
    uint lane = firstbithigh(remainingThreads);
    temp = NvShfl(val, lane);
    val = remainingThreads != 0 ? temp : uint4(0, 0, 0, 0);
    return NvWaveMultiPrefixInclusiveXOr(val, mask);
}


//----------------------------------------------------------------------------//
//------------------------- DXR Micro-map Extension --------------------------//
//----------------------------------------------------------------------------//

float3x3 NvRtTriangleObjectPositions()
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].opcode = NV_EXTN_OP_RT_TRIANGLE_OBJECT_POSITIONS;

    float3x3 ret;
    ret[0][0] = asfloat(g_NvidiaExt.IncrementCounter());
    ret[0][1] = asfloat(g_NvidiaExt.IncrementCounter());
    ret[0][2] = asfloat(g_NvidiaExt.IncrementCounter());
    ret[1][0] = asfloat(g_NvidiaExt.IncrementCounter());
    ret[1][1] = asfloat(g_NvidiaExt.IncrementCounter());
    ret[1][2] = asfloat(g_NvidiaExt.IncrementCounter());
    ret[2][0] = asfloat(g_NvidiaExt.IncrementCounter());
    ret[2][1] = asfloat(g_NvidiaExt.IncrementCounter());
    ret[2][2] = asfloat(g_NvidiaExt.IncrementCounter());
    return ret;
}

float3x3 NvRtMicroTriangleObjectPositions()
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].opcode = NV_EXTN_OP_RT_MICRO_TRIANGLE_OBJECT_POSITIONS;

    float3x3 ret;
    ret[0][0] = asfloat(g_NvidiaExt.IncrementCounter());
    ret[0][1] = asfloat(g_NvidiaExt.IncrementCounter());
    ret[0][2] = asfloat(g_NvidiaExt.IncrementCounter());
    ret[1][0] = asfloat(g_NvidiaExt.IncrementCounter());
    ret[1][1] = asfloat(g_NvidiaExt.IncrementCounter());
    ret[1][2] = asfloat(g_NvidiaExt.IncrementCounter());
    ret[2][0] = asfloat(g_NvidiaExt.IncrementCounter());
    ret[2][1] = asfloat(g_NvidiaExt.IncrementCounter());
    ret[2][2] = asfloat(g_NvidiaExt.IncrementCounter());
    return ret;
}

float3x2 NvRtMicroTriangleBarycentrics()
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].opcode = NV_EXTN_OP_RT_MICRO_TRIANGLE_BARYCENTRICS;

    float3x2 ret;
    ret[0][0] = asfloat(g_NvidiaExt.IncrementCounter());
    ret[0][1] = asfloat(g_NvidiaExt.IncrementCounter());
    ret[1][0] = asfloat(g_NvidiaExt.IncrementCounter());
    ret[1][1] = asfloat(g_NvidiaExt.IncrementCounter());
    ret[2][0] = asfloat(g_NvidiaExt.IncrementCounter());
    ret[2][1] = asfloat(g_NvidiaExt.IncrementCounter());
    return ret;
}

bool NvRtIsMicroTriangleHit()
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].opcode = NV_EXTN_OP_RT_IS_MICRO_TRIANGLE_HIT;
    uint ret = g_NvidiaExt.IncrementCounter();
    return ret != 0;
}

bool NvRtIsBackFacing()
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].opcode = NV_EXTN_OP_RT_IS_BACK_FACING;
    uint ret = g_NvidiaExt.IncrementCounter();
    return ret != 0;
}

#if __SHADER_TARGET_MAJOR > 6 || (__SHADER_TARGET_MAJOR == 6 && __SHADER_TARGET_MINOR >= 5)

float3 NvRtMicroVertexObjectPosition(RaytracingAccelerationStructure AccelerationStructure, uint InstanceIndex, uint GeometryIndex, uint PrimitiveIndex, uint2 UV)
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].opcode = NV_EXTN_OP_RT_MICRO_VERTEX_OBJECT_POSITION;
    g_NvidiaExt[index].src0u.x = InstanceIndex;
    g_NvidiaExt[index].src0u.y = GeometryIndex;
    g_NvidiaExt[index].src0u.z = PrimitiveIndex;
    g_NvidiaExt[index].src0u.w = UV.x;
    g_NvidiaExt[index].src1u.x = UV.y;
    uint handle = g_NvidiaExt.IncrementCounter();
    float3 ret;
    ret.x = asfloat(g_NvidiaExt.IncrementCounter());
    ret.y = asfloat(g_NvidiaExt.IncrementCounter());
    ret.z = asfloat(g_NvidiaExt.IncrementCounter());

    RayQuery<0> rq;
    rq.TraceRayInline(AccelerationStructure, 0, handle, (RayDesc)0);

    return ret;
}

float2 NvRtMicroVertexBarycentrics(RaytracingAccelerationStructure AccelerationStructure, uint InstanceIndex, uint GeometryIndex, uint PrimitiveIndex, uint2 UV)
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].opcode = NV_EXTN_OP_RT_MICRO_VERTEX_BARYCENTRICS;
    g_NvidiaExt[index].src0u.x = InstanceIndex;
    g_NvidiaExt[index].src0u.y = GeometryIndex;
    g_NvidiaExt[index].src0u.z = PrimitiveIndex;
    g_NvidiaExt[index].src0u.w = UV.x;
    g_NvidiaExt[index].src1u.x = UV.y;
    uint handle = g_NvidiaExt.IncrementCounter();
    float2 ret;
    ret.x = asfloat(g_NvidiaExt.IncrementCounter());
    ret.y = asfloat(g_NvidiaExt.IncrementCounter());

    RayQuery<0> rq;
    rq.TraceRayInline(AccelerationStructure, 0, handle, (RayDesc)0);

    return ret;
}

#endif

//----------------------------------------------------------------------------//
//------------------------- DXR HitObject Extension --------------------------//
//----------------------------------------------------------------------------//

// Support for templates in HLSL requires HLSL 2021+. When using dxc,
// use the -HV 2021 command line argument to enable these versions.
#if defined(__HLSL_VERSION) && (__HLSL_VERSION >= 2021) && !defined(NV_HITOBJECT_USE_MACRO_API)

struct NvHitObject {
    uint _handle;

    bool IsMiss()
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_IS_MISS;
        g_NvidiaExt[index].src0u.x = _handle;
        uint ret = g_NvidiaExt.IncrementCounter();
        return ret != 0;
    }

    bool IsHit()
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_IS_HIT;
        g_NvidiaExt[index].src0u.x = _handle;
        uint ret = g_NvidiaExt.IncrementCounter();
        return ret != 0;
    }

    bool IsNop()
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_IS_NOP;
        g_NvidiaExt[index].src0u.x = _handle;
        uint ret = g_NvidiaExt.IncrementCounter();
        return ret != 0;
    }

    uint GetInstanceID()
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_GET_INSTANCE_ID;
        g_NvidiaExt[index].src0u.x = _handle;
        return g_NvidiaExt.IncrementCounter();
    }

    uint GetInstanceIndex()
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_GET_INSTANCE_INDEX;
        g_NvidiaExt[index].src0u.x = _handle;
        return g_NvidiaExt.IncrementCounter();
    }

    uint GetPrimitiveIndex()
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_GET_PRIMITIVE_INDEX;
        g_NvidiaExt[index].src0u.x = _handle;
        return g_NvidiaExt.IncrementCounter();
    }

    uint GetGeometryIndex()
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_GET_GEOMETRY_INDEX;
        g_NvidiaExt[index].src0u.x = _handle;
        return g_NvidiaExt.IncrementCounter();
    }

    uint GetHitKind()
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_GET_HIT_KIND;
        g_NvidiaExt[index].src0u.x = _handle;
        return g_NvidiaExt.IncrementCounter();
    }

    RayDesc GetRayDesc()
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_GET_RAY_DESC;
        g_NvidiaExt[index].src0u.x = _handle;

        uint tmin = g_NvidiaExt.IncrementCounter();
        uint tmax = g_NvidiaExt.IncrementCounter();
        uint rayOrgX = g_NvidiaExt.IncrementCounter();
        uint rayOrgY = g_NvidiaExt.IncrementCounter();
        uint rayOrgZ = g_NvidiaExt.IncrementCounter();
        uint rayDirX = g_NvidiaExt.IncrementCounter();
        uint rayDirY = g_NvidiaExt.IncrementCounter();
        uint rayDirZ = g_NvidiaExt.IncrementCounter();

        RayDesc ray;
        ray.TMin = asfloat(tmin);
        ray.TMax = asfloat(tmax);
        ray.Origin.x = asfloat(rayOrgX);
        ray.Origin.y = asfloat(rayOrgY);
        ray.Origin.z = asfloat(rayOrgZ);
        ray.Direction.x = asfloat(rayDirX);
        ray.Direction.y = asfloat(rayDirY);
        ray.Direction.z = asfloat(rayDirZ);

        return ray;
    }

    template <typename T>
    T GetAttributes()
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_GET_ATTRIBUTES;
        g_NvidiaExt[index].src0u.x = _handle;
        uint callHandle = g_NvidiaExt.IncrementCounter();

        T attrs;
        CallShader(callHandle, attrs);
        return attrs;
    }

    uint GetShaderTableIndex()
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_GET_SHADER_TABLE_INDEX;
        g_NvidiaExt[index].src0u.x = _handle;
        return g_NvidiaExt.IncrementCounter();
    }

    uint LoadLocalRootTableConstant(uint RootConstantOffsetInBytes)
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_LOAD_LOCAL_ROOT_TABLE_CONSTANT;
        g_NvidiaExt[index].src0u.x = _handle;
        g_NvidiaExt[index].src0u.y = RootConstantOffsetInBytes;
        return g_NvidiaExt.IncrementCounter();
    }
};

template<typename T>
NvHitObject NvTraceRayHitObject(
    RaytracingAccelerationStructure AccelerationStructure,
    uint RayFlags,
    uint InstanceInclusionMask,
    uint RayContributionToHitGroupIndex,
    uint MultiplierForGeometryContributionToHitGroupIndex,
    uint MissShaderIndex,
    RayDesc Ray,
    inout T Payload)
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_TRACE_RAY;
    g_NvidiaExt[index].numOutputsForIncCounter = 2;
    g_NvidiaExt[index].src0u.x = MissShaderIndex;
    uint hitHandle = g_NvidiaExt.IncrementCounter();
    uint traceHandle = g_NvidiaExt.IncrementCounter();

    TraceRay(AccelerationStructure, RayFlags, InstanceInclusionMask, RayContributionToHitGroupIndex, MultiplierForGeometryContributionToHitGroupIndex, traceHandle, Ray, Payload);

    NvHitObject hitObj;
    hitObj._handle = hitHandle;
    return hitObj;
}

template <typename T>
NvHitObject NvMakeHit(
    RaytracingAccelerationStructure AccelerationStructure,
    uint InstanceIndex,
    uint GeometryIndex,
    uint PrimitiveIndex,
    uint HitKind,
    uint RayContributionToHitGroupIndex,
    uint MultiplierForGeometryContributionToHitGroupIndex,
    RayDesc Ray,
    T Attributes)
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_MAKE_HIT;
    g_NvidiaExt[index].numOutputsForIncCounter = 2;
    g_NvidiaExt[index].src0u.x = InstanceIndex;
    g_NvidiaExt[index].src0u.y = GeometryIndex;
    g_NvidiaExt[index].src0u.z = PrimitiveIndex;
    g_NvidiaExt[index].src0u.w = HitKind;
    g_NvidiaExt[index].src1u.x = RayContributionToHitGroupIndex;
    g_NvidiaExt[index].src1u.y = MultiplierForGeometryContributionToHitGroupIndex;
    uint hitHandle = g_NvidiaExt.IncrementCounter();
    uint traceHandle = g_NvidiaExt.IncrementCounter();

    struct AttrWrapper { T Attrs; };
    AttrWrapper wrapper;
    wrapper.Attrs = Attributes;
    CallShader(traceHandle, wrapper);

    struct DummyPayload { int a; };
    DummyPayload payload;
    TraceRay(AccelerationStructure, 0, 0, 0, 0, traceHandle, Ray, payload);

    NvHitObject hitObj;
    hitObj._handle = hitHandle;
    return hitObj;
}

template <typename T>
NvHitObject NvMakeHitWithRecordIndex(
    uint HitGroupRecordIndex,
    RaytracingAccelerationStructure AccelerationStructure,
    uint InstanceIndex,
    uint GeometryIndex,
    uint PrimitiveIndex,
    uint HitKind,
    RayDesc Ray,
    T Attributes)
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_MAKE_HIT_WITH_RECORD_INDEX;
    g_NvidiaExt[index].numOutputsForIncCounter = 2;
    g_NvidiaExt[index].src0u.x = InstanceIndex;
    g_NvidiaExt[index].src0u.y = GeometryIndex;
    g_NvidiaExt[index].src0u.z = PrimitiveIndex;
    g_NvidiaExt[index].src0u.w = HitKind;
    g_NvidiaExt[index].src1u.x = HitGroupRecordIndex;
    uint hitHandle = g_NvidiaExt.IncrementCounter();
    uint traceHandle = g_NvidiaExt.IncrementCounter();

    struct AttrWrapper { T Attrs; };
    AttrWrapper wrapper;
    wrapper.Attrs = Attributes;
    CallShader(traceHandle, wrapper);

    struct DummyPayload { int a; };
    DummyPayload payload;
    TraceRay(AccelerationStructure, 0, 0, 0, 0, traceHandle, Ray, payload);

    NvHitObject hitObj;
    hitObj._handle = hitHandle;
    return hitObj;
}

NvHitObject NvMakeMiss(
    uint MissShaderIndex,
    RayDesc Ray)
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_MAKE_MISS;
    g_NvidiaExt[index].src0u.x = MissShaderIndex;
    g_NvidiaExt[index].src0u.y = asuint(Ray.TMin);
    g_NvidiaExt[index].src0u.z = asuint(Ray.TMax);
    g_NvidiaExt[index].src1u.x = asuint(Ray.Origin.x);
    g_NvidiaExt[index].src1u.y = asuint(Ray.Origin.y);
    g_NvidiaExt[index].src1u.z = asuint(Ray.Origin.z);
    g_NvidiaExt[index].src2u.x = asuint(Ray.Direction.x);
    g_NvidiaExt[index].src2u.y = asuint(Ray.Direction.y);
    g_NvidiaExt[index].src2u.z = asuint(Ray.Direction.z);
    uint hitHandle = g_NvidiaExt.IncrementCounter();

    NvHitObject hitObj;
    hitObj._handle = hitHandle;
    return hitObj;
}

NvHitObject NvMakeNop()
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_MAKE_NOP;
    uint hitHandle = g_NvidiaExt.IncrementCounter();

    NvHitObject hitObj;
    hitObj._handle = hitHandle;
    return hitObj;
}

void NvReorderThread(uint CoherenceHint, uint NumCoherenceHintBits)
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_REORDER_THREAD;
    g_NvidiaExt[index].src0u.x = 0;
    g_NvidiaExt[index].src0u.y = 0;
    g_NvidiaExt[index].src0u.z = CoherenceHint;
    g_NvidiaExt[index].src0u.w = NumCoherenceHintBits;
    g_NvidiaExt.IncrementCounter();
}
)" R"(

void NvReorderThread(NvHitObject HitObj, uint CoherenceHint, uint NumCoherenceHintBits)
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_REORDER_THREAD;
    g_NvidiaExt[index].src0u.x = 1;
    g_NvidiaExt[index].src0u.y = HitObj._handle;
    g_NvidiaExt[index].src0u.z = CoherenceHint;
    g_NvidiaExt[index].src0u.w = NumCoherenceHintBits;
    g_NvidiaExt.IncrementCounter();
}

void NvReorderThread(NvHitObject HitObj)
{
    NvReorderThread(HitObj, 0, 0);
}

template<typename T>
void NvInvokeHitObject(
    RaytracingAccelerationStructure AccelerationStructure,
    NvHitObject HitObj,
    inout T Payload)
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_INVOKE;
    g_NvidiaExt[index].src0u.x = HitObj._handle;
    uint handle = g_NvidiaExt.IncrementCounter();

    TraceRay(AccelerationStructure, 0, 0, 0, 0, handle, (RayDesc)0, Payload);
}

// Macro-based version of the HitObject API. Use this when HLSL 2021 is not available.
// Enable by specifying #define NV_HITOBJECT_USE_MACRO_API before including this header.
#elif defined(NV_HITOBJECT_USE_MACRO_API)

struct NvHitObject {
    uint _handle;

    bool IsMiss()
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_IS_MISS;
        g_NvidiaExt[index].src0u.x = _handle;
        uint ret = g_NvidiaExt.IncrementCounter();
        return ret != 0;
    }

    bool IsHit()
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_IS_HIT;
        g_NvidiaExt[index].src0u.x = _handle;
        uint ret = g_NvidiaExt.IncrementCounter();
        return ret != 0;
    }

    bool IsNop()
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_IS_NOP;
        g_NvidiaExt[index].src0u.x = _handle;
        uint ret = g_NvidiaExt.IncrementCounter();
        return ret != 0;
    }

    uint GetInstanceID()
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_GET_INSTANCE_ID;
        g_NvidiaExt[index].src0u.x = _handle;
        return g_NvidiaExt.IncrementCounter();
    }

    uint GetInstanceIndex()
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_GET_INSTANCE_INDEX;
        g_NvidiaExt[index].src0u.x = _handle;
        return g_NvidiaExt.IncrementCounter();
    }

    uint GetPrimitiveIndex()
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_GET_PRIMITIVE_INDEX;
        g_NvidiaExt[index].src0u.x = _handle;
        return g_NvidiaExt.IncrementCounter();
    }

    uint GetGeometryIndex()
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_GET_GEOMETRY_INDEX;
        g_NvidiaExt[index].src0u.x = _handle;
        return g_NvidiaExt.IncrementCounter();
    }

    uint GetHitKind()
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_GET_HIT_KIND;
        g_NvidiaExt[index].src0u.x = _handle;
        return g_NvidiaExt.IncrementCounter();
    }

    RayDesc GetRayDesc()
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_GET_RAY_DESC;
        g_NvidiaExt[index].src0u.x = _handle;

        uint tmin = g_NvidiaExt.IncrementCounter();
        uint tmax = g_NvidiaExt.IncrementCounter();
        uint rayOrgX = g_NvidiaExt.IncrementCounter();
        uint rayOrgY = g_NvidiaExt.IncrementCounter();
        uint rayOrgZ = g_NvidiaExt.IncrementCounter();
        uint rayDirX = g_NvidiaExt.IncrementCounter();
        uint rayDirY = g_NvidiaExt.IncrementCounter();
        uint rayDirZ = g_NvidiaExt.IncrementCounter();

        RayDesc ray;
        ray.TMin = asfloat(tmin);
        ray.TMax = asfloat(tmax);
        ray.Origin.x = asfloat(rayOrgX);
        ray.Origin.y = asfloat(rayOrgY);
        ray.Origin.z = asfloat(rayOrgZ);
        ray.Direction.x = asfloat(rayDirX);
        ray.Direction.y = asfloat(rayDirY);
        ray.Direction.z = asfloat(rayDirZ);

        return ray;
    }    

    uint GetShaderTableIndex()
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_GET_SHADER_TABLE_INDEX;
        g_NvidiaExt[index].src0u.x = _handle;
        return g_NvidiaExt.IncrementCounter();
    }

    uint LoadLocalRootTableConstant(uint RootConstantOffsetInBytes)
    {
        uint index = g_NvidiaExt.IncrementCounter();
        g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_LOAD_LOCAL_ROOT_TABLE_CONSTANT;
        g_NvidiaExt[index].src0u.x = _handle;
        g_NvidiaExt[index].src0u.y = RootConstantOffsetInBytes;
        return g_NvidiaExt.IncrementCounter();
    }
};

#define NvTraceRayHitObject(AccelerationStructure,RayFlags,InstanceInclusionMask,RayContributionToHitGroupIndex,MultiplierForGeometryContributionToHitGroupIndex,MissShaderIndex,Ray,Payload,ResultHitObj) \
do { \
    uint _rayFlags = RayFlags; \
    uint _instanceInclusionMask = InstanceInclusionMask; \
    uint _rayContributionToHitGroupIndex = RayContributionToHitGroupIndex; \
    uint _multiplierForGeometryContributionToHitGroupIndex = MultiplierForGeometryContributionToHitGroupIndex; \
    uint _missShaderIndex = MissShaderIndex; \
    RayDesc _ray = Ray; \
    uint _index = g_NvidiaExt.IncrementCounter(); \
    g_NvidiaExt[_index].opcode = NV_EXTN_OP_HIT_OBJECT_TRACE_RAY; \
    g_NvidiaExt[_index].numOutputsForIncCounter = 2; \
    g_NvidiaExt[_index].src0u.x = _missShaderIndex; \
    uint _hitHandle = g_NvidiaExt.IncrementCounter(); \
    uint _traceHandle = g_NvidiaExt.IncrementCounter(); \
    TraceRay(AccelerationStructure, _rayFlags, _instanceInclusionMask, _rayContributionToHitGroupIndex, _multiplierForGeometryContributionToHitGroupIndex, _traceHandle, _ray, Payload); \
    ResultHitObj._handle = _hitHandle; \
} while(0)

struct NvHitObjectMacroDummyPayloadType { int a; };

#define NvMakeHit(AccelerationStructure,InstanceIndex,GeometryIndex,PrimitiveIndex,HitKind,RayContributionToHitGroupIndex,MultiplierForGeometryContributionToHitGroupIndex,Ray,Attributes,ResultHitObj) \
do { \
    uint _instanceIndex = InstanceIndex; \
    uint _geometryIndex = GeometryIndex; \
    uint _primitiveIndex = PrimitiveIndex; \
    uint _hitKind = HitKind; \
    uint _rayContributionToHitGroupIndex = RayContributionToHitGroupIndex; \
    uint _multiplierForGeometryContributionToHitGroupIndex = MultiplierForGeometryContributionToHitGroupIndex; \
    RayDesc _ray = Ray; \
    uint _index = g_NvidiaExt.IncrementCounter(); \
    g_NvidiaExt[_index].opcode = NV_EXTN_OP_HIT_OBJECT_MAKE_HIT; \
    g_NvidiaExt[_index].numOutputsForIncCounter = 2; \
    g_NvidiaExt[_index].src0u.x = _instanceIndex; \
    g_NvidiaExt[_index].src0u.y = _geometryIndex; \
    g_NvidiaExt[_index].src0u.z = _primitiveIndex; \
    g_NvidiaExt[_index].src0u.w = _hitKind; \
    g_NvidiaExt[_index].src1u.x = _rayContributionToHitGroupIndex; \
    g_NvidiaExt[_index].src1u.y = _multiplierForGeometryContributionToHitGroupIndex; \
    uint _hitHandle = g_NvidiaExt.IncrementCounter(); \
    uint _traceHandle = g_NvidiaExt.IncrementCounter(); \
    CallShader(_traceHandle, Attributes); \
    NvHitObjectMacroDummyPayloadType _payload; \
    TraceRay(AccelerationStructure, 0, 0, 0, 0, _traceHandle, _ray, _payload); \
    ResultHitObj._handle = _hitHandle; \
} while(0)

#define NvMakeHitWithRecordIndex(HitGroupRecordIndex,AccelerationStructure,InstanceIndex,GeometryIndex,PrimitiveIndex,HitKind,Ray,Attributes,ResultHitObj) \
do { \
    uint _hitGroupRecordIndex = HitGroupRecordIndex; \
    uint _instanceIndex = InstanceIndex; \
    uint _geometryIndex = GeometryIndex; \
    uint _primitiveIndex = PrimitiveIndex; \
    uint _hitKind = HitKind; \
    RayDesc _ray = Ray; \
    uint _index = g_NvidiaExt.IncrementCounter(); \
    g_NvidiaExt[_index].opcode = NV_EXTN_OP_HIT_OBJECT_MAKE_HIT_WITH_RECORD_INDEX; \
    g_NvidiaExt[_index].numOutputsForIncCounter = 2; \
    g_NvidiaExt[_index].src0u.x = _instanceIndex; \
    g_NvidiaExt[_index].src0u.y = _geometryIndex; \
    g_NvidiaExt[_index].src0u.z = _primitiveIndex; \
    g_NvidiaExt[_index].src0u.w = _hitKind; \
    g_NvidiaExt[_index].src1u.x = _hitGroupRecordIndex; \
    uint _hitHandle = g_NvidiaExt.IncrementCounter(); \
    uint _traceHandle = g_NvidiaExt.IncrementCounter(); \
    CallShader(_traceHandle, Attributes); \
    NvHitObjectMacroDummyPayloadType _payload; \
    TraceRay(AccelerationStructure, 0, 0, 0, 0, _traceHandle, _ray, _payload); \
    ResultHitObj._handle = _hitHandle; \
} while(0)

NvHitObject NvMakeMiss(
    uint MissShaderIndex,
    RayDesc Ray)
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_MAKE_MISS;
    g_NvidiaExt[index].src0u.x = MissShaderIndex;
    g_NvidiaExt[index].src0u.y = asuint(Ray.TMin);
    g_NvidiaExt[index].src0u.z = asuint(Ray.TMax);
    g_NvidiaExt[index].src1u.x = asuint(Ray.Origin.x);
    g_NvidiaExt[index].src1u.y = asuint(Ray.Origin.y);
    g_NvidiaExt[index].src1u.z = asuint(Ray.Origin.z);
    g_NvidiaExt[index].src2u.x = asuint(Ray.Direction.x);
    g_NvidiaExt[index].src2u.y = asuint(Ray.Direction.y);
    g_NvidiaExt[index].src2u.z = asuint(Ray.Direction.z);
    uint hitHandle = g_NvidiaExt.IncrementCounter();

    NvHitObject hitObj;
    hitObj._handle = hitHandle;
    return hitObj;
}

NvHitObject NvMakeNop()
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_MAKE_NOP;
    uint hitHandle = g_NvidiaExt.IncrementCounter();

    NvHitObject hitObj;
    hitObj._handle = hitHandle;
    return hitObj;
}

#define NvGetAttributesFromHitObject(HitObj,ResultAttributes) \
do { \
    uint _index = g_NvidiaExt.IncrementCounter(); \
    g_NvidiaExt[_index].opcode = NV_EXTN_OP_HIT_OBJECT_GET_ATTRIBUTES; \
    g_NvidiaExt[_index].src0u.x = HitObj._handle; \
    uint _callHandle = g_NvidiaExt.IncrementCounter(); \
    CallShader(_callHandle, ResultAttributes); \
} while(0)

void NvReorderThread(uint CoherenceHint, uint NumCoherenceHintBits)
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_REORDER_THREAD;
    g_NvidiaExt[index].src0u.x = 0;
    g_NvidiaExt[index].src0u.y = 0;
    g_NvidiaExt[index].src0u.z = CoherenceHint;
    g_NvidiaExt[index].src0u.w = NumCoherenceHintBits;
    g_NvidiaExt.IncrementCounter();
}

void NvReorderThread(NvHitObject HitObj, uint CoherenceHint, uint NumCoherenceHintBits)
{
    uint index = g_NvidiaExt.IncrementCounter();
    g_NvidiaExt[index].opcode = NV_EXTN_OP_HIT_OBJECT_REORDER_THREAD;
    g_NvidiaExt[index].src0u.x = 1;
    g_NvidiaExt[index].src0u.y = HitObj._handle;
    g_NvidiaExt[index].src0u.z = CoherenceHint;
    g_NvidiaExt[index].src0u.w = NumCoherenceHintBits;
    g_NvidiaExt.IncrementCounter();
}

void NvReorderThread(NvHitObject HitObj)
{
    NvReorderThread(HitObj, 0, 0);
}

#define NvInvokeHitObject(AccelerationStructure,HitObj,Payload) \
do { \
    uint _index = g_NvidiaExt.IncrementCounter(); \
    g_NvidiaExt[_index].opcode = NV_EXTN_OP_HIT_OBJECT_INVOKE; \
    g_NvidiaExt[_index].src0u.x = HitObj._handle; \
    uint _handle = g_NvidiaExt.IncrementCounter(); \
    TraceRay(AccelerationStructure, 0, 0, 0, 0, _handle, (RayDesc)0, Payload); \
} while(0)

#endif
)"