/*==========================================================================
  © 
  
  Program: Apply mask to input image and return the output masked image
  Language: C++
  Date: $Date: 2015-05-07 $
  Version: $Revision: 1.0 $
  Author: $Sebastien Tourbier$

==========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

/* Standard includes */

#include <sstream>  
#include <string>
#include <stdlib.h> 

/* Itk includes */
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkEuler3DTransform.h"

#include "itkMultiplyImageFilter.h"

#include "itkResampleImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"

#include "mialsrtkMaskImage.h"

mialsrtkMaskImage::mialsrtkMaskImage(const char* const _inputFile, const char* const _maskFile, const char* const _outputFile)
{
	inputFile = _inputFile;
	maskFile = _maskFile;
	outputFile = _outputFile;
}

mialsrtkMaskImage::~mialsrtkMaskImage()
{

}


bool mialsrtkMaskImage::runMaskImage()
{
	
         // typedefs
        const   unsigned int    Dimension = 3;
        typedef float  PixelType;
        typedef unsigned char  MaskPixelType;

        typedef itk::Image< PixelType, Dimension >  ImageType;
        typedef itk::ImageFileReader< ImageType > ImageReaderType;
        typedef itk::ImageFileWriter< ImageType > ImageWriterType;

        typedef itk::Image< MaskPixelType, Dimension >  ImageMaskType;
        typedef itk::ImageFileReader< ImageMaskType > MaskReaderType;

        ImageReaderType::Pointer imageReader = ImageReaderType::New();
        imageReader -> SetFileName(inputFile);
        try
		{
		imageReader->Update();
		}
		catch (itk::ExceptionObject & error)
		{
			throw "error reading input image";
			return false;
		}

        ImageType::Pointer im = imageReader->GetOutput();


        MaskReaderType::Pointer maskReader = MaskReaderType::New();
        maskReader -> SetFileName(maskFile);
        try
		{
		maskReader->Update();
		}
		catch (itk::ExceptionObject & error)
		{
			throw "error reading mask";
			return false;
		}

        typedef itk::ResampleImageFilter<ImageMaskType,ImageMaskType> ResampleImageMaskFilterType;
        typedef itk::NearestNeighborInterpolateImageFunction<ImageMaskType> NNInterpolatorType;

        ResampleImageMaskFilterType::Pointer maskUpsampler = ResampleImageMaskFilterType::New();
        NNInterpolatorType::Pointer nnInterpolator = NNInterpolatorType::New();

        typedef  itk::Euler3DTransform<double> Euler3DTransformType;
        Euler3DTransformType::Pointer idTransform = Euler3DTransformType::New();
        idTransform->SetIdentity();

        maskUpsampler -> SetInterpolator(nnInterpolator);
        maskUpsampler -> SetInput(maskReader->GetOutput());
        maskUpsampler -> SetTransform(idTransform);
        maskUpsampler -> SetOutputParametersFromImage(im);
        maskUpsampler -> SetOutputSpacing(im->GetSpacing());
        maskUpsampler -> SetSize(im->GetLargestPossibleRegion().GetSize());

        maskUpsampler -> Update();


        typedef itk::MultiplyImageFilter< ImageType, ImageMaskType, ImageType > MultiplyImageFilterType;
        MultiplyImageFilterType::Pointer filter = MultiplyImageFilterType::New();
        filter -> SetInput1(imageReader -> GetOutput());
        filter -> SetInput2(maskUpsampler -> GetOutput());

        ImageWriterType::Pointer writer =  ImageWriterType::New();
        writer -> SetFileName(outputFile);
        writer -> SetInput( filter->GetOutput() );

        try
	    {
	    	writer->Update();
	    }
	  	catch( itk::ExceptionObject & error )
	    {
			throw "error writing masked image";
	    	return false;
	    }


    return true;
}
