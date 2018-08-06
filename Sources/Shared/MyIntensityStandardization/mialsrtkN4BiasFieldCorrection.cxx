/*=========================================================================

Program: N4 MRI Bias Field Correction
Language: C++
Date: $Date: 2012-28-12 14:00:00 +0100 (Fri, 28 Dec 2012) $
Version: $Revision: 1 $
Author: $Sebastien Tourbier$

Copyright (c) 2017 Medical Image Analysis Laboratory (MIAL), Lausanne
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

==========================================================================*/

#include "itkImage.h"
#include "itkShrinkImageFilter.h"
#include <itkN4BiasFieldCorrectionImageFilter.h>

#include <iostream>
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkEuler3DTransform.h"

#include "itkResampleImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"

#include "mialsrtkN4BiasFieldCorrection.h"


mialsrtkN4BiasFieldCorrection::mialsrtkN4BiasFieldCorrection(const char* const _inputFile, const char* const _maskFile, const char* const _outputFile, const char* const _outputBiasFile)
{
	inputFile = _inputFile;
	maskFile = _maskFile;
	outputFile = _outputFile;
	outputBiasFile = _outputBiasFile;
	ShrinkFactor = 4;
}

mialsrtkN4BiasFieldCorrection::~mialsrtkN4BiasFieldCorrection()
{

}

