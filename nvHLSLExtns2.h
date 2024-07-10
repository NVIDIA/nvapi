R"({
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
	)" R"(
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