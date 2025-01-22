#pragma once

#include <dxgi.h>
#include <stdint.h>
#include <cstdio>

#include "..\ThirdParty\TinyImage\tinyimageformat_apis.h"
#include "..\ThirdParty\TinyImage\tinyimageformat_query.h"
#include "..\Math\SEMath_Utility.h"

//--------------------------------------------------------------------------------------
// Macros
//--------------------------------------------------------------------------------------
#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8) |       \
                ((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24 ))
#endif /* defined(MAKEFOURCC) */

#define DDS_FOURCC      0x00000004  // DDPF_FOURCC
#define DDS_HEIGHT 0x00000002 // DDSD_HEIGHT
#define DDS_HEADER_FLAGS_VOLUME         0x00800000  // DDSD_DEPTH
#define DDS_RGB         0x00000040  // DDPF_RGB
#define DDS_LUMINANCE   0x00020000  // DDPF_LUMINANCE
#define DDS_ALPHA       0x00000002  // DDPF_ALPHA
#define DDS_BUMPDUDV    0x00080000  // DDPF_BUMPDUDV

#define DDS_CUBEMAP_POSITIVEX 0x00000600 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEX
#define DDS_CUBEMAP_NEGATIVEX 0x00000a00 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEX
#define DDS_CUBEMAP_POSITIVEY 0x00001200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEY
#define DDS_CUBEMAP_NEGATIVEY 0x00002200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEY
#define DDS_CUBEMAP_POSITIVEZ 0x00004200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEZ
#define DDS_CUBEMAP_NEGATIVEZ 0x00008200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEZ

#define DDS_CUBEMAP_ALLFACES ( DDS_CUBEMAP_POSITIVEX | DDS_CUBEMAP_NEGATIVEX |\
                                DDS_CUBEMAP_POSITIVEY | DDS_CUBEMAP_NEGATIVEY |\
                                DDS_CUBEMAP_POSITIVEZ | DDS_CUBEMAP_NEGATIVEZ )

#define DDS_CUBEMAP 0x00000200 // DDSCAPS2_CUBEMAP

#define SE_SUCCESS 0
#define SE_NOT_SUPPORTED -1
#define SE_INVALID_DATA -2
//--------------------------------------------------------------------------------------


const uint32_t DDS_MAGIC = 0x20534444; // "DDS "
const uint32_t DX10 = 0x30315844; //"DX10"

struct DDS_PIXELFORMAT
{
    uint32_t    size;
    uint32_t    flags;
    uint32_t    fourCC;
    uint32_t    RGBBitCount;
    uint32_t    RBitMask;
    uint32_t    GBitMask;
    uint32_t    BBitMask;
    uint32_t    ABitMask;
};

struct DDS_HEADER
{
    uint32_t        size;
    uint32_t        flags;
    uint32_t        height;
    uint32_t        width;
    uint32_t        pitchOrLinearSize;
    uint32_t        depth; // only if DDS_HEADER_FLAGS_VOLUME is set in flags
    uint32_t        mipMapCount;
    uint32_t        reserved1[11];
    DDS_PIXELFORMAT ddspf;
    uint32_t        caps;
    uint32_t        caps2;
    uint32_t        caps3;
    uint32_t        caps4;
    uint32_t        reserved2;
};

struct DDS_HEADER_DXT10
{
    DXGI_FORMAT     dxgiFormat;
    uint32_t        resourceDimension;
    uint32_t        miscFlag;
    uint32_t        arraySize;
    uint32_t        miscFlags2;
};

enum TextureDimension
{
    SE_TEXTURE_UNKNOWN = 0,
    SE_TEXTURE_1D = 2,
    SE_TEXTURE_2D = 3,
    SE_TEXTURE_3D = 4
};

struct ImageInfo
{
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t rowBytes;
    uint32_t numBytes;
    uint32_t numRows;
    void* data;
};

struct TextureInfo
{
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mipCount;
    uint32_t arraySize;

    TextureDimension resDim;
    TinyImageFormat format;
    bool isCubeMap;

    ImageInfo* images;
};

