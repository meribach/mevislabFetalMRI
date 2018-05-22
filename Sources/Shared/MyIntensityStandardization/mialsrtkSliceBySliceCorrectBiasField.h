/*=========================================================================

Program: Independent slice by slice N4 bias field correction
Language: C++
Date: $Date: 2012-28-12 $
Version: $Revision: 1 $
Author: $Sebastien Tourbier$

Copyright (c) 2017 Medical Image Analysis Laboratory (MIAL), Lausanne
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

==========================================================================*/
#ifndef mialsrtkSliceBySliceCorrectBiasField_H
#define mialsrtkSliceBySliceCorrectBiasField_H

#include "itkImage.h"
#include "itkPoint.h"
#include "itkImageMaskSpatialObject.h"

#include "itkExpImageFilter.h"
#include "itkAddImageFilter.h"
#include "itkDivideImageFilter.h"
#include "itkStatisticsImageFilter.h"

#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkImageRegionIterator.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "float.h"


class SliceBySliceCorrectBiasField
{

public:

   //! default constructor
   SliceBySliceCorrectBiasField(const char* const _inputImageFile, const char* const _inputMaskFile, const char* const _inputBiasFieldFile, const char* const _outputImageFile);
   //! default destructor
   ~SliceBySliceCorrectBiasField();

   const char* inputImageFile;
   const char* inputMaskFile;
   const char* inputBiasFieldFile;
   const char* outputImageFile;

   bool applyBiasFieldCorrection();

};


#endif mialsrtkSliceBySliceCorrectBiasField_H
