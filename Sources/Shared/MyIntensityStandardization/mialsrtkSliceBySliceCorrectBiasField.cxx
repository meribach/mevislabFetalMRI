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

#include "mialsrtkSliceBySliceCorrectBiasField.h"


SliceBySliceCorrectBiasField::SliceBySliceCorrectBiasField(const char* const _inputImageFile, const char* const _inputMaskFile, const char* const _inputBiasFieldFile, const char* const _outputImageFile)
{
   inputImageFile = _inputImageFile;
   inputMaskFile = _inputMaskFile;
   inputBiasFieldFile = _inputBiasFieldFile;
   outputImageFile = _outputImageFile;
}

SliceBySliceCorrectBiasField::~SliceBySliceCorrectBiasField()
{
}

bool SliceBySliceCorrectBiasField::applyBiasFieldCorrection() // main( int argc, char * argv [] )
{

    const unsigned int dimension3D = 3;
    const unsigned int dimension2D = 2;


    typedef float InputPixelType;
    typedef float OutputPixelType;
    typedef unsigned char MaskPixelType;

    typedef itk::Image<InputPixelType, dimension3D> InputImageType;
    typedef itk::Image<OutputPixelType, dimension3D> OutputImageType;
    typedef itk::Image<MaskPixelType, dimension3D> MaskType;

    typedef itk::ImageMaskSpatialObject< dimension3D > MaskSpatialType;

    typedef InputImageType::RegionType InputRegionType;

    typedef itk::Image<InputPixelType, dimension2D> SliceImageType;
    typedef itk::Image<MaskPixelType, dimension2D> SliceImageMaskType;

    typedef itk::ImageFileReader<InputImageType> ReaderType;
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
    typedef itk::ImageFileReader<MaskType> MaskReaderType;

    typedef itk::ExtractImageFilter<InputImageType, SliceImageType> ExtractImageFilterType;
    typedef itk::ExtractImageFilter<MaskType, SliceImageMaskType> ExtractImageMaskFilterType;

    typedef itk::StatisticsImageFilter<SliceImageType> StatisticsImageFilterType;
    typedef itk::AddImageFilter<SliceImageType, SliceImageType, SliceImageType> AddFilterType;

    typedef itk::DivideImageFilter<InputImageType, InputImageType, InputImageType> itkDivideFilter;

    //

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(inputImageFile);
    try
    {
        reader->Update();
    }
    catch( itk::ExceptionObject & excp )
    {
		throw "error opening input file";
        return false;
    }

    InputImageType::Pointer inputImage = reader->GetOutput();


    MaskReaderType::Pointer maskReader = MaskReaderType::New();
    maskReader->SetFileName(inputMaskFile);
    try{
    maskReader->Update();
    }
    catch (itk::ExceptionObject &err)
    {
	   throw "error opening input file";
       return false;
    }

    MaskType::Pointer maskImage = maskReader->GetOutput();

    maskImage->SetOrigin(inputImage->GetOrigin());
    maskImage->SetSpacing(inputImage->GetSpacing());

    MaskSpatialType::Pointer mask = MaskSpatialType::New();
    mask -> SetImage( maskImage );

    InputRegionType roi = mask -> GetAxisAlignedBoundingBoxRegion();


    ReaderType::Pointer biasReader = ReaderType::New();
    biasReader->SetFileName(inputBiasFieldFile);

    try
    {
        biasReader->Update();
    }
    catch( itk::ExceptionObject & excp )
    {
		throw "error reading bias field";
        return false;
    }


    InputImageType::Pointer biasImage = biasReader->GetOutput();

    biasImage->SetOrigin(inputImage->GetOrigin());
    biasImage->SetSpacing(inputImage->GetSpacing());
    biasImage->SetDirection(inputImage->GetDirection());
    biasImage->Update();

    //std::cerr << "Compute the slice corrected... " << std::endl;
    typedef itk::ExpImageFilter<OutputImageType, OutputImageType> ExpFilterType;
    ExpFilterType::Pointer expFilter = ExpFilterType::New();
    expFilter->SetInput( biasImage );
    expFilter->Update();

    typedef itk::DivideImageFilter<OutputImageType, OutputImageType, OutputImageType> DividerType;
    DividerType::Pointer divider = DividerType::New();
    divider->SetInput1( inputImage );
    divider->SetInput2( expFilter->GetOutput() );
    divider->SetConstant2(1e-6);
    divider->Update();

    OutputImageType::Pointer outputImage = divider->GetOutput();

    InputImageType::IndexType inputIndex = roi.GetIndex();
    InputImageType::SizeType  inputSize  = roi.GetSize();

    // Remove the min value slice by slice
    //Loop over slices in the brain mask
    for ( unsigned int i=inputIndex[2]; i < inputIndex[2] + inputSize[2]; i++ )
    {
        InputImageType::RegionType wholeSliceRegion;
        wholeSliceRegion = roi;

        InputImageType::IndexType  wholeSliceRegionIndex = wholeSliceRegion.GetIndex();
        InputImageType::SizeType   wholeSliceRegionSize  = wholeSliceRegion.GetSize();

        wholeSliceRegionIndex[2]= i;
        wholeSliceRegionSize[2] = 0;

        wholeSliceRegion.SetIndex(wholeSliceRegionIndex);
        wholeSliceRegion.SetSize(wholeSliceRegionSize);

        //Extract slice in input mask
        ExtractImageMaskFilterType::Pointer sliceMaskExtractor = ExtractImageMaskFilterType::New();
        sliceMaskExtractor->SetExtractionRegion(wholeSliceRegion);
        sliceMaskExtractor->SetInput(maskImage);
#if ITK_VERSION_MAJOR >= 4
        sliceMaskExtractor->SetDirectionCollapseToIdentity(); // This is required.
#endif
        sliceMaskExtractor->Update();

        //Extract slice in input image
        ExtractImageFilterType::Pointer sliceExtractor = ExtractImageFilterType::New();
        sliceExtractor->SetExtractionRegion(wholeSliceRegion);
        sliceExtractor->SetInput(outputImage);
#if ITK_VERSION_MAJOR >= 4
        sliceExtractor->SetDirectionCollapseToIdentity(); // This is required.
#endif
        sliceExtractor->Update();

        //Extract min value in the slice contained in the brain mask

        itk::ImageRegionIterator<SliceImageType> ItS(sliceExtractor->GetOutput(),sliceExtractor->GetOutput()->GetLargestPossibleRegion());
        itk::ImageRegionIterator<SliceImageMaskType> ItSM(sliceMaskExtractor->GetOutput(),sliceMaskExtractor->GetOutput()->GetLargestPossibleRegion());

        float minValue = FLT_MAX;

        for( ItS.GoToBegin(), ItSM.GoToBegin(); !ItS.IsAtEnd(); ++ItS, ++ItSM)
        {
            if(ItSM.Get() > 1e-2 && ItS.Get() > 1e-1)
            {
                if(ItS.Get() <= minValue) minValue = ItS.Get();
            }
        }

        //std::cout << "min : " << minValue << std::endl;

        //REMOVE THE MIN VALUE and set the new value in the output image (if contained in the brain mask, otherwise value set to zero)

        OutputImageType::RegionType wholeSliceRegion3D;
        wholeSliceRegion3D = roi;

        InputImageType::IndexType  wholeSliceRegionIndex3D = wholeSliceRegion3D.GetIndex();
        InputImageType::SizeType   wholeSliceRegionSize3D  = wholeSliceRegion3D.GetSize();

        wholeSliceRegionIndex3D[2]= i;
        wholeSliceRegionSize3D[2] = 1;

        wholeSliceRegion3D.SetIndex(wholeSliceRegionIndex3D);
        wholeSliceRegion3D.SetSize(wholeSliceRegionSize3D);

        itk::ImageRegionIterator<OutputImageType> ItO(outputImage,wholeSliceRegion3D);

        for( ItS.GoToBegin(), ItSM.GoToBegin(), ItO.GoToBegin(); !ItS.IsAtEnd(); ++ItS, ++ItSM, ++ItO)
        {
            if((ItS.Get() - minValue) > 0.0)
            {
                ItO.Set( (ItS.Get() - minValue) );
            }
            else
            {
                ItO.Set(0);
            }
        }



    }

    //  WriterType::Pointer dbgWriter = WriterType::New();
    //  dbgWriter->SetFileName("/home/tourbier/Desktop/F022_DiffBiasCorr/F022_orig_masked_crop_bcorr_interm.nii");
    //  dbgWriter->SetInput(outputImage);
    //  dbgWriter->Update();

    //Extract the min value of the inputImage and remove it
    /*
  itkStatisticsImageFilter::Pointer statisticsImageFilter = itkStatisticsImageFilter::New ();
  statisticsImageFilter->SetInput(inputImage);
  statisticsImageFilter->Update();

  InputPixelType minValue = statisticsImageFilter->GetMinimum();

  std::cerr << "Remove the min value : " << minValue << std::endl;

  itkAddFilter::Pointer add = itkAddFilter::New();
  add->SetInput1(outputImage);
  add->SetConstant2(-minValue);
  add->Update();
  outputImage = add->GetOutput();
  */

    //

    //std::cerr << "Write the bias-corrected volume... " << std::endl;

    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputImageFile);
    writer->SetInput(outputImage);

    try
    {
    writer->Update();
    }
    catch (itk::ExceptionObject &err)
    {
		throw "error writing output";
       return false;
    }
    //std::cerr << "Done! " << std::endl;

    return true;
}