inline void ReadDDSFile(const char* filename, uint8_t** bitData, uint32_t* numBytes, DDS_HEADER* ddsHeader, DDS_HEADER_DXT10* ddsHeader10)
{
    FILE* file = nullptr;
    fopen_s(&file, filename, "rb");

    char errMsg[1024]{};

    if (!file)
    {
        sprintf_s(errMsg, "Failed to open file %s. Exiting program.\n", filename);
        MessageBoxA(nullptr, errMsg, "File open error.", MB_OK);
        exit(4);
    }

    fseek(file, 0, SEEK_END);

    uint32_t fileSize = ftell(file);

    fseek(file, 0, SEEK_SET);

    uint32_t magicNum = 0;
    fread(&magicNum, sizeof(uint32_t), 1, file);
    if (magicNum != DDS_MAGIC)
    {
        sprintf_s(errMsg, "%s is not a DDS file. Exiting Progam.\n", filename);
        MessageBoxA(nullptr, errMsg, "DDS file error.", MB_OK);
        fclose(file);
        exit(4);
    }

    fread(ddsHeader, sizeof(DDS_HEADER), 1, file);
    if (ddsHeader->size != sizeof(DDS_HEADER))
    {
        sprintf_s(errMsg, "DDS Header file size != 124 bytes. Exiting Progam.");
        MessageBoxA(nullptr, errMsg, "DDS header error.", MB_OK);
        fclose(file);
        exit(4);
    }

    if (ddsHeader->ddspf.size != sizeof(DDS_PIXELFORMAT))
    {
        sprintf_s(errMsg, "DDS Pixel Format file size != 32 bytes. Exiting Progam.");
        MessageBoxA(nullptr, errMsg, "DDS Pixel Format error.", MB_OK);
        fclose(file);
        exit(4);
    }

    //Check for DX10 extension
    bool bDXT10Header = false;
    if (ddsHeader->ddspf.flags & DDS_FOURCC && ddsHeader->ddspf.fourCC == DX10)
    {
        fread(ddsHeader10, sizeof(DDS_HEADER_DXT10), 1, file);
        bDXT10Header = true;
    }

    uint32_t offset = sizeof(uint32_t) + sizeof(DDS_HEADER) + (bDXT10Header ? sizeof(DDS_HEADER_DXT10) : 0);
    *numBytes = fileSize - offset;
    *bitData = (uint8_t*)calloc(*numBytes, sizeof(uint8_t));
    fread(*bitData, *numBytes, 1, file);

    fclose(file);
}

//--------------------------------------------------------------------------------------
// Return the BPP for a particular format
//--------------------------------------------------------------------------------------
inline size_t BitsPerPixel(_In_ DXGI_FORMAT fmt) noexcept
{
    switch (fmt)
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return 128;

    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        return 96;

    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
    case DXGI_FORMAT_Y416:
    case DXGI_FORMAT_Y210:
    case DXGI_FORMAT_Y216:
        return 64;

    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
    case DXGI_FORMAT_AYUV:
    case DXGI_FORMAT_Y410:
    case DXGI_FORMAT_YUY2:
        return 32;

    case DXGI_FORMAT_P010:
    case DXGI_FORMAT_P016:
    case DXGI_FORMAT_V408:
        return 24;

    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
    case DXGI_FORMAT_A8P8:
    case DXGI_FORMAT_B4G4R4A4_UNORM:
    case DXGI_FORMAT_P208:
    case DXGI_FORMAT_V208:
        return 16;

    case DXGI_FORMAT_NV12:
    case DXGI_FORMAT_420_OPAQUE:
    case DXGI_FORMAT_NV11:
        return 12;

    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
    case DXGI_FORMAT_AI44:
    case DXGI_FORMAT_IA44:
    case DXGI_FORMAT_P8:
        return 8;

    case DXGI_FORMAT_R1_UNORM:
        return 1;

    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        return 4;

    default:
        return 0;
    }
}

#define ISBITMASK( r,g,b,a ) ( ddpf.RBitMask == r && ddpf.GBitMask == g && ddpf.BBitMask == b && ddpf.ABitMask == a )

