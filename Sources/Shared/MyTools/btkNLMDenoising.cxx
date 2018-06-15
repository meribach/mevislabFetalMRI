/*
Copyright or © or Copr. Université de Strasbourg - Centre National de la Recherche Scientifique

25 january 2011
rousseau@unistra.fr

This software is governed by the CeCILL-B license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL-B
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL-B license and that you accept its terms.
*/

/*
This program implements a denoising method proposed by Coupé et al. described in :
 Coupé, P., Yger, P., Prima, S., Hellier, P., Kervrann, C., Barillot, C., 2008. 
 An optimized blockwise nonlocal means denoising filter for 3-D magnetic resonance images.
 IEEE Transactions on Medical Imaging 27 (4), 425–441.
*/


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkImage.h"
#include "itkConstrainedValueDifferenceImageFilter.h"

#include "btkNLMTool.h"

#include "btkNLMDenoising.h"
#include <vector>

#include <itkVersion.h>


btkNLMDenoise::btkNLMDenoise(const char* const _inputFile, const char* const _outputFile, const char* const _maskFile, const char* const _refFile,
	float _padding, int _hwn, int _hwvs, float _beta, int _block, int _center, int _optimized, float _lowerMeanThreshold,
	float _lowerVarianceThreshold, const char* const _differenceFile, int _localSmoothing)
{
  inputFile = _inputFile;
  outputFile = _outputFile;
  maskFile = _maskFile;
  refFile = _refFile;
  padding = _padding;
  hwn = _hwn;
  hwvs = _hwvs;
  beta = _beta;
  block = _block;
  center = _center;
  optimized = _optimized;
  lowerMeanThreshold = _lowerMeanThreshold;
  lowerVarianceThreshold = _lowerVarianceThreshold;
  differenceFile = _differenceFile;
  localSmoothing = _localSmoothing;

}

btkNLMDenoise::~btkNLMDenoise()
{

}


bool btkNLMDenoise::runDenoising()
{

	//const char * itkVersion = itk::Version::GetITKVersion();
	//std::cout << "itk version: " << itkVersion << std::endl;
	//itk::Version::Pointer version = itk::Version::New();
	//std::cout << version->GetITKVersion() << std::endl;
	
	//ITK declaration
    typedef float PixelType;
    const   unsigned int Dimension = 3;
	typedef itk::Image< PixelType, Dimension > ImageType;  //same type for input and output
    typedef ImageType::Pointer ImagePointer;

    typedef itk::ImageFileReader< ImageType >  ReaderType;
    typedef itk::ImageFileWriter< ImageType >  WriterType;

    //Read the image
	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName(inputFile);
	try
	{
      reader->Update();
	}
	catch (itk::ExceptionObject &err)
	{
		throw "error opening input";
		return false;
	}
	
	ImagePointer inputImage = reader->GetOutput();

    ImagePointer outputImage = ImageType::New();

    ImagePointer maskImage;
    ImagePointer refImage;

    btk::NLMTool<PixelType> btkTool;

	btkTool.SetInput(inputImage);

    if (std::strncmp(maskFile, "",4)!=0) {               //reading the mask image

	  ReaderType::Pointer maskReader = ReaderType::New();
      maskReader->SetFileName(maskFile);
	  try
	  {
		maskReader->Update();
	  }
	  catch (itk::ExceptionObject &err)
	  {
		throw "error opening maskFile : " + std::string(maskFile);
		return false;
	  }
	  maskImage = maskReader->GetOutput();
	  btkTool.SetMaskImage(maskImage);
    }
	else
	{
	  //creating a mask image using the padding value
		btkTool.SetPaddingValue(padding);
	}

	btkTool.SetPatchSize(hwn);
	btkTool.SetSpatialBandwidth(hwvs);
    
    if (std::strncmp(refFile, "",4)!=0)
	{
     ReaderType::Pointer refReader = ReaderType::New();
     refReader->SetFileName(refFile);
	 try
	 {
	   refReader->Update();
	 }
	 catch (itk::ExceptionObject &err)
	 {
	   throw "error opening refFile";
	   return false;
	 }
	  refImage = refReader->GetOutput();
	  btkTool.SetReferenceImage(refImage);
    }    
    
	btkTool.SetCentralPointStrategy(center);
	btkTool.SetBlockwiseStrategy(block);
	btkTool.SetOptimizationStrategy(optimized);
	btkTool.SetLowerThresholds(lowerMeanThreshold, lowerVarianceThreshold);

	btkTool.SetSmoothing(beta);
    if(localSmoothing == 1)
		btkTool.SetLocalSmoothing(beta);

    
	btkTool.ComputeOutput();

    outputImage = btkTool.GetOutput();

    //Write the result
    WriterType::Pointer writer = WriterType::New();
    try
	{
	  writer->SetFileName(outputFile);
      writer->SetInput( outputImage );
      writer->Update();
	}
	catch (itk::ExceptionObject &err)
	{
	  throw "error writing output file";
	  return false;

	}
    if (strncmp(differenceFile, "",4) !=0)
	{
      itk::ConstrainedValueDifferenceImageFilter<ImageType,ImageType,ImageType>::Pointer diffFilter = itk::ConstrainedValueDifferenceImageFilter<ImageType,ImageType,ImageType>::New();

      diffFilter->SetInput1( inputImage );
      diffFilter->SetInput2( outputImage );

      WriterType::Pointer diffWriter = WriterType::New();
      
	  try
	  {
	    diffWriter->SetFileName( differenceFile );
        diffWriter->SetInput( diffFilter->GetOutput() );
        diffWriter->Update();
	  }
	  catch (itk::ExceptionObject &err)
	  {
		throw "error writing diff file";
		return false;

	  }
    }

	return true;

}
