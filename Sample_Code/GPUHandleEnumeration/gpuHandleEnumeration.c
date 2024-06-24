/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: LicenseRef-NvidiaProprietary
 *
 * NVIDIA CORPORATION, its affiliates and licensors retain all intellectual
 * property and proprietary rights in and to this material, related
 * documentation and any modifications thereto. Any use, reproduction,
 * disclosure or distribution of this material and related documentation
 * without an express license agreement from NVIDIA CORPORATION or
 * its affiliates is strictly prohibited.
 */

/*************************************************************/
/* gpuHandleEnumeration.cpp
/* 
/* The goal of this sample code is to demonstrate the usage of
/* the GPU handle enumeration APIs: 
/* 1) NvAPI_SYS_GetLogicalGPUs 
/* 2) NvAPI_SYS_GetPhysicalGPUs 
/*  
/* These APIs are capable of enumerating the GPU handles for 
/* all types of NVIDIA adapters in the system - WDDM/MCDM/TCC. 
/*  
/* The GPU handles obtained from these APIs can further be 
/* used to call other APIs.
/* 
/*************************************************************/

#include <stdio.h>
#include "nvapi.h"

// Statically link to the NvAPI static library
#pragma comment(lib, "nvapi64.lib")
int main()
{
    NvAPI_Status      status  = NVAPI_OK;
    NvAPI_ShortString szError = { 0 };

    // Initialize the NVAPI interface first.
    status = NvAPI_Initialize();
    if (status != NVAPI_OK)
    {
        NvAPI_GetErrorMessage(status, szError);
        printf("\nNvAPI_Initialize() failed. The Error code is: %s", szError);
        return -1;
    }

    //---------------------------------------------------------------------------
    //  Call the NvAPI_SYS_GetLogicalGPUs API and print the logical GPU handles
    //  with their adapter type.
    //---------------------------------------------------------------------------
    NV_LOGICAL_GPUS_V1 nvLogicalGPUs = { 0 };
    nvLogicalGPUs.version = NV_LOGICAL_GPUS_VER1;
    status = NvAPI_SYS_GetLogicalGPUs(&nvLogicalGPUs);
    if (status != NVAPI_OK)
    {
        NvAPI_GetErrorMessage(status, szError);
        printf("\nNvAPI_SYS_GetLogicalGPUs() failed. The Error code is: %s", szError);
        return -1;
    }

    printf("\n\nThe enumerated logical GPU handles are listed below. GPU_Count=%d.\n", nvLogicalGPUs.gpuHandleCount);
    for (NvU32 i = 0; i < nvLogicalGPUs.gpuHandleCount; i++)
    {
        printf("\nLogical_GPUHandle[%d]:0x%p | AdapterType = %d", i, nvLogicalGPUs.gpuHandleData[i].hLogicalGpu,
               nvLogicalGPUs.gpuHandleData[i].adapterType);
    }

    //----------------------------------------------------------------------------
    //  Call the NvAPI_SYS_GetPhysicalGPUs API and print the physical GPU handles
    //  with their adapter type
    //----------------------------------------------------------------------------
    NV_PHYSICAL_GPUS_V1 nvPhysicalGPUs = { 0 };
    nvPhysicalGPUs.version = NV_PHYSICAL_GPUS_VER1;
    status = NvAPI_SYS_GetPhysicalGPUs(&nvPhysicalGPUs);
    if (status != NVAPI_OK)
    {
        NvAPI_GetErrorMessage(status, szError);
        printf("\nNvAPI_SYS_GetPhysicalGPUs() failed. The Error code is: %s", szError);
        return -1;
    }

    for (NvU32 i = 0; i < nvPhysicalGPUs.gpuHandleCount; i++)
    {
        printf("\nPhysical_GPUHandle[%d]:0x%p | AdapterType = %d", i, nvPhysicalGPUs.gpuHandleData[i].hPhysicalGpu,
               nvPhysicalGPUs.gpuHandleData[i].adapterType);
    }

    return 0;
}
