#ifndef mialsrtkCorrectBiasFieldWithMotionApplied_H
#define mialsrtkCorrectBiasFieldWithMotionApplied_H

#include <iostream>
#include <limits>       // std::numeric_limits


#include "itkImage.h"
#include "itkImageMaskSpatialObject.h"

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkExpImageFilter.h"
#include "itkDivideImageFilter.h"

#include "itkTransformFileReader.h"
#include "itkTransformFactory.h"
#include "mialsrtkSliceBySliceTransform.h"
#include "itkEuler3DTransform.h"
#include "itkVersorRigid3DTransform.h"

#include "vnl/vnl_matops.h"
#include "vnl/vnl_sparse_matrix.h"
#include "btkLinearInterpolateImageFunctionWithWeights.h"
#include "mialsrtkOrientedSpatialFunction.h"

#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIteratorWithIndex.h"

#include "itkResampleImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"

class mialsrtkCorrectBiasFieldWithMotionApplied {

  public:
	  mialsrtkCorrectBiasFieldWithMotionApplied(const char* _inputFileName, const char* _maskFileName, const char* _transformFileName, const char* _inBiasFieldFileName, const char* _outImageFileName, const char* _outBiasFieldFileName);
	  ~mialsrtkCorrectBiasFieldWithMotionApplied();

	  const char* inputFileName;
	  const char* maskFileName;
	  const char* transformFileName;
	  const char* inBiasFieldFileName;
	  const char* outImageFileName;
	  const char* outBiasFieldFileName;

	  bool RunBiasCorrectionWithMotionApplied();
	  bool boolExit;

};


#endif mialsrtkCorrectBiasFieldWithMotionApplied_H