inline DXGI_FORMAT GetDXGIFormat(const DDS_PIXELFORMAT& ddpf) noexcept
{
    if (ddpf.flags & DDS_RGB)
    {
        // Note that sRGB formats are written using the "DX10" extended header

        switch (ddpf.RGBBitCount)
        {
        case 32:
            if (ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
            {
                return DXGI_FORMAT_R8G8B8A8_UNORM;
            }

            if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000))
            {
                return DXGI_FORMAT_B8G8R8A8_UNORM;
            }

            if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0))
            {
                return DXGI_FORMAT_B8G8R8X8_UNORM;
            }

            // No DXGI format maps to ISBITMASK(0x000000ff,0x0000ff00,0x00ff0000,0) aka D3DFMT_X8B8G8R8

            // Note that many common DDS reader/writers (including D3DX) swap the
            // the RED/BLUE masks for 10:10:10:2 formats. We assume
            // below that the 'backwards' header mask is being used since it is most
            // likely written by D3DX. The more robust solution is to use the 'DX10'
            // header extension and specify the DXGI_FORMAT_R10G10B10A2_UNORM format directly

            // For 'correct' writers, this should be 0x000003ff,0x000ffc00,0x3ff00000 for RGB data
            if (ISBITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000))
            {
                return DXGI_FORMAT_R10G10B10A2_UNORM;
            }

            // No DXGI format maps to ISBITMASK(0x000003ff,0x000ffc00,0x3ff00000,0xc0000000) aka D3DFMT_A2R10G10B10

            if (ISBITMASK(0x0000ffff, 0xffff0000, 0, 0))
            {
                return DXGI_FORMAT_R16G16_UNORM;
            }

            if (ISBITMASK(0xffffffff, 0, 0, 0))
            {
                // Only 32-bit color channel format in D3D9 was R32F
                return DXGI_FORMAT_R32_FLOAT; // D3DX writes this out as a FourCC of 114
            }
            break;

        case 24:
            // No 24bpp DXGI formats aka D3DFMT_R8G8B8
            break;

        case 16:
            if (ISBITMASK(0x7c00, 0x03e0, 0x001f, 0x8000))
            {
                return DXGI_FORMAT_B5G5R5A1_UNORM;
            }
            if (ISBITMASK(0xf800, 0x07e0, 0x001f, 0))
            {
                return DXGI_FORMAT_B5G6R5_UNORM;
            }

            // No DXGI format maps to ISBITMASK(0x7c00,0x03e0,0x001f,0) aka D3DFMT_X1R5G5B5

            if (ISBITMASK(0x0f00, 0x00f0, 0x000f, 0xf000))
            {
                return DXGI_FORMAT_B4G4R4A4_UNORM;
            }

            // NVTT versions 1.x wrote this as RGB instead of LUMINANCE
            if (ISBITMASK(0x00ff, 0, 0, 0xff00))
            {
                return DXGI_FORMAT_R8G8_UNORM;
            }
            if (ISBITMASK(0xffff, 0, 0, 0))
            {
                return DXGI_FORMAT_R16_UNORM;
            }

            // No DXGI format maps to ISBITMASK(0x0f00,0x00f0,0x000f,0) aka D3DFMT_X4R4G4B4

            // No 3:3:2:8 or paletted DXGI formats aka D3DFMT_A8R3G3B2, D3DFMT_A8P8, etc.
            break;

        case 8:
            // NVTT versions 1.x wrote this as RGB instead of LUMINANCE
            if (ISBITMASK(0xff, 0, 0, 0))
            {
                return DXGI_FORMAT_R8_UNORM;
            }

            // No 3:3:2 or paletted DXGI formats aka D3DFMT_R3G3B2, D3DFMT_P8
            break;

        default:
            return DXGI_FORMAT_UNKNOWN;
        }
    }
    else if (ddpf.flags & DDS_LUMINANCE)
    {
        switch (ddpf.RGBBitCount)
        {
        case 16:
            if (ISBITMASK(0xffff, 0, 0, 0))
            {
                return DXGI_FORMAT_R16_UNORM; // D3DX10/11 writes this out as DX10 extension
            }
            if (ISBITMASK(0x00ff, 0, 0, 0xff00))
            {
                return DXGI_FORMAT_R8G8_UNORM; // D3DX10/11 writes this out as DX10 extension
            }
            break;

        case 8:
            if (ISBITMASK(0xff, 0, 0, 0))
            {
                return DXGI_FORMAT_R8_UNORM; // D3DX10/11 writes this out as DX10 extension
            }

            // No DXGI format maps to ISBITMASK(0x0f,0,0,0xf0) aka D3DFMT_A4L4

            if (ISBITMASK(0x00ff, 0, 0, 0xff00))
            {
                return DXGI_FORMAT_R8G8_UNORM; // Some DDS writers assume the bitcount should be 8 instead of 16
            }
            break;

        default:
            return DXGI_FORMAT_UNKNOWN;
        }
    }
    else if (ddpf.flags & DDS_ALPHA)
    {
        if (8 == ddpf.RGBBitCount)
        {
            return DXGI_FORMAT_A8_UNORM;
        }
    }
    else if (ddpf.flags & DDS_BUMPDUDV)
    {
        switch (ddpf.RGBBitCount)
        {
        case 32:
            if (ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
            {
                return DXGI_FORMAT_R8G8B8A8_SNORM; // D3DX10/11 writes this out as DX10 extension
            }
            if (ISBITMASK(0x0000ffff, 0xffff0000, 0, 0))
            {
                return DXGI_FORMAT_R16G16_SNORM; // D3DX10/11 writes this out as DX10 extension
            }

            // No DXGI format maps to ISBITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000) aka D3DFMT_A2W10V10U10
            break;

        case 16:
            if (ISBITMASK(0x00ff, 0xff00, 0, 0))
            {
                return DXGI_FORMAT_R8G8_SNORM; // D3DX10/11 writes this out as DX10 extension
            }
            break;

        default:
            return DXGI_FORMAT_UNKNOWN;
        }

        // No DXGI format maps to DDPF_BUMPLUMINANCE aka D3DFMT_L6V5U5, D3DFMT_X8L8V8U8
    }
    else if (ddpf.flags & DDS_FOURCC)
    {
        if (MAKEFOURCC('D', 'X', 'T', '1') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC1_UNORM;
        }
        if (MAKEFOURCC('D', 'X', 'T', '3') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC2_UNORM;
        }
        if (MAKEFOURCC('D', 'X', 'T', '5') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC3_UNORM;
        }

        // While pre-multiplied alpha isn't directly supported by the DXGI formats,
        // they are basically the same as these BC formats so they can be mapped
        if (MAKEFOURCC('D', 'X', 'T', '2') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC2_UNORM;
        }
        if (MAKEFOURCC('D', 'X', 'T', '4') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC3_UNORM;
        }

        if (MAKEFOURCC('A', 'T', 'I', '1') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC4_UNORM;
        }
        if (MAKEFOURCC('B', 'C', '4', 'U') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC4_UNORM;
        }
        if (MAKEFOURCC('B', 'C', '4', 'S') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC4_SNORM;
        }

        if (MAKEFOURCC('A', 'T', 'I', '2') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC5_UNORM;
        }
        if (MAKEFOURCC('B', 'C', '5', 'U') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC5_UNORM;
        }
        if (MAKEFOURCC('B', 'C', '5', 'S') == ddpf.fourCC)
        {
            return DXGI_FORMAT_BC5_SNORM;
        }

        // BC6H and BC7 are written using the "DX10" extended header

        if (MAKEFOURCC('R', 'G', 'B', 'G') == ddpf.fourCC)
        {
            return DXGI_FORMAT_R8G8_B8G8_UNORM;
        }
        if (MAKEFOURCC('G', 'R', 'G', 'B') == ddpf.fourCC)
        {
            return DXGI_FORMAT_G8R8_G8B8_UNORM;
        }

        if (MAKEFOURCC('Y', 'U', 'Y', '2') == ddpf.fourCC)
        {
            return DXGI_FORMAT_YUY2;
        }

        // Check for D3DFORMAT enums being set here
        switch (ddpf.fourCC)
        {
        case 36: // D3DFMT_A16B16G16R16
            return DXGI_FORMAT_R16G16B16A16_UNORM;

        case 110: // D3DFMT_Q16W16V16U16
            return DXGI_FORMAT_R16G16B16A16_SNORM;

        case 111: // D3DFMT_R16F
            return DXGI_FORMAT_R16_FLOAT;

        case 112: // D3DFMT_G16R16F
            return DXGI_FORMAT_R16G16_FLOAT;

        case 113: // D3DFMT_A16B16G16R16F
            return DXGI_FORMAT_R16G16B16A16_FLOAT;

        case 114: // D3DFMT_R32F
            return DXGI_FORMAT_R32_FLOAT;

        case 115: // D3DFMT_G32R32F
            return DXGI_FORMAT_R32G32_FLOAT;

        case 116: // D3DFMT_A32B32G32R32F
            return DXGI_FORMAT_R32G32B32A32_FLOAT;

            // No DXGI format maps to D3DFMT_CxV8U8

        default:
            return DXGI_FORMAT_UNKNOWN;
        }
    }

    return DXGI_FORMAT_UNKNOWN;
}

