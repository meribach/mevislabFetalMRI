/*=========================================================================

Program: Independent slice by slice N4 MRI Bias Field Correction
Language: C++
Date: $Date: 2012-28-12 14:00:00 +0100 (Fri, 28 Dec 2012) $
Version: $Revision: 1 $
Author: $Sebastien Tourbier$

Copyright (c) 2017 Medical Image Analysis Laboratory (MIAL), Lausanne
This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

==========================================================================*/

#ifndef mialsrtkSliceBySliceN4BiasFieldCorrection_H
#define mialsrtkSliceBySliceN4BiasFieldCorrection_H

#include "itkImage.h"
#include "itkImageMaskSpatialObject.h"
#include "itkExtractImageFilter.h"
#include "itkShrinkImageFilter.h"
#include <itkN4BiasFieldCorrectionImageFilter.h>

#include "itkStatisticsImageFilter.h"
#include "itkAddImageFilter.h"

#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageLinearConstIteratorWithIndex.h"

#include <iostream>
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "float.h"

//#include "limits.h"

class SliceBySliceN4BiasFieldCorrection
{
public:

	//! default constructor
	SliceBySliceN4BiasFieldCorrection(const char* const _inputImageFile, const char* const _inputMaskFile, const char* const _outputImageFile, const char* const _outputBiasFile);

	//! default destructor
	~SliceBySliceN4BiasFieldCorrection();

	const char* inputImageFile;
	const char* inputMaskFile;
	const char* outputImageFile;
	const char* outputBiasFile;

	bool runSliceBySliceN4BiasFieldCorrection();
};

#endif mialSliceBySliceN4BiasFieldCorrection_H
