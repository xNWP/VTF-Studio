#ifndef IMAGETOOLS_H
#define IMAGETOOLS_H

#include "xVTF/vtf_file.h"

#include "channelselector.h"

#include <QImage>

#include <vector>

namespace VTFStudio
{
    namespace ImageTools
    {
        //----------------------------------------------------------------------------------------------------
        /// Creates a QImage from an existing VTFFile.
        /// @param[in] vtf          The VTFFile pointer.
        /// @param[out] channels    A vector with the channel mappings applied.
        /// @param[out] error       Contains the error (if one occured).
        /// @param[in] MipLevel		The mip to load, 0 being the largest.
        /// @param[in] Frame		The frame to load, 0 being the first frame.
        /// @param[in] Face			The face to load (if the image contains any), 0 being the first face.
        /// @param[in] zLevel		The z-depth/slice to load (if any), 0 being the first slice.
        /// @return QColor*         A vector containing all the pixels.
        //----------------------------------------------------------------------------------------------------
        std::vector<QColor> CreateImageFromVTF(xvtf::Bitmap::VTFFile* vtf, std::vector<ChannelLink>& channels, uint * const & error = nullptr, const unsigned int MipLevel = 0,
                                   const unsigned int Frame = 0, const unsigned int Face = 0,
                                   const unsigned int zLevel = 0);
    }
}

#endif // IMAGETOOLS_H