#undef ISBITMASK

inline void GetImageInfo(uint32_t w, uint32_t h, TinyImageFormat fmt,
    uint32_t* outNumBytes, uint32_t* outRowBytes,
    uint32_t* outNumRows)
{
    uint32_t numBytes = 0;
    uint32_t rowBytes = 0;
    uint32_t numRows = 0;

    bool bc = false;
    bool packed = false;
    bool planar = false;
    uint32_t bpe = 0;

    switch (fmt)
    {
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
        bc = true;
        bpe = 8;
        break;

    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        bc = true;
        bpe = 16;
        break;

    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_YUY2:
        packed = true;
        bpe = 4;
        break;

    case DXGI_FORMAT_Y210:
    case DXGI_FORMAT_Y216:
        packed = true;
        bpe = 8;
        break;

    case DXGI_FORMAT_NV12:
    case DXGI_FORMAT_420_OPAQUE:
        planar = true;
        bpe = 2;
        break;

    case DXGI_FORMAT_P010:
    case DXGI_FORMAT_P016:
        planar = true;
        bpe = 4;
        break;
    default:
        break;
    }

    if (bc)
    {
        uint32_t numBlocksWide = 0;
        if (w > 0)
        {
            numBlocksWide = SEMax(1, (w + 3) / 4);
        }
        uint32_t numBlocksHigh = 0;
        if (h > 0)
        {
            numBlocksHigh = SEMax(1, (h + 3) / 4);
        }
        rowBytes = numBlocksWide * bpe;
        numRows = numBlocksHigh;
        numBytes = rowBytes * numBlocksHigh;
    }
    else if (packed)
    {
        rowBytes = ((w + 1) >> 1) * bpe;
        numRows = h;
        numBytes = rowBytes * h;
    }
    else if (fmt == DXGI_FORMAT_NV11)
    {
        rowBytes = ((w + 3) >> 2) * 4;
        numRows = h * 2;
        numBytes = rowBytes + numRows;
    }
    else if (planar)
    {
        rowBytes = ((w + 1) >> 1) * bpe;
        numBytes = (rowBytes * h) + ((rowBytes * h + 1) >> 1);
        numRows = h + ((h + 1) >> 1);
    }
    else
    {
        uint32_t bpp = TinyImageFormat_BitSizeOfBlock(fmt);
        rowBytes = (w * bpp + 7) / 8;
        numRows = h;
        numBytes = rowBytes * h;
    }

    if (outNumBytes)
    {
        *outNumBytes = numBytes;
    }
    if (outRowBytes)
    {
        *outRowBytes = rowBytes;
    }
    if (outNumRows)
    {
        *outNumRows = numRows;
    }
}

