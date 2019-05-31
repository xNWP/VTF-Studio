#include "imagetools.h"

#include "xVTF/flags.h"
#include "xVTF/luts.h"
#include "xVTF/pixelformats.h"

#include <QDebug>

std::vector<QColor> VTFStudio::ImageTools::CreateImageFromVTF
(xvtf::Bitmap::VTFFile* vtf, std::vector<ChannelLink>& channels, uint * const & error,
 const unsigned int MipLevel, const unsigned int Frame,
 const unsigned int Face, const unsigned int zLevel)
{
    using namespace xvtf::Bitmap;

    // Get some data that will be used by all formats
    auto ImgFrmt = static_cast<VTF::ImageFormat>(vtf->GetImageFormat());
    BitmapImage* bmp = nullptr;
    Resolution res;

    std::vector<QColor> rval;
    channels.clear();

    if (!vtf->GetResolution(&res, MipLevel, error))
        return rval;
    
    if (!vtf->GetImage(bmp, error, MipLevel, Frame, Face, zLevel))
        return rval;
    
    auto pixCount = bmp->GetPixelCount();
    rval.reserve(pixCount);
    
    // RGB888 Images
    if (ImgFrmt == VTF::ImageFormat::DXT1 || ImgFrmt == VTF::ImageFormat::RGB888 || ImgFrmt == VTF::ImageFormat::RGB888_BLUESCREEN)
    {
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::RGB888*>(bmp->at(i));
            rval.push_back(QColor(pix->R, pix->G, pix->B));
        }
        
        BitmapImage::Free(bmp);
        channels.push_back(ChannelLink("R", Channel::R));
        channels.push_back(ChannelLink("G", Channel::G));
        channels.push_back(ChannelLink("B", Channel::B));

        return rval;
    }
    
    // RGBA8888 Images
    if (ImgFrmt == VTF::ImageFormat::RGBA8888 || ImgFrmt == VTF::ImageFormat::DXT3 ||
            ImgFrmt == VTF::ImageFormat::DXT5 || ImgFrmt == VTF::ImageFormat::DXT1_ONEBITALPHA)
    {
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::RGBA8888*>(bmp->at(i));
            rval.push_back(QColor(pix->R, pix->G, pix->B, pix->A));
        }

        BitmapImage::Free(bmp);
        channels.push_back(ChannelLink("R", Channel::R));
        channels.push_back(ChannelLink("G", Channel::G));
        channels.push_back(ChannelLink("B", Channel::B));
        channels.push_back(ChannelLink("A", Channel::A));

        return rval;
    }

    // BGR888 Images
    if (ImgFrmt == VTF::ImageFormat::BGR888 || ImgFrmt == VTF::ImageFormat::BGR888_BLUESCREEN)
    {
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::BGR888*>(bmp->at(i));
            rval.push_back(QColor(pix->B, pix->G, pix->R));
        }

        BitmapImage::Free(bmp);
        channels.push_back(ChannelLink("B", Channel::B));
        channels.push_back(ChannelLink("G", Channel::G));
        channels.push_back(ChannelLink("R", Channel::R));

        return rval;
    }

    // ABGR8888 Images
    if (ImgFrmt == VTF::ImageFormat::ABGR8888)
    {
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::ABGR8888*>(bmp->at(i));
            rval.push_back(QColor(pix->A, pix->B, pix->G, pix->R));
        }

        BitmapImage::Free(bmp);
        channels.push_back(ChannelLink("A", Channel::A));
        channels.push_back(ChannelLink("B", Channel::B));
        channels.push_back(ChannelLink("G", Channel::G));
        channels.push_back(ChannelLink("R", Channel::R));

        return rval;
    }

    // RGB565 Images
    if (ImgFrmt == VTF::ImageFormat::RGB565)
    {
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::RGB565*>(bmp->at(i));
            rval.push_back(QColor(xvtf::Tools::LUT::LUT5[pix->R],
                                  xvtf::Tools::LUT::LUT6[pix->G],
                                  xvtf::Tools::LUT::LUT5[pix->B]));
        }

        BitmapImage::Free(bmp);
        channels.push_back(ChannelLink("R", Channel::R));
        channels.push_back(ChannelLink("G", Channel::G));
        channels.push_back(ChannelLink("B", Channel::B));

        return rval;
    }

    // I8 (Grayscale) Images
    if (ImgFrmt == VTF::ImageFormat::I8 || ImgFrmt == VTF::ImageFormat::A8)
    {
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::I8*>(bmp->at(i));
            rval.push_back(QColor(pix->I, 0, 0));
        }

        BitmapImage::Free(bmp);
        std::vector<Channel> ch = { Channel::R, Channel::G, Channel::B };
        channels.push_back(ChannelLink("I", ch));

        return rval;
    }

    // IA88 (Grayscale w/ Alpha) Images
    if (ImgFrmt == VTF::ImageFormat::IA88)
    {
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::IA88*>(bmp->at(i));
            rval.push_back(QColor(pix->I, pix->A, 0));
        }

        BitmapImage::Free(bmp);
        std::vector<Channel> ch = { Channel::R, Channel::G, Channel::B };
        channels.push_back(ChannelLink("I", ch));
        channels.push_back(ChannelLink("A", Channel::A));

        return rval;
    }

    // Not supported by the VTF format nor Qt
    //// P8 (Palleted) Images
    //if (ImgFrmt == VTF::ImageFormat::P8)
    //{
    //    uchar* out = new uchar[pixCount];
    //    for (uint i = 0; i < pixCount; ++i)
    //    {
    //        out[i] = *static_cast<uchar*>(bmp->at(i));
    //    }
    //
    //    BitmapImage::Free(bmp);
    //
    //    return new QImage(out, static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_Indexed8);
    //}

    // ARGB8888 Images
    // VTFEdit exports these wrong, they export it as GBAR (... seriously).
    if (ImgFrmt == VTF::ImageFormat::ARGB8888)
    {
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::ARGB8888*>(bmp->at(i));
            rval.push_back(QColor(pix->A, pix->R, pix->G, pix->B));
        }

        BitmapImage::Free(bmp);
        channels.push_back(ChannelLink("A", Channel::A));
        channels.push_back(ChannelLink("R", Channel::R));
        channels.push_back(ChannelLink("G", Channel::G));
        channels.push_back(ChannelLink("B", Channel::B));

        return rval;
    }

    // BGRA8888 Images
    if (ImgFrmt == VTF::ImageFormat::BGRA8888)
    {
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::BGRA8888*>(bmp->at(i));
            rval.push_back(QColor(pix->B, pix->G, pix->R, pix->A));
        }

        BitmapImage::Free(bmp);
        channels.push_back(ChannelLink("B", Channel::B));
        channels.push_back(ChannelLink("G", Channel::G));
        channels.push_back(ChannelLink("R", Channel::R));
        channels.push_back(ChannelLink("A", Channel::A));

        return rval;
    }

    // BGRX8888 Images
    if (ImgFrmt == VTF::ImageFormat::BGRX8888)
    {
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::BGRX8888*>(bmp->at(i));
            rval.push_back(QColor(pix->B, pix->G, pix->R, pix->X));
        }

        BitmapImage::Free(bmp);
        channels.push_back(ChannelLink("B", Channel::B));
        channels.push_back(ChannelLink("G", Channel::G));
        channels.push_back(ChannelLink("R", Channel::R));
        channels.push_back(ChannelLink("X", Channel::NONE));

        return rval;
    }

    // BGRX5551 Images
    if (ImgFrmt == VTF::ImageFormat::BGRX5551)
    {
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::BGRX5551*>(bmp->at(i));
            rval.push_back(QColor(
                           xvtf::Tools::LUT::LUT5[pix->B],
                           xvtf::Tools::LUT::LUT5[pix->G],
                           xvtf::Tools::LUT::LUT5[pix->R],
                           pix->X == 1 ? 255 : 0));
        }

        BitmapImage::Free(bmp);
        channels.push_back(ChannelLink("B", Channel::B));
        channels.push_back(ChannelLink("G", Channel::G));
        channels.push_back(ChannelLink("R", Channel::R));
        channels.push_back(ChannelLink("X", Channel::NONE));

        return rval;
    }

    // BGR565 Images
    if (ImgFrmt == VTF::ImageFormat::BGR565)
    {
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::BGR565*>(bmp->at(i));
            rval.push_back(QColor(
                           xvtf::Tools::LUT::LUT5[pix->B],
                           xvtf::Tools::LUT::LUT6[pix->G],
                           xvtf::Tools::LUT::LUT5[pix->R]));
        }

        BitmapImage::Free(bmp);
        channels.push_back(ChannelLink("B", Channel::B));
        channels.push_back(ChannelLink("G", Channel::G));
        channels.push_back(ChannelLink("R", Channel::R));

        return rval;
    }

    // BGRA4444 Images
    if (ImgFrmt == VTF::ImageFormat::BGRA4444)
    {
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::BGRA4444*>(bmp->at(i));
            rval.push_back(QColor(
                           xvtf::Tools::LUT::LUT4[pix->B],
                           xvtf::Tools::LUT::LUT4[pix->G],
                           xvtf::Tools::LUT::LUT4[pix->R],
                           xvtf::Tools::LUT::LUT4[pix->A]));
        }

        BitmapImage::Free(bmp);
        channels.push_back(ChannelLink("B", Channel::B));
        channels.push_back(ChannelLink("G", Channel::G));
        channels.push_back(ChannelLink("R", Channel::R));
        channels.push_back(ChannelLink("A", Channel::A));

        return rval;
    }

    // BGRA5551 Images
    if (ImgFrmt == VTF::ImageFormat::BGRA5551)
    {
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::BGRA5551*>(bmp->at(i));
            rval.push_back(QColor(
                           xvtf::Tools::LUT::LUT5[pix->B],
                           xvtf::Tools::LUT::LUT5[pix->G],
                           xvtf::Tools::LUT::LUT5[pix->R],
                           pix->A == 1 ? 255 : 0));
        }

        BitmapImage::Free(bmp);
        channels.push_back(ChannelLink("B", Channel::B));
        channels.push_back(ChannelLink("G", Channel::G));
        channels.push_back(ChannelLink("R", Channel::R));
        channels.push_back(ChannelLink("A", Channel::A));

        return rval;
    }

    // UV88 (Du/Dv) Images
    if (ImgFrmt == VTF::ImageFormat::UV88)
    {
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::UV88*>(bmp->at(i));
            rval.push_back(QColor(pix->U, pix->V, 0));
        }

        BitmapImage::Free(bmp);
        channels.push_back(ChannelLink("U", Channel::R));
        channels.push_back(ChannelLink("V", Channel::G));

        return rval;
    }

    // UVWQ8888/UVLX8888 (Normal Map) Images
    if (ImgFrmt == VTF::ImageFormat::UVWQ8888 || ImgFrmt == VTF::ImageFormat::UVLX8888)
    {
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::UVWQ8888*>(bmp->at(i));
            rval.push_back(QColor(pix->U, pix->V, pix->W, pix->Q));
        }

        BitmapImage::Free(bmp);
        channels.push_back(ChannelLink("U", Channel::R));
        channels.push_back(ChannelLink("V", Channel::G));

        if (ImgFrmt == VTF::ImageFormat::UVWQ8888)
        {
            channels.push_back(ChannelLink("W", Channel::B));
            channels.push_back(ChannelLink("Q", Channel::A));
        }
        else
        {
            channels.push_back(ChannelLink("L", Channel::B));
            channels.push_back(ChannelLink("X", Channel::NONE));
        }

        return rval;
    }

    // A8 (Alpha) Images
    if (ImgFrmt == VTF::ImageFormat::A8)
    {
        for (uint i = 0; i < pixCount; ++i)
        {
            auto pix = static_cast<PixelFormats::A8*>(bmp->at(i));
            rval.push_back(QColor(pix->A, 0, 0));
        }

        BitmapImage::Free(bmp);
        std::vector<Channel> ch = { Channel::R, Channel::G, Channel::B };
        channels.push_back(ChannelLink("A", ch));

        return rval;
    }

    // TODO
    // RGBA16161616 (Integer HDR) Images
    // if (ImgFrmt == VTF::ImageFormat::RGBA16161616)
    // {
    //     uint64_t* out = new uint64_t[pixCount];
    //     for(uint i = 0; i < pixCount; ++i)
    //     {
    //         out[i] = *static_cast<uint64_t*>(bmp->at(i));
    //     }
    //
    //     BitmapImage::Free(bmp);
    //
    //     return new QImage(reinterpret_cast<uchar*>(out), static_cast<int>(res.Width), static_cast<int>(res.Height), QImage::Format_RGBA64);
    // }

    // TODO: Error Handling !!!a

    return rval;
}
