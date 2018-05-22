#ifndef mialsrtkCorrectSliceIntensity_H
#define mialsrtkCorrectSliceIntensity_H

#include "itkImage.h"
#include "itkPoint.h"
#include "itkImageMaskSpatialObject.h"

#include "itkAddImageFilter.h"
#include "itkDivideImageFilter.h"
#include "itkStatisticsImageFilter.h"

#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"
#include "itkExtractImageFilter.h"
#include "itkImageRegionIterator.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkChangeInformationImageFilter.h"

#include "float.h"
#include "vnl_index_sort.h"
#include <algorithm>


class correctSliceIntensity {

    public:
        //three input parameters. inputfile maskfile et outputfile
        //only one constructor it doesn't make sens to not have the mask file. can't correct the whole volume, intensity too inhomogeneous outside the fetus
		//! default constructor
		correctSliceIntensity(const char* const _inputFile, const char* const _maskFile, const char* const _outputFile);
		//! default destructor
		~correctSliceIntensity();
		
		const char* inputFile;
		const char* outputFile;
		const char* maskFile;
		bool SliceIntensityCorrection();
};


#endif mialsrtkCorrectSliceIntensity_H