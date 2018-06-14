#ifndef mialImageReconstruction_H
#define mialImageReconstruction_H


#include "itkNormalizedCorrelationImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "itkEuler3DTransform.h"
#include "itkTransformFileWriter.h"
#include "itkImage.h"
#include "itkImageMaskSpatialObject.h"
#include "itkCastImageFilter.h"
#include "itkImageDuplicator.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"


#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"

#include "itkVersorRigid3DTransform.h"

#include "mialsrtkVersorSliceBySliceTransform.h"
#include "mialsrtkSliceBySliceTransformBase.h"
#include "mialsrtkLowToHighImageResolutionMethod.h"

#include "mialsrtkImageIntersectionCalculator.h"

#include "mialsrtkResampleImageByInjectionFilter.h"
#include "mialsrtkSliceBySliceRigidRegistration.h"
#include "mialsrtkBSplineInterpolateImageFunction.h"

class mialImageReconstruction {

public:

		//! default constructor
		mialImageReconstruction(std::vector< std::string > _inputFile, const char* const _outputFile, std::vector< std::string > _maskFile, const char* _refFile, std::vector< std::string > _transformout,
            std::vector< std::string > _resampled, const char* const _combinedMasks, unsigned int _itMax, double _epsilon, double _margin, bool _rigid3D, bool _noreg, bool _debluring, bool _boxSwitch,
            bool _maskSwitch, bool _allSwitch,unsigned int _ImageBaseOfRecon,const char* const _MetricToUse, unsigned int _m_Iterations, double _m_GradientMagnitudeTolerance, double _m_MinStepLength,
            double _m_MaxStepLength, double _m_RelaxationFactor);

		//! default destructor
		~mialImageReconstruction();

		std::vector< std::string > inputFile;
		const char* outputFile;
		std::vector< std::string >  maskFile;
		const char* refFile;
		std::vector< std::string > transformout;
		std::vector< std::string > resampled;
		const char* combinedMask;

        unsigned int itMax;
        double epsilon;
        double margin;
        bool rigid3D;
        bool noreg;
        bool debluring;

        bool boxSwitch;
        bool maskSwitch;
        bool allSwitch;
        unsigned int ImageBaseOfRecon;
        const char* MetricToUse;

        unsigned int m_Iterations;
        double m_GradientMagnitudeTolerance;
        double m_MinStepLength;
        double m_MaxStepLength;
        double m_RelaxationFactor;


		bool runImageReconstruction();


};

#endif mialImageReconstruction_H
