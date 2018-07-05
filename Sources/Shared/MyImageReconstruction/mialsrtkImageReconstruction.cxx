
/*=========================================================================

Program: Performs slice to volume registration and Scattered Data Interpolation
Language: C++
Date: $Date: 2012-28-12 14:00:00 +0100 (Fri, 28 Dec 2012) $
Version: $Revision: 1 $
Author: $Sebastien Tourbier$

Copyright (c) 2017 Medical Image Analysis Laboratory (MIAL), Lausanne
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

==========================================================================*/


#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

/* Standard includes */
//#include <tclap/CmdLine.h>
#include "stdio.h"

/* Itk includes */
#include "itkNormalizedCorrelationImageToImageMetric.h"
#include "itkMutualInformationImageToImageMetric.h"
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

/*Btk includes*/
//#include "btkSliceBySliceTransform.h"
//#include "btkEulerSliceBySliceTransform.h"
//#include "btkSliceBySliceTransformBase.h"
//#include "btkLowToHighImageResolutionMethod.h"


//#include "btkImageIntersectionCalculator.h"

/* mialsrtk includes */
//#include "mialsrtkLowToHighImageResolutionMethod.h"

#include "mialsrtkVersorSliceBySliceTransform.h"
#include "mialsrtkSliceBySliceTransformBase.h"
#include "mialsrtkLowToHighImageResolutionMethod.h"

#include "mialsrtkImageIntersectionCalculator.h"

#include "mialsrtkResampleImageByInjectionFilter.h"
#include "mialsrtkSliceBySliceRigidRegistration.h"
#include "mialsrtkBSplineInterpolateImageFunction.h"

#include "mialsrtkImageReconstruction.h"


mialImageReconstruction::mialImageReconstruction(std::vector< std::string > _inputFile, const char* const _outputFile, std::vector< std::string > _maskFile, const char* _refFile, std::vector< std::string > _transformout,
	std::vector< std::string > _resampled, const char* const _combinedMasks, unsigned int _itMax, double _epsilon, double _margin, bool _rigid3D, bool _noreg, bool _debluring, bool _boxSwitch,
	bool _maskSwitch, bool _allSwitch, unsigned int _ImageBaseOfRecon, const char* const _MetricToUse, unsigned int _m_Iterations, double _m_GradientMagnitudeTolerance, double _m_MinStepLength,
	double _m_MaxStepLength, double _m_RelaxationFactor, float _resampleKernelMultiplicator)
{

  inputFile = _inputFile;
  outputFile = _outputFile;
  maskFile = _maskFile;
  refFile = _refFile;
  transformout = _transformout;
  combinedMask = _combinedMasks;
  resampled = _resampled;
  itMax = _itMax;
  epsilon = _epsilon;
  margin = _margin;
  rigid3D = _rigid3D;
  noreg = _noreg;
  debluring = _debluring;
  boxSwitch = _boxSwitch;
  maskSwitch = _maskSwitch;
  allSwitch = _allSwitch;
  ImageBaseOfRecon = _ImageBaseOfRecon;
  MetricToUse = _MetricToUse;
  resampleKernelMultiplicator = _resampleKernelMultiplicator;

  m_Iterations = _m_Iterations;
  m_GradientMagnitudeTolerance = _m_GradientMagnitudeTolerance;
  m_MinStepLength = _m_MinStepLength;
  m_MaxStepLength = _m_MaxStepLength;
  m_RelaxationFactor = _m_RelaxationFactor;

}


mialImageReconstruction::~mialImageReconstruction()
{


}


bool mialImageReconstruction::runImageReconstruction()
{

  // typedefs

  const    unsigned int    Dimension = 3;
  typedef  float           PixelType;

  typedef itk::Image< PixelType, Dimension >  ImageType;
  typedef ImageType::Pointer                  ImagePointer;

  typedef ImageType::RegionType               RegionType;
  typedef std::vector< RegionType >           RegionArrayType;

  typedef itk::ImageFileReader< ImageType  >  ImageReaderType;

  typedef itk::Image< unsigned char, Dimension >    ImageMaskType;
  typedef ImageMaskType::Pointer                    ImageMaskPointer;

  typedef itk::ImageFileReader< ImageMaskType >     MaskReaderType;
  typedef itk::ImageMaskSpatialObject< Dimension >  MaskType;
  typedef MaskType::Pointer  MaskPointer;



  /* Registration type required in case of slice by slice transformations
  A rigid transformation is employed because there is not distortions like
  in diffusion imaging. We have performed a comparison of accuracy between
  both types of transformations. */
  typedef mialsrtk::SliceBySliceRigidRegistration<ImageType> RegistrationType;
  typedef RegistrationType::Pointer RegistrationPointer;

  // Registration type required in case of 3D affine trasforms
  typedef mialsrtk::RigidRegistration<ImageType> Rigid3DRegistrationType;

  typedef Rigid3DRegistrationType::Pointer Rigid3DRegistrationPointer;

  // Slice by slice transform definition (typically for in utero reconstructions)
  typedef mialsrtk::SliceBySliceTransformBase< double, Dimension > TransformBaseType;
  typedef mialsrtk::SliceBySliceTransform< double, Dimension > TransformType;
  typedef TransformType::Pointer                          TransformPointer;



  // Rigid 3D transform definition (typically for reconstructions in adults)
  //typedef itk::Euler3DTransform< double > Rigid3DTransformType;
  typedef itk::VersorRigid3DTransform< double > Rigid3DTransformType;
  typedef Rigid3DTransformType::Pointer   Rigid3DTransformPointer;

  // This filter does a rigid registration over all the LR images and compute the average image in HR space
  //typedef mialsrtk::LowToHighImageResolutionMethod<ImageType,Rigid3DTransformType > LowToHighResFilterType;
  typedef mialsrtk::LowToHighImageResolutionMethod<ImageType,Rigid3DTransformType > LowToHighResFilterType;
  LowToHighResFilterType::Pointer lowToHighResFilter = LowToHighResFilterType::New();

  // Resampler type required in case of a slice by slice transform
  typedef mialsrtk::ResampleImageByInjectionFilter< ImageType, ImageType
                                               >  ResamplerType;

  typedef itk::MattesMutualInformationImageToImageMetric< ImageType,
                                                        ImageType > MIMetricType;

  typedef itk::NormalizedCorrelationImageToImageMetric< ImageType,
                                                        ImageType > NCMetricType;

  typedef mialsrtk::ImageIntersectionCalculator<ImageType> IntersectionCalculatorType;
  IntersectionCalculatorType::Pointer intersectionCalculator = IntersectionCalculatorType::New();

  typedef itk::CastImageFilter<ImageType,ImageMaskType> CasterType;
  typedef itk::ImageDuplicator<ImageType> DuplicatorType;


  // Filter setup
  unsigned int numberOfImages = inputFile.size();
  std::vector< ImagePointer >         images(numberOfImages);
  std::vector< ImageMaskPointer >     imageMasks(numberOfImages);

  std::vector< TransformPointer >     transforms(numberOfImages);
  std::vector< Rigid3DTransformPointer >     rigid3DTransforms(numberOfImages);

  std::vector< RegistrationPointer >  registration(numberOfImages);
  std::vector< Rigid3DRegistrationPointer >  rigid3DRegistration(numberOfImages);

  std::vector< MaskPointer >          masks(numberOfImages);
  std::vector< RegionType >           rois(numberOfImages);

  ImagePointer hrImage;
  ImagePointer hrImageOld;
  ImagePointer hrImageIni;
  ImagePointer hrRefImage;

  lowToHighResFilter -> SetNumberOfImages(numberOfImages);
  lowToHighResFilter -> SetTargetImage( ImageBaseOfRecon );
  lowToHighResFilter -> SetMargin( margin );

  if (noreg)
  {
    lowToHighResFilter -> SetIterations( 0 );
    itMax = 1;
  }
  bool computeRefImage = true;

  if(strncmp(refFile, "", 4) != 0)
  {
      //std::cout<<"Reading the reference image : "<<refImage<<"\n";
      ImageReaderType::Pointer imageReader = ImageReaderType::New();
      imageReader -> SetFileName(refFile);
      try
      {
      imageReader -> Update();
      }
      catch (itk::ExceptionObject &err)
	{
		throw "error opening refImage";
		return false;
	}
      hrImageIni = imageReader -> GetOutput();
      hrRefImage = imageReader -> GetOutput();
      computeRefImage = false;
  }

  for (unsigned int i=0; i<numberOfImages; i++)
  {
    //std::cout<<"Reading image : "<<input[i].c_str()<<"\n";
    ImageReaderType::Pointer imageReader = ImageReaderType::New();
    imageReader -> SetFileName( inputFile[i].c_str() );
    try{
    imageReader -> Update();
    }
    catch (itk::ExceptionObject &err)
	{
		throw "error opening inputImage";  // +inputFile[i].c_str();
		return false;
	}
    images[i] = imageReader -> GetOutput();

    lowToHighResFilter -> SetImageArray(i, images[i] );

    if ( boxSwitch)
    {
      intersectionCalculator -> AddImage( images[i] );
    }

  }

  if ( boxSwitch )
  {
    intersectionCalculator -> Update();
  }

  // Set roi according to the provided arguments

  for (unsigned int i=0; i<numberOfImages; i++)
  {
    // if a mask of the LR image is provided ...
    if (maskSwitch)
    {
      MaskReaderType::Pointer maskReader = MaskReaderType::New();
      maskReader -> SetFileName( maskFile[i].c_str() );
      try
      {
      maskReader -> Update();
      }
      catch (itk::ExceptionObject &err)
	  {
		throw "error opening maskImage"; // +maskFile[i].c_str();
		return false;
      }

      imageMasks[i] = maskReader -> GetOutput();
      lowToHighResFilter -> SetImageMaskArray( i, imageMasks[i] );

      masks[i] = MaskType::New();
      masks[i] -> SetImage( imageMasks[i] );
      rois[i] = masks[i] -> GetAxisAlignedBoundingBoxRegion();

      lowToHighResFilter -> SetRegionArray( i, rois[i] );


    }
    // estimate the intersection of the ROIs (-> brain can be cropped !)
    else if (boxSwitch)
      {
        imageMasks[i] = intersectionCalculator -> GetImageMask(i);
        lowToHighResFilter -> SetImageMaskArray( i, imageMasks[i] );

        masks[i] = MaskType::New();
        masks[i] -> SetImage( imageMasks[i] );
        rois[i] = masks[i] -> GetAxisAlignedBoundingBoxRegion();
        lowToHighResFilter -> SetRegionArray( i, rois[i] );


      }
      // use the entire image (longer computation)
      else if (allSwitch)
        {
          DuplicatorType::Pointer duplicator = DuplicatorType::New();
          duplicator -> SetInputImage( images[i] );
          duplicator -> Update();

          CasterType::Pointer caster = CasterType::New();
          caster -> SetInput( duplicator -> GetOutput() );
          caster -> Update();

          imageMasks[i] = caster -> GetOutput();
          imageMasks[i] -> FillBuffer(1);
          lowToHighResFilter -> SetImageMaskArray( i, imageMasks[i] );

          masks[i] = MaskType::New();
          masks[i] -> SetImage( imageMasks[i] );
          rois[i] = masks[i] -> GetAxisAlignedBoundingBoxRegion();
          lowToHighResFilter -> SetRegionArray( i, rois[i] );

        }
  }

  //std::cout<<"Start rigid registration on the desired target image (#0 by default)\n";
  try
    {
       lowToHighResFilter->StartRegistration();

    }
  catch( itk::ExceptionObject & err )
    {
        //std::cerr << "ExceptionObject caught !" << std::endl;
        //std::cerr << err << std::endl;
	    throw "error lowToHighResFilter start registration";
        return false;
    }


  // Write combined image mask

  typedef itk::ImageFileWriter< ImageMaskType >  MaskWriterType;

  if (strncmp(combinedMask, "", 4) != 0)
  {
    try
    {
      //std::cout << "Write image mask combination..." <<std::endl << lowToHighResFilter -> GetImageMaskCombination() << std::endl;
      MaskWriterType::Pointer maskWriter =  MaskWriterType::New();
      maskWriter -> SetFileName( combinedMask );
      maskWriter -> SetInput( lowToHighResFilter -> GetImageMaskCombination() );
      maskWriter -> Update();
    }
    catch( itk::ExceptionObject & err )
    {
      //std::cerr << "ExceptionObject caught !" << std::endl;
      //std::cerr << err << std::endl;
		throw "error writing combinedMask";
    }
  }

  // Write resampled LR images in HR space
  if ( resampled.size() > 0 )
  {
    for (unsigned int i=0; i<numberOfImages; i++)
    {
      lowToHighResFilter -> WriteResampledImages( i, resampled[i].c_str() );
    }
  }

  // Image registration performed slice by slice or affine 3D according to
  // the user selection

  if(computeRefImage)
  {
    //std::cout << "Get global rigid HR image" << std::endl;
    hrImageIni = lowToHighResFilter->GetHighResolutionImage();
    hrRefImage = lowToHighResFilter->GetHighResolutionImage();
  }

  for (unsigned int i=0; i<numberOfImages; i++)
  {
    if (rigid3D)
    {
      rigid3DTransforms[i] = lowToHighResFilter -> GetTransformArray(i);
    }else
    {
      transforms[i] = TransformType::New();
      transforms[i] -> SetImage( images[i] );
      transforms[i] -> Initialize( lowToHighResFilter -> GetInverseTransformArray(i) );
    }
  }

  int im = numberOfImages;

  //Identify and exclude bad slices based on SNR
  /** Check all image slices and compute and store the SNR metric for all slices */

  //std::cout << "Checking image slice quality... " << std::endl;
  /** Stores the starting slice index of jth image in the image slice index vector */
  //m_ImageSliceIndex.push_back( m_sliceSNRmetric.size() );

  /** Anisotropic diffusion Gaussian filtering for computing the SNR measure */
  // typedef itk::CurvatureAnisotropicDiffusionImageFilter< ImageType, ImageType >  ADGaussianFilterType;

  // typedef itk::SubtractImageFilter< InputImageType, ImageType, ImageType > SubtractFilterType;
  // typedef itk::ResampleImageFilter< ImageType, ImageType > ResampleFilterType;

  // ADGaussianFilter = ADGaussianFilterType::New();
  // ADGaussianFilter->SetInput( images[i] );
  // int          numberOfIterations = 3;
  // double       conductance = 3.0;
  // double       timeStep = 0.0125;
  // ADGaussianFilter->SetNumberOfIterations( numberOfIterations );
  // ADGaussianFilter->SetTimeStep( timeStep );
  // ADGaussianFilter->SetConductanceParameter( conductance );
  // ADGaussianFilter->Update();


  // Slice-to-volume registration

  float previousMetric = 0.0;
  float currentMetric = 0.0;

  for(unsigned int it=1; it <= itMax; it++)
  {
    std::cout << "Iteration " << it << std::endl;

    // Start registration

    #pragma omp parallel for private(im) schedule(dynamic)

    for (im=0; im<numberOfImages; im++)
    {
      //std::cout << "Registering image " << im << " ... "; std::cout.flush();

      if (rigid3D)
      {
        rigid3DRegistration[im] = Rigid3DRegistrationType::New();
        rigid3DRegistration[im] -> SetFixedImage( images[im] );
        rigid3DRegistration[im] -> SetMovingImage( hrRefImage );
		//rigid3DRegistration[im] -> SetCoregistrationMethod(MetricToUse);
        rigid3DRegistration[im] -> SetFixedImageMask( imageMasks[im] );
        rigid3DRegistration[im] -> SetTransform( rigid3DTransforms[im] );

        if (noreg)
          rigid3DRegistration[im] -> SetIterations( 0 );

        try
          {
          rigid3DRegistration[im]->Update();
          }
        catch( itk::ExceptionObject & err )
          {
          //std::cerr << "ExceptionObject caught !" << std::endl;
          //std::cerr << err << std::endl;
  //        return EXIT_FAILURE;
			throw "error 3D registration";
            //return false;
          }

        rigid3DTransforms[im] = rigid3DRegistration[im] -> GetTransform();
      } else
        {
          //slice by slice registration
          registration[im] = RegistrationType::New();
          registration[im] -> SetFixedImage( images[im] );
          registration[im] -> SetMovingImage( hrRefImage );
          registration[im] -> SetImageMask( imageMasks[im] );
          registration[im] -> SetTransform( transforms[im] );
          registration[im] -> SetIterations( m_Iterations );
		  registration[im] -> SetCoregistrationMethod(std::string(MetricToUse));
          registration[im] -> SetGradientMagnitudeTolerance( m_GradientMagnitudeTolerance );
          registration[im] -> SetMinStepLength( m_MinStepLength );
          registration[im] -> SetMaxStepLength( m_MaxStepLength );
          registration[im] -> SetRelaxationFactor( m_RelaxationFactor );

          //new

          if (noreg)
            registration[im] -> SetIterations( 0 );

          try
            {
            registration[im] -> StartRegistration();
            }
          catch( itk::ExceptionObject & err )
            {
            throw "error slice by slice registration";
            //return false;
            //std::cerr << err << std::endl;
    //        return EXIT_FAILURE;
            }

          transforms[im] = static_cast< TransformType* >(registration[im] -> GetTransform());
        }

      //std::cout << "done. "; std::cout.flush();

    }

    //std::cout << std::endl; std::cout.flush();
	std::cout << "registration done" << std::endl;
    // Inject images

    //std::cout << "Injecting images ... "; std::cout.flush();

	//update mask low resolution en fonction des outliers des coregistrations


    ResamplerType::Pointer resampler = ResamplerType::New();

    for (unsigned int i=0; i<numberOfImages; i++)
    {
      resampler -> AddInput( images[i] );
      resampler -> AddMask( imageMasks[i] );
      resampler -> AddRegion( rois[i] );

      if (rigid3D)
      {
        transforms[i] = TransformType::New();
        transforms[i] -> SetImage( images[i] );
        transforms[i] -> Initialize( rigid3DTransforms[i] );
      }

      resampler -> SetTransform(i, transforms[i]) ;
    }

    resampler -> UseReferenceImageOn();
    resampler -> SetReferenceImage( hrRefImage );
    resampler -> SetReferenceImageMask(lowToHighResFilter -> GetImageMaskCombination());

    resampler -> SetUseDebluringKernel( debluring );
	resampler -> SetkernelRadiusMultiplicator(resampleKernelMultiplicator);

    try
	{
	 resampler -> Update();
	}
	catch (itk::ExceptionObject & err)
	{
	 throw "error resampling";
	 return false;

	}
    if (it == 1)
      hrImageOld = hrImageIni;
    else
      hrImageOld = hrImage;

    hrImage = resampler -> GetOutput();

    //std::cout << "done. " << std::endl; std::cout.flush();

    // compute error

    typedef itk::Euler3DTransform< double > EulerTransformType;
    EulerTransformType::Pointer identity = EulerTransformType::New();
    identity -> SetIdentity();

    //typedef itk::LinearInterpolateImageFunction<
    //                                  ImageType,
    //                                  double>     InterpolatorType;
    typedef itk::BSplineInterpolateImageFunction<
                                      ImageType,
                                      double>     InterpolatorType;

    InterpolatorType::Pointer interpolator = InterpolatorType::New();

    
	
	//lui j'y touche pas, meme si on passe en multi modal a terme ?
	//la c'est juste un metric "sans rapport" avec la coregistration ? on le change aussi entre NC et MI ou on s'en fout ?
	//on pourrait aussi bien mettre le MSE comme metric ?
	NCMetricType::Pointer nc = NCMetricType::New();
    nc -> SetFixedImage(  hrImageOld );
    nc -> SetMovingImage( hrImage );
    nc -> SetFixedImageRegion( hrImageOld -> GetLargestPossibleRegion() );
    nc -> SetTransform( identity );
    nc -> SetInterpolator( interpolator );
    nc -> Initialize();

    previousMetric = currentMetric;
    currentMetric = - nc -> GetValue( identity -> GetParameters() );
    //std::cout<<"previousMetric: "<<previousMetric<<", currentMetric: "<<currentMetric<<"\n";
    double delta = 0.0;

    if (it >= 2)
      delta = (currentMetric - previousMetric) / previousMetric;
    else
      delta = 1;

	if (delta < epsilon)
	{
        //we store the different outlier values
		std::cout << "TO DO outlier registration slices" << std::endl;
		break;
	}

	if (it == itMax)
	{
		//we store the different outlier values
		std::cout << "ITMAX" << std::endl;
	}

  }

  std::cout << "write datas" << std::endl;

  // Write HR image

  typedef itk::ImageFileWriter< ImageType >  WriterType;

  WriterType::Pointer writer =  WriterType::New();
  writer-> SetFileName(outputFile);
  writer-> SetInput( hrImage );
  try
  {
  writer-> Update();
  }
  catch ( itk::ExceptionObject & err )
  {
  throw "error writing image output";
  return false;
  }

  // Write transforms

  typedef itk::TransformFileWriter TransformWriterType;

  if (transformout.size() > 0 )
  {
    for (unsigned int i=0; i<numberOfImages; i++)
    {
      TransformWriterType::Pointer transformWriter = TransformWriterType::New();

      if (rigid3D)
      {
        transformWriter -> SetInput( rigid3DTransforms[i] );
      } else
        {
          transformWriter -> SetInput( transforms[i] );
        }

      transformWriter -> SetFileName ( transformout[i].c_str() );


      try
      {
        //std::cout << "Writing " << transform[i].c_str() << " ... " ; std::cout.flush();
        transformWriter -> Update();
        //std::cout << " done! " << std::endl;
      }
      catch ( itk::ExceptionObject & excp )
      {
        //std::cerr << "Error while saving transform" << std::endl;
        //std::cerr << excp << std::endl;
        //std::cout << "[FAILED]" << std::endl;
        throw "error writing transform out";
        return false;
      }

    }
  }

  return true;

}