bool mialsrtkN4BiasFieldCorrection::runN4BiasFieldCorrection()
{
 
  const unsigned int dimension = 3; 
  
  typedef float InputPixelType;
  typedef float OutputPixelType;
  typedef float MaskPixelType;

  typedef itk::Image<InputPixelType, dimension> InputImageType;
  typedef itk::Image<OutputPixelType, dimension> OutputImageType;
  typedef itk::Image<MaskPixelType, dimension> MaskType;
    
  typedef itk::ImageFileReader<InputImageType> ReaderType;
  typedef itk::ImageFileWriter<OutputImageType> WriterType;
  typedef itk::ImageFileReader<MaskType> MaskReaderType;


  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(inputFile);
  try
  {
  reader->Update();
  }
  catch (itk::ExceptionObject &err)
  {
	  throw "error opening inputImage"; 
	  return false;
  }

  InputImageType::Pointer inputImage = reader->GetOutput();


  MaskReaderType::Pointer maskReader = MaskReaderType::New();
  maskReader->SetFileName(maskFile);
  try
  {
  maskReader->Update();
  }
  catch (itk::ExceptionObject &err)
  {
	  throw "error opening maskImage";
	  return false;
  }

  typedef itk::ResampleImageFilter<MaskType,MaskType> ResampleImageMaskFilterType;
  typedef itk::NearestNeighborInterpolateImageFunction<MaskType> NNInterpolatorType;

  ResampleImageMaskFilterType::Pointer maskUpsampler = ResampleImageMaskFilterType::New();
  NNInterpolatorType::Pointer nnInterpolator = NNInterpolatorType::New();

  typedef  itk::Euler3DTransform<double> Euler3DTransformType;
  Euler3DTransformType::Pointer idTransform = Euler3DTransformType::New();
  idTransform->SetIdentity();

  maskUpsampler -> SetInterpolator(nnInterpolator);
  maskUpsampler -> SetInput(maskReader->GetOutput());
  maskUpsampler -> SetTransform(idTransform);
  maskUpsampler -> SetOutputParametersFromImage(inputImage.GetPointer());
  maskUpsampler -> SetOutputSpacing(inputImage->GetSpacing());
  maskUpsampler -> SetSize(inputImage->GetLargestPossibleRegion().GetSize());

  maskUpsampler -> Update();

  MaskType::Pointer maskImage = maskUpsampler->GetOutput();

  typedef itk::ShrinkImageFilter<InputImageType, InputImageType> ShrinkerType;
  ShrinkerType::Pointer shrinker = ShrinkerType::New();
  
  shrinker->SetInput(inputImage);
  shrinker->SetShrinkFactors(ShrinkFactor);
  shrinker->Update();
  shrinker->UpdateLargestPossibleRegion();


  typedef itk::ShrinkImageFilter<MaskType, MaskType> MaskShrinkerType;
  MaskShrinkerType::Pointer maskShrinker = MaskShrinkerType::New();
  
  maskShrinker->SetInput(maskImage);
  maskShrinker->SetShrinkFactors(ShrinkFactor);
  maskShrinker->Update();
  maskShrinker->UpdateLargestPossibleRegion();

  typedef itk::N4BiasFieldCorrectionImageFilter<InputImageType,MaskType,InputImageType> CorrecterType;
  CorrecterType::Pointer correcter = CorrecterType::New();
  
  correcter->SetInput1(shrinker->GetOutput());
 
  correcter->SetMaskImage(maskShrinker->GetOutput());
  correcter->SetMaskLabel(1);

  unsigned int NumberOfFittingLevels = 3;
  unsigned int NumberOfHistogramBins = 200;

  //With B-spline grid res. = [1, 1, 1]
  CorrecterType::ArrayType NumberOfControlPoints(NumberOfFittingLevels);
  NumberOfControlPoints[0] = NumberOfFittingLevels+1;
  NumberOfControlPoints[1] = NumberOfFittingLevels+1;
  NumberOfControlPoints[2] = NumberOfFittingLevels+1;

  CorrecterType::VariableSizeArrayType maximumNumberOfIterations(NumberOfFittingLevels); 
  maximumNumberOfIterations[0] = 50;
  maximumNumberOfIterations[1] = 40;
  maximumNumberOfIterations[2] = 30;

  float WienerFilterNoise = 0.01;
  float FullWidthAtHalfMaximum = 0.15;
  float ConvergenceThreshold = 0.0001;

  correcter->SetMaximumNumberOfIterations(maximumNumberOfIterations);
  correcter->SetNumberOfFittingLevels(NumberOfFittingLevels);
  correcter->SetNumberOfControlPoints(NumberOfControlPoints); 
  correcter->SetWienerFilterNoise(WienerFilterNoise);
  correcter->SetBiasFieldFullWidthAtHalfMaximum(FullWidthAtHalfMaximum);
  correcter->SetConvergenceThreshold(ConvergenceThreshold);
  correcter->SetNumberOfHistogramBins(NumberOfHistogramBins);
  correcter->Update();


  typedef CorrecterType::BiasFieldControlPointLatticeType PointType;
  typedef CorrecterType::ScalarImageType ScalarImageType;

  typedef itk::BSplineControlPointImageFilter<PointType, ScalarImageType> BSplinerType;
  BSplinerType::Pointer bspliner = BSplinerType::New();
  bspliner->SetInput( correcter->GetLogBiasFieldControlPointLattice() );
  bspliner->SetSplineOrder( correcter->GetSplineOrder() );
  bspliner->SetSize( inputImage->GetLargestPossibleRegion().GetSize() );
  bspliner->SetOrigin( inputImage->GetOrigin() );
  bspliner->SetDirection( inputImage->GetDirection() );
  bspliner->SetSpacing( inputImage->GetSpacing() );
  bspliner->Update();

  InputImageType::Pointer logField = InputImageType::New();
  logField->SetOrigin(bspliner->GetOutput()->GetOrigin());
  logField->SetSpacing(bspliner->GetOutput()->GetSpacing());
  logField->SetRegions(bspliner->GetOutput()->GetLargestPossibleRegion().GetSize());
  logField->SetDirection(bspliner->GetOutput()->GetDirection());
  logField->Allocate();

  itk::ImageRegionIterator<ScalarImageType> ItB(bspliner->GetOutput(),bspliner->GetOutput()->GetLargestPossibleRegion());
  itk::ImageRegionIterator<InputImageType> ItF(logField,logField->GetLargestPossibleRegion());
  
  for(ItB.GoToBegin(), ItF.GoToBegin(); !ItB.IsAtEnd(); ++ItB, ++ItF)
  {
    ItF.Set( ItB.Get()[0] );
  }



  typedef itk::ExpImageFilter<InputImageType, InputImageType>
  ExpFilterType;
  ExpFilterType::Pointer expFilter = ExpFilterType::New();
  expFilter->SetInput( logField );
  expFilter->Update();

  typedef itk::DivideImageFilter<InputImageType, InputImageType, InputImageType> DividerType;
  DividerType::Pointer divider = DividerType::New();
  divider->SetInput1( inputImage );
  divider->SetInput2( expFilter->GetOutput() );
  divider->Update();



  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outputFile);
  writer->SetInput(divider->GetOutput());
  try
  {
  writer->Update();
  }
  catch (itk::ExceptionObject &err)
  {
	  throw "error writing outputImage";
	  return false;
  }
  WriterType::Pointer fieldWriter = WriterType::New();
  fieldWriter->SetFileName(outputBiasFile);
  fieldWriter->SetInput(logField);
  try
  {
  fieldWriter->Update();
  }
  catch (itk::ExceptionObject &err)
  {
	  throw "error writing output BiasField";
	  return false;
  }

  return true;
}