inline int RetrieveTextureInfo(const DDS_HEADER* const ddsHeader, const DDS_HEADER_DXT10* const ddsHeader10, uint8_t* bitData, const uint32_t numBytes,
    TextureInfo* textureInfo)
{
    textureInfo->width = ddsHeader->width;
    textureInfo->height = ddsHeader->height;
    textureInfo->depth = ddsHeader->depth;

    //SETextureDimension resDim = SE_TEXTURE_UNKNOWN;
    textureInfo->arraySize = 1;
    textureInfo->format = TinyImageFormat_UNDEFINED;
    textureInfo->isCubeMap = false;

    textureInfo->mipCount = ddsHeader->mipMapCount;
    if (textureInfo->mipCount == 0)
    {
        textureInfo->mipCount = 1;
    }

    if (ddsHeader->ddspf.flags & DDS_FOURCC && ddsHeader->ddspf.fourCC == DX10)
    {
        textureInfo->arraySize = ddsHeader10->arraySize;
        if (textureInfo->arraySize == 0)
        {
            return SE_INVALID_DATA;
        }

        switch (ddsHeader10->dxgiFormat)
        {
        case DXGI_FORMAT_NV12:
        case DXGI_FORMAT_P010:
        case DXGI_FORMAT_P016:
        case DXGI_FORMAT_420_OPAQUE:
            if ((ddsHeader10->resourceDimension != SE_TEXTURE_2D + 1)
                || (textureInfo->width % 2) != 0 || (textureInfo->height % 2) != 0)
            {
                return SE_NOT_SUPPORTED;
            }
            break;

        case DXGI_FORMAT_YUY2:
        case DXGI_FORMAT_Y210:
        case DXGI_FORMAT_Y216:
        case DXGI_FORMAT_P208:
            if ((textureInfo->width % 2) != 0)
            {
                return SE_NOT_SUPPORTED;
            }
            break;

        case DXGI_FORMAT_NV11:
            if ((textureInfo->width % 4) != 0)
            {
                return SE_NOT_SUPPORTED;
            }
            break;

        case DXGI_FORMAT_AI44:
        case DXGI_FORMAT_IA44:
        case DXGI_FORMAT_P8:
        case DXGI_FORMAT_A8P8:
            return SE_NOT_SUPPORTED;

        case DXGI_FORMAT_V208:
            if ((ddsHeader10->resourceDimension != SE_TEXTURE_2D + 1)
                || (textureInfo->height % 2) != 0)
            {
                return SE_NOT_SUPPORTED;
            }
            break;

        default:
            if (BitsPerPixel(ddsHeader10->dxgiFormat) == 0)
            {
                return SE_NOT_SUPPORTED;
            }
        }

        textureInfo->format = TinyImageFormat_FromDXGI_FORMAT((TinyImageFormat_DXGI_FORMAT)ddsHeader10->dxgiFormat);

        switch (ddsHeader10->resourceDimension)
        {
        case SE_TEXTURE_1D:
            // D3DX writes 1D textures with a fixed Height of 1
            if ((ddsHeader->flags & DDS_HEIGHT) && textureInfo->height != 1)
            {
                return SE_INVALID_DATA;
            }
            textureInfo->height = 1;
            textureInfo->depth = 1;
            textureInfo->resDim = SE_TEXTURE_1D;
            break;

        case SE_TEXTURE_2D:
            if (ddsHeader10->miscFlag & 0x4 /* RESOURCE_MISC_TEXTURECUBE */)
            {
                textureInfo->arraySize *= 6;
                textureInfo->isCubeMap = true;
            }
            textureInfo->depth = 1;
            textureInfo->resDim = SE_TEXTURE_2D;
            break;

        case SE_TEXTURE_3D:
            if (!(ddsHeader->flags & DDS_HEADER_FLAGS_VOLUME))
            {
                return SE_INVALID_DATA;
            }

            if (textureInfo->arraySize > 1)
            {
                return SE_NOT_SUPPORTED;
            }

            textureInfo->resDim = SE_TEXTURE_3D;
            break;

        default:
            return SE_NOT_SUPPORTED;
        }
    }
    else
    {
        textureInfo->format = TinyImageFormat_FromDXGI_FORMAT((TinyImageFormat_DXGI_FORMAT)GetDXGIFormat(ddsHeader->ddspf));

        if (textureInfo->format == DXGI_FORMAT_UNKNOWN)
        {
            return SE_NOT_SUPPORTED;
        }

        if (ddsHeader->flags & DDS_HEADER_FLAGS_VOLUME)
        {
            textureInfo->resDim = SE_TEXTURE_3D;
        }
        else
        {
            if (ddsHeader->caps2 & DDS_CUBEMAP)
            {
                // We require all six faces to be defined
                if ((ddsHeader->caps2 & DDS_CUBEMAP_ALLFACES) != DDS_CUBEMAP_ALLFACES)
                {
                    return SE_NOT_SUPPORTED;
                }

                textureInfo->arraySize = 6;
                textureInfo->isCubeMap = true;
            }

            textureInfo->depth = 1;
            textureInfo->resDim = SE_TEXTURE_2D;

            // Note there's no way for a legacy Direct3D 9 DDS to express a '1D' texture
        }

        if (TinyImageFormat_BitSizeOfBlock(textureInfo->format) == 0)
        {
            return SE_NOT_SUPPORTED;
        }
    }

    // Bound sizes (for security purposes we don't trust DDS file metadata larger than the Direct3D hardware requirements)
    //D3D12_REQ_MIP_LEVELS = 15
    if (textureInfo->mipCount > 15)
    {
        return SE_NOT_SUPPORTED;
    }

    switch (textureInfo->resDim)
    {
    case SE_TEXTURE_1D:
        //D3D12_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION = 2048
        //D3D12_REQ_TEXTURE1D_U_DIMENSION = 16384
        if ((textureInfo->arraySize > 2048) ||
            (textureInfo->width > 16384))
        {
            return SE_NOT_SUPPORTED;
        }
        break;

    case SE_TEXTURE_2D:
        if (textureInfo->isCubeMap)
        {
            // This is the right bound because we set arraySize to (NumCubes*6) above
            //D3D12_REQ_TEXTURED_ARRAY_AXIS_DIMENSION = 2048
            //D3D12_REQ_TEXTURECUBE_DIMENSION = 16384
            if ((textureInfo->arraySize > 2048) ||
                (textureInfo->width > 16384) ||
                (textureInfo->height > 16384))
            {
                return SE_NOT_SUPPORTED;
            }
        }
        //D3D12_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION = 2048
        //D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION = 16384
        else if ((textureInfo->arraySize > 2048) ||
            (textureInfo->width > 16384) ||
            (textureInfo->height > 16384))
        {
            return SE_NOT_SUPPORTED;
        }
        break;

    case SE_TEXTURE_3D:
        //D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION = 2048
        if ((textureInfo->arraySize > 1) ||
            (textureInfo->width > 2048) ||
            (textureInfo->height > 2048) ||
            (textureInfo->depth > 2048))
        {
            return SE_NOT_SUPPORTED;
        }
        break;

    default:
        return SE_NOT_SUPPORTED;
    }

    textureInfo->images = (ImageInfo*)calloc(textureInfo->arraySize * textureInfo->mipCount, sizeof(ImageInfo));
    uint8_t* srcBits = bitData;
    uint8_t* endBits = bitData + numBytes;
    uint32_t index = 0;
    for (uint32_t i = 0; i < textureInfo->arraySize; ++i)
    {
        uint32_t width = textureInfo->width;
        uint32_t height = textureInfo->height;
        uint32_t depth = textureInfo->depth;
        for (uint32_t j = 0; j < textureInfo->mipCount; ++j)
        {
            uint32_t bytesCount = 0;
            uint32_t rowBytes = 0;
            uint32_t numRows = 0;
            GetImageInfo(width, height, textureInfo->format, &bytesCount, &rowBytes, &numRows);
            textureInfo->images[index].width = width;
            textureInfo->images[index].height = height;
            textureInfo->images[index].depth = depth;
            textureInfo->images[index].rowBytes = rowBytes;
            textureInfo->images[index].numRows = numRows;
            textureInfo->images[index].numBytes = bytesCount;
            textureInfo->images[index].data = srcBits;

            if (srcBits + (bytesCount * depth) > endBits)
            {
                return SE_INVALID_DATA;
            }

            srcBits += bytesCount * depth;
            width = SEMax(1, width >> 1u);
            height = SEMax(1, height >> 1u);
            depth = SEMax(1, depth >> 1u);

            ++index;
        }
    }

    return SE_SUCCESS;
}