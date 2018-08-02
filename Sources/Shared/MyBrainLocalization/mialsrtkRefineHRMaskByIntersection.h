#ifndef mialsrtkRefineHRMaskByIntersection_H
#define mialsrtkRefineHRMaskByIntersection_H


#include <sstream>  

#include <iostream>
#include <fstream> 
#include <string>
#include <stdlib.h> 

/* Itk includes */
#include "itkVersorRigid3DTransform.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageMaskSpatialObject.h"
#include "itkTransformFileReader.h"
#include "itkTransformFactory.h"
#include "itkCastImageFilter.h"

#include "itkPermuteAxesImageFilter.h"
#include "itkFlipImageFilter.h"
#include "itkOrientImageFilter.h"  

/*Mialsrtktk includes*/
#include "mialsrtkSliceBySliceTransform.h"

#include "itkResampleImageFilter.h"

#include "itkExtractImageFilter.h"
#include "itkAddImageFilter.h"
#include "itkMultiplyImageFilter.h"

#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"

#include "itkImageDuplicator.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageLinearConstIteratorWithIndex.h"

#include "itkBinaryThresholdImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRelabelComponentImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryMorphologicalClosingImageFilter.h"
#include "itkBinaryMorphologicalOpeningImageFilter.h"

#include "itkBinaryDilateImageFilter.h"

// classes help the MRF/Gibbs filter to segment the image
#include "itkMRFImageFilter.h"
#include "itkImageClassifierBase.h"
#include "itkDistanceToCentroidMembershipFunction.h"
#include "itkMinimumDecisionRule.h"
#include "itkComposeImageFilter.h"

#include "itkImageGaussianModelEstimator.h"
#include "itkMahalanobisDistanceMembershipFunction.h"

// image storage and I/O classes
#include "itkSize.h"
#include "itkVector.h"


#include "crlMSTAPLEImageFilter.h"

class mialsrtkRefineHRMaskByIntersection {

public:
	
	mialsrtkRefineHRMaskByIntersection(std::vector< std::string > _inputFiles, std::vector< std::string > _transformIn, std::vector< std::string > _maskFiles, const char* _outputHRFile, 
		std::vector< std::string > _outputLRFiles, const char* _referenceFile, int _radiusDilation, bool _useStaple);
	~mialsrtkRefineHRMaskByIntersection();

	std::vector< std::string > inputFiles;
	std::vector< std::string > transformIn;
	std::vector< std::string > maskFiles;
	const char* outputHRFile;
	std::vector< std::string > outputLRFiles;
	const char* referenceFile;
	int radiusDilation;
	bool useStaple;

	bool runRefineMask();
};

#endif mialsrtkRefineHRMaskByIntersection_H