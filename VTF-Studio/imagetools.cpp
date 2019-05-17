#include "imagetools.h"

#include "xVTF/xLUTs.h"
#include "xVTF/xPixelFormats.h"

QImage* VTFStudio::ImageTools::CreateImageFromVTF
(xvtf::Bitmap::VTFFile* vtf, const unsigned int MipLevel, const unsigned int Frame,
 const unsigned int Face, const unsigned int zLevel)
{
    using namespace xvtf::Bitmap;

    // Get some data that will be used by all formats
    auto ImgFrmt = static_cast<VTF::ImageFormat>(vtf->GetImageFormat());
    BitmapImage* bmp = nullptr;
    Resolution res;

    if (!vtf->GetResolution(&res, MipLevel))
        return nullptr;
    
    if (!vtf->GetImage(bmp, nullptr, MipLevel, Frame, Face, zLevel))
        return nullptr;
    
    auto pixCount = bmp->GetPixelCount();
    
    // RGB888 Images
    if (ImgFrmt == VTF::ImageFormat::DXT1 || ImgFrmt == VTF::ImageFormat::RGB888 || ImgFrmt == VTF::ImageFormat::RGB888_BLUESCREEN)
    {
        uchar* out = new uchar[pixCount * 3];
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::RGB888*>(bmp->at(i));
            out[i * 3] = pix->R;
            out[i * 3 + 1] = pix->G;
            out[i * 3 + 2] = pix->B;
        }
        
        BitmapImage::Free(bmp);
        
        return new QImage(out, static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_RGB888);
    }
    
    // RGBA8888 Images
    if (ImgFrmt == VTF::ImageFormat::RGBA8888 || ImgFrmt == VTF::ImageFormat::DXT3 ||
            ImgFrmt == VTF::ImageFormat::DXT5 || ImgFrmt == VTF::ImageFormat::DXT1_ONEBITALPHA)
    {
        uchar* out = new uchar[pixCount * 4];
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::RGBA8888*>(bmp->at(i));
            out[i * 4] = pix->R;
            out[i * 4 + 1] = pix->G;
            out[i * 4 + 2] = pix->B;
            out[i * 4 + 3] = pix->A;
        }

        BitmapImage::Free(bmp);

        return new QImage(out, static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_RGBA8888);
    }

    // BGR888 Images
    if (ImgFrmt == VTF::ImageFormat::BGR888 || ImgFrmt == VTF::ImageFormat::BGR888_BLUESCREEN)
    {
        uchar* out = new uchar[pixCount * 3];
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::BGR888*>(bmp->at(i));
            out[i * 3] = pix->R;
            out[i * 3 + 1] = pix->G;
            out[i * 3 + 2] = pix->B;
        }

        BitmapImage::Free(bmp);

        return new QImage(out, static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_RGB888);
    }

    // ABGR8888 Images
    if (ImgFrmt == VTF::ImageFormat::ABGR8888)
    {
        uchar* out = new uchar[pixCount * 4];
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::ABGR8888*>(bmp->at(i));
            out[i * 4] = pix->A;
            out[i * 4 + 1] = pix->R;
            out[i * 4 + 2] = pix->G;
            out[i * 4 + 3] = pix->B;
        }

        BitmapImage::Free(bmp);

        return new QImage(out, static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_ARGB32);
    }

    // RGB565 Images
    if (ImgFrmt == VTF::ImageFormat::RGB565)
    {
        uint16_t* out = new uint16_t[pixCount];
        for (uint i = 0; i < pixCount; ++i)
        {
            out[i] = *static_cast<uint16_t*>(bmp->at(i));
        }

        BitmapImage::Free(bmp);

        return new QImage(reinterpret_cast<uchar*>(out), static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_RGB16);
    }

    // I8 (Grayscale) Images
    if (ImgFrmt == VTF::ImageFormat::I8 || ImgFrmt == VTF::ImageFormat::A8)
    {
        uchar* out = new uchar[pixCount];
        for (uint i = 0; i < pixCount; ++i)
        {
            out[i] = *static_cast<uchar*>(bmp->at(i));
        }

        BitmapImage::Free(bmp);

        return new QImage(out, static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_Grayscale8);
    }

    // IA88 (Grayscale w/ Alpha) Images
    if (ImgFrmt == VTF::ImageFormat::IA88)
    {
        uchar* out = new uchar[pixCount * 4];
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::IA88*>(bmp->at(i));
            out[0] = pix->A;
            out[1] = pix->I;
            out[2] = pix->I;
            out[3] = pix->I;
        }

        BitmapImage::Free(bmp);

        return new QImage(out, static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_ARGB32);
    }

    // P8 (Palleted) Images
    if (ImgFrmt == VTF::ImageFormat::P8)
    {
        uchar* out = new uchar[pixCount];
        for (uint i = 0; i < pixCount; ++i)
        {
            out[i] = *static_cast<uchar*>(bmp->at(i));
        }

        BitmapImage::Free(bmp);

        return new QImage(out, static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_Indexed8);
    }

    // ARGB8888 Images
    if (ImgFrmt == VTF::ImageFormat::ARGB8888)
    {
        uint32_t* out = new uint32_t[pixCount];
        for (uint i = 0; i < pixCount; ++i)
        {
            out[i] = *static_cast<uint32_t*>(bmp->at(i));
        }

        BitmapImage::Free(bmp);

        return new QImage(reinterpret_cast<uchar*>(out), static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_ARGB32);
    }

    // BGRA8888 Images
    if (ImgFrmt == VTF::ImageFormat::BGRA8888)
    {
        uchar* out = new uchar[pixCount * 4];
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::BGRA8888*>(bmp->at(i));
            out[i * 4] = pix->A;
            out[i * 4 + 1] = pix->R;
            out[i * 4 + 2] = pix->G;
            out[i * 4 + 3] = pix->B;
        }

        BitmapImage::Free(bmp);

        return new QImage(out, static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_ARGB32);
    }

    // BGRX8888 Images
    if (ImgFrmt == VTF::ImageFormat::BGRX8888)
    {
        uchar* out = new uchar[pixCount * 4];
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::BGRX8888*>(bmp->at(i));
            out[i * 4] = pix->R;
            out[i * 4 + 1] = pix->G;
            out[i * 4 + 2] = pix->B;
            out[i * 4 + 3] = pix->X;
        }

        BitmapImage::Free(bmp);

        return new QImage(out, static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_RGBX8888);
    }

    // BGRX5551 Images
    if (ImgFrmt == VTF::ImageFormat::BGRX5551)
    {
        uchar* out = new uchar[pixCount * 4];
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::BGRX5551*>(bmp->at(i));
            out[i * 4] = pix->R;
            out[i * 4 + 1] = pix->G;
            out[i * 4 + 2] = pix->B;
            out[i * 4 + 3] = pix->X == 1 ? 255 : 0;
        }

        BitmapImage::Free(bmp);

        return new QImage(out, static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_RGBX8888);
    }

    // BGR565 Images
    if (ImgFrmt == VTF::ImageFormat::BGR565)
    {
        uint16_t* out = new uint16_t[pixCount];
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = *static_cast<uint16_t*>(bmp->at(i));
            out[i] =   0b0000011111100000 & pix;
            out[i] += (0b1111100000000000 & pix) >> 11;
            out[i] += (0b0000000000011111 & pix) << 11;
        }

        BitmapImage::Free(bmp);

        return new QImage(reinterpret_cast<uchar*>(out), static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_RGB16);
    }

    // BGRA4444 Images
    if (ImgFrmt == VTF::ImageFormat::BGRA4444)
    {
        uchar* out = new uchar[pixCount * 4];
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::BGRA4444*>(bmp->at(i));
            out[i * 4] =  xvtf::Tools::LUT::LUT4[pix->A];
            out[i * 4 + 1] = xvtf::Tools::LUT::LUT4[pix->R];
            out[i * 4 + 2] = xvtf::Tools::LUT::LUT4[pix->G];
            out[i * 4 + 3] = xvtf::Tools::LUT::LUT4[pix->B];
        }

        BitmapImage::Free(bmp);

        return new QImage(out, static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_ARGB32);
    }

    // BGRA5551 Images
    if (ImgFrmt == VTF::ImageFormat::BGRA5551)
    {
        uchar* out = new uchar[pixCount * 4];
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::BGRA5551*>(bmp->at(i));
            out[i * 4] = pix->R;
            out[i * 4 + 1] = pix->G;
            out[i * 4 + 2] = pix->B;
            out[i * 4 + 3] = pix->A == 1 ? 255 : 0;
        }

        BitmapImage::Free(bmp);

        return new QImage(out, static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_RGBA8888);
    }

    // UV88 (Du/Dv) Images
    if (ImgFrmt == VTF::ImageFormat::UV88)
    {
        uchar* out = new uchar[pixCount * 3];
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::UV88*>(bmp->at(i));
            out[i * 3] = pix->U;
            out[i * 3 + 1] = pix->V;
            out[i * 3 + 2] = 0;
        }

        BitmapImage::Free(bmp);

        return new QImage(out, static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_RGB888);
    }

    // UVWQ8888/UVLX8888 (Normal Map) Images
    if (ImgFrmt == VTF::ImageFormat::UVWQ8888 || ImgFrmt == VTF::ImageFormat::UVLX8888)
    {
        uchar* out = new uchar[pixCount * 4];
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::UVWQ8888*>(bmp->at(i));
            out[i * 4] = pix->U;
            out[i * 4 + 1] = pix->V;
            out[i * 4 + 2] = pix->W;
            out[i * 4 + 3] = pix->Q;
        }

        BitmapImage::Free(bmp);

        return new QImage(out, static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_RGBX8888);
    }

    // RGBA16161616 (Integer HDR) Images
    if (ImgFrmt == VTF::ImageFormat::RGBA16161616)
    {
        uint64_t* out = new uint64_t[pixCount];
        for(uint i = 0; i < pixCount; ++i)
        {
            out[i] = *static_cast<uint64_t*>(bmp->at(i));
        }

        BitmapImage::Free(bmp);

        return new QImage(reinterpret_cast<uchar*>(out), static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_RGBA64);
    }

    return nullptr;
}
