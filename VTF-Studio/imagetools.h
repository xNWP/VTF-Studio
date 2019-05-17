#ifndef IMAGETOOLS_H
#define IMAGETOOLS_H

#include "xVTF/xVTFFile.h"

#include <QImage>

namespace VTFStudio
{
    namespace ImageTools
    {
        //----------------------------------------------------------------------------------------------------
        /// Creates a QImage from an existing VTFFile.
        /// @param[in] vtf          The VTFFile pointer.
        /// @param[in] MipLevel		The mip to load, 0 being the largest.
        /// @param[in] Frame		The frame to load, 0 being the first frame.
        /// @param[in] Face			The face to load (if the image contains any), 0 being the first face.
        /// @param[in] zLevel		The z-depth/slice to load (if any), 0 being the first slice.
        /// @return QImage*         The generated QImage pointer or nullptr if an error occured.
        //----------------------------------------------------------------------------------------------------
        QImage* CreateImageFromVTF(xvtf::Bitmap::VTFFile* vtf, const unsigned int MipLevel = 0,
                                   const unsigned int Frame = 0, const unsigned int Face = 0,
                                   const unsigned int zLevel = 0);
    }
}

#endif // IMAGETOOLS_H
