/*=========================================================================

Program: Mean slice intensity correctiom
Language: C++
Date: $Date: 2012-28-12 $
Version: $Revision: 1 $
Author: $Sebastien Tourbier$

Copyright (c) 2017 Medical Image Analysis Laboratory (MIAL), Lausanne
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
     
==========================================================================*/

#include "mialsrtkCorrectSliceIntensity.h"


correctSliceIntensity::correctSliceIntensity(const char* const _inputFile, const char* const _maskFile, const char* const _outputFile)
{
	inputFile = _inputFile;
	outputFile = _outputFile;
	maskFile = _maskFile;

}


correctSliceIntensity::~correctSliceIntensity()
{


}

bool correctSliceIntensity::SliceIntensityCorrection()
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

	// typedef itk::StatisticsImageFilter<SliceImageType> StatisticsSliceImageFilterType;
	typedef itk::AddImageFilter<SliceImageType, SliceImageType, SliceImageType> AddFilterType;

	typedef itk::DivideImageFilter<InputImageType, InputImageType, InputImageType> itkDivideFilter;

	typedef itk::ChangeInformationImageFilter<MaskType> ChangeInfoFilter;

	ReaderType::Pointer reader = ReaderType::New();
	reader->SetFileName(inputFile);
	try
	{
		reader->Update();
	}
	catch (itk::ExceptionObject &err)
	{
		throw "error opening inputFile";
		return false;
	}

	MaskReaderType::Pointer imMaskReader = MaskReaderType::New();
	imMaskReader->SetFileName(inputFile);
	try
	{
	  imMaskReader->Update();
	}
	catch (itk::ExceptionObject &err)
	{
		throw "error opening inputFile second time";
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
		throw "error opening maskFile";
		return false;
	}


	ChangeInfoFilter::Pointer changeInfoFilter = ChangeInfoFilter::New();
	changeInfoFilter->SetInput(maskReader->GetOutput());
	changeInfoFilter->UseReferenceImageOn();
	changeInfoFilter->ChangeRegionOn();
	changeInfoFilter->SetReferenceImage(imMaskReader->GetOutput());
	changeInfoFilter->Update();

	MaskType::Pointer maskImage = changeInfoFilter->GetOutput();

	maskImage->SetOrigin(inputImage->GetOrigin());
	maskImage->SetSpacing(inputImage->GetSpacing());
	maskImage->SetDirection(inputImage->GetDirection());
	maskImage->SetLargestPossibleRegion(inputImage->GetLargestPossibleRegion());

	maskImage->Update();

	MaskSpatialType::Pointer mask = MaskSpatialType::New();
	mask->SetImage(maskImage);

	InputRegionType roi = mask->GetAxisAlignedBoundingBoxRegion();

	InputImageType::IndexType inputIndex = roi.GetIndex();
	InputImageType::SizeType  inputSize = roi.GetSize();

	//Initialize the output image
	OutputImageType::Pointer outputImage = OutputImageType::New();
	outputImage->SetRegions(inputImage->GetLargestPossibleRegion());
	outputImage->Allocate();
	outputImage->FillBuffer(0.0);

	outputImage->SetOrigin(inputImage->GetOrigin());
	outputImage->SetSpacing(inputImage->GetSpacing());
	outputImage->SetDirection(inputImage->GetDirection());

	//Compute global mean intensity inside the brain
	itk::ImageRegionIterator<InputImageType> ItIm(inputImage, inputImage->GetLargestPossibleRegion());
	itk::ImageRegionIterator<MaskType> ItM(maskImage, inputImage->GetLargestPossibleRegion());

	float gmean = 0.0;
	int gcounter = 0;

	for (ItIm.GoToBegin(), ItM.GoToBegin(); !ItIm.IsAtEnd(); ++ItIm, ++ItM)
	{
		//here there is a threshold. check it makes sens as well if using other than T2* images
		if (ItM.Get() > 0.1)
		{
			gcounter++;
			gmean += ItIm.Get();
		}
	}
	gmean = gmean / gcounter;

	/*vnl_vector<float> x;
	x.set_size(gcounter);

	gcounter = 0;
	for (ItIm.GoToBegin(), ItM.GoToBegin(); !ItIm.IsAtEnd(); ++ItIm, ++ItM)
	{
		if (ItM.Get() > 0.1)
		{
			x[gcounter] = ItIm.Get();
			gcounter++;
		}
	}
	
	typedef float MeasurementValueType;
	typedef int RankValType;
	typedef vnl_vector<int> IndexVectorType;
	typedef mialsrtk::vnl_index_sort<MeasurementValueType, RankValType> IndexSortType;

	IndexSortType indexXSort;
	vnl_vector<float> sortedXVals;
	IndexVectorType sortXIndices;

	indexXSort.vector_sort(x, sortedXVals, sortXIndices);
	int indPercentile = (int)round(0.5 * (float)x.size());
	float gmedian = sortedXVals[indPercentile];*/


	/*
	// Computes mean slice intensity in a neighborhood of each slice
	//Loop over slices in the brain mask
	for (unsigned int i = inputIndex[2]; i < inputIndex[2] + inputSize[2]; i++)
	{

		InputImageType::RegionType wholeSliceRegion;
		wholeSliceRegion = roi;

		InputImageType::IndexType  wholeSliceRegionIndex = wholeSliceRegion.GetIndex();
		InputImageType::SizeType   wholeSliceRegionSize = wholeSliceRegion.GetSize();

		wholeSliceRegionIndex[2] = i;
		wholeSliceRegionSize[2] = 0;

		wholeSliceRegion.SetIndex(wholeSliceRegionIndex);
		wholeSliceRegion.SetSize(wholeSliceRegionSize);

		//std::cout << "inputMask " << std::endl;
		//std::cout << maskImage->GetLargestPossibleRegion() << std::endl;
		//std::cout << "inputImage" << std::endl;
		//std::cout << inputImage->GetLargestPossibleRegion() << std::endl;
		//std::cout << "wholeSliceRegion" << std::endl;
		//std::cout << wholeSliceRegion << std::endl;
		

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
		sliceExtractor->SetInput(inputImage);
#if ITK_VERSION_MAJOR >= 4
		sliceExtractor->SetDirectionCollapseToIdentity(); // This is required.
#endif
		sliceExtractor->Update();


		itk::ImageRegionIterator<SliceImageType> ItS(sliceExtractor->GetOutput(), sliceExtractor->GetOutput()->GetLargestPossibleRegion());
		itk::ImageRegionIterator<SliceImageMaskType> ItSM(sliceMaskExtractor->GetOutput(), sliceMaskExtractor->GetOutput()->GetLargestPossibleRegion());

		std::vector<float> vneighbmean;


		float neighbmean = 0.0;
		int neighbcnt = 0;
		float smean = 0.0;
		int scounter = 0;

		for (ItS.GoToBegin(), ItSM.GoToBegin(); !ItS.IsAtEnd(); ++ItS, ++ItSM)
		{
			if (ItSM.Get() > 0 && ItS.Get() > 0)
			{
				scounter++;
				smean += ItS.Get();
			}
		}

		if (scounter>0) smean = smean / (float)(scounter);

		vneighbmean.push_back(smean);

		neighbmean += smean;
		neighbcnt++;

		int neighbradius = 2;

		for (unsigned int j = -neighbradius; j <= neighbradius; j++)
		{
			if ((i - j)>inputIndex[2])
			{
				//Extract mean in preceding slice
				wholeSliceRegionIndex[2] = i - j;
				wholeSliceRegionSize[2] = 0;

				wholeSliceRegion.SetIndex(wholeSliceRegionIndex);
				wholeSliceRegion.SetSize(wholeSliceRegionSize);

				//Extract slice in input mask
				sliceMaskExtractor->SetExtractionRegion(wholeSliceRegion);
				sliceMaskExtractor->SetInput(maskImage);
#if ITK_VERSION_MAJOR >= 4
				sliceMaskExtractor->SetDirectionCollapseToIdentity(); // This is required.
#endif
				sliceMaskExtractor->Update();

				//Extract slice in input image
				sliceExtractor->SetExtractionRegion(wholeSliceRegion);
				sliceExtractor->SetInput(inputImage);
#if ITK_VERSION_MAJOR >= 4
				sliceExtractor->SetDirectionCollapseToIdentity(); // This is required.
#endif
				sliceExtractor->Update();

				//Extract min value in the slice contained in the brain mask

				itk::ImageRegionIterator<SliceImageType> ItS(sliceExtractor->GetOutput(), sliceExtractor->GetOutput()->GetLargestPossibleRegion());
				itk::ImageRegionIterator<SliceImageMaskType> ItSM(sliceMaskExtractor->GetOutput(), sliceMaskExtractor->GetOutput()->GetLargestPossibleRegion());

				float smeanMinusOne = 0.0;
				scounter = 0;
				for (ItS.GoToBegin(), ItSM.GoToBegin(); !ItS.IsAtEnd(); ++ItS, ++ItSM)
				{
					if (ItSM.Get() > 0 && ItS.Get() > 0)
					{
						scounter++;
						smeanMinusOne += ItS.Get();
					}
				}
				if (scounter>0)  smeanMinusOne = smeanMinusOne / scounter;

				vneighbmean.push_back(smeanMinusOne);

				neighbmean += smeanMinusOne;
				neighbcnt++;
			}

			if ((i - j)<inputIndex[2] + inputSize[2] - 1)
			{
				//Extract mean in preceding slice
				wholeSliceRegionIndex[2] = i + j;
				wholeSliceRegionSize[2] = 0;

				wholeSliceRegion.SetIndex(wholeSliceRegionIndex);
				wholeSliceRegion.SetSize(wholeSliceRegionSize);

				//Extract slice in input mask
				sliceMaskExtractor->SetExtractionRegion(wholeSliceRegion);
				sliceMaskExtractor->SetInput(maskImage);
#if ITK_VERSION_MAJOR >= 4
				sliceMaskExtractor->SetDirectionCollapseToIdentity(); // This is required.
#endif
				sliceMaskExtractor->Update();

				//Extract slice in input image
				sliceExtractor->SetExtractionRegion(wholeSliceRegion);
				sliceExtractor->SetInput(inputImage);
#if ITK_VERSION_MAJOR >= 4
				sliceExtractor->SetDirectionCollapseToIdentity(); // This is required.
#endif
				sliceExtractor->Update();

				//Extract min value in the slice contained in the brain mask
				itk::ImageRegionIterator<SliceImageType> ItS(sliceExtractor->GetOutput(), sliceExtractor->GetOutput()->GetLargestPossibleRegion());
				itk::ImageRegionIterator<SliceImageMaskType> ItSM(sliceMaskExtractor->GetOutput(), sliceMaskExtractor->GetOutput()->GetLargestPossibleRegion());

				float smeanPlusOne = 0.0;
				scounter = 0;
				for (ItS.GoToBegin(), ItSM.GoToBegin(); !ItS.IsAtEnd(); ++ItS, ++ItSM)
				{
					if (ItSM.Get() > 0 && ItS.Get() > 0)
					{
						scounter++;
						smeanPlusOne += ItS.Get();
					}
				}
				if (scounter>0) smeanPlusOne = smeanPlusOne / scounter;

				vneighbmean.push_back(smeanPlusOne);

				neighbmean += smeanPlusOne;
				neighbcnt++;
			}

		}

		if (neighbcnt>0)
		{
			//Get median of the mean intensity of the slices in the neighborhood
			std::sort(vneighbmean.begin(), vneighbmean.end());
			float neighbmedian = vneighbmean[vneighbmean.size() / 2];

			neighbmean = neighbmedian;
			//neighbmean = neighbmean / neighbcnt;

		}

		//if(scounter>0) smean = smean / scounter;

		//float smeanDiff = smean - gmean;
		float smeanDiff = smean - neighbmean;

		//std::cout << "slice mean intensity : " << smean << " , expected mean :  " << neighbmean << " ( diff : " << smeanDiff << " ) " << std::endl;

		//Shift the mean intensity of the slice to the global mean intensity of the stack

		OutputImageType::RegionType wholeSliceRegion3D;
		wholeSliceRegion3D = roi;

		InputImageType::IndexType  wholeSliceRegionIndex3D = wholeSliceRegion3D.GetIndex();
		InputImageType::SizeType   wholeSliceRegionSize3D = wholeSliceRegion3D.GetSize();

		wholeSliceRegionIndex3D[2] = i;
		wholeSliceRegionSize3D[2] = 1;

		wholeSliceRegion3D.SetIndex(wholeSliceRegionIndex3D);
		wholeSliceRegion3D.SetSize(wholeSliceRegionSize3D);

		itk::ImageRegionIterator<OutputImageType> ItO(outputImage, wholeSliceRegion3D);

		for (ItS.GoToBegin(), ItSM.GoToBegin(), ItO.GoToBegin(); !ItS.IsAtEnd(); ++ItS, ++ItSM, ++ItO)
		{
			if (ItS.Get() > smeanDiff)
			{
				ItO.Set((ItS.Get() - smeanDiff));
			}
			else
			{
				ItO.Set(0);
			}
		}



	}
	*/


	// Subtract gmean value slice by slice
	//Loop over slices in the brain mask
	for (unsigned int i = inputIndex[2]; i < inputIndex[2] + inputSize[2]; i++)
	{

		InputImageType::RegionType wholeSliceRegion;
		wholeSliceRegion = roi;

		InputImageType::IndexType  wholeSliceRegionIndex = wholeSliceRegion.GetIndex();
		InputImageType::SizeType   wholeSliceRegionSize = wholeSliceRegion.GetSize();

		wholeSliceRegionIndex[2] = i;
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
		sliceExtractor->SetInput(inputImage);
#if ITK_VERSION_MAJOR >= 4
		sliceExtractor->SetDirectionCollapseToIdentity(); // This is required.
#endif
		sliceExtractor->Update();

		//Extract min value in the slice contained in the brain mask

		itk::ImageRegionIterator<SliceImageType> ItS(sliceExtractor->GetOutput(), sliceExtractor->GetOutput()->GetLargestPossibleRegion());
		itk::ImageRegionIterator<SliceImageMaskType> ItSM(sliceMaskExtractor->GetOutput(), sliceMaskExtractor->GetOutput()->GetLargestPossibleRegion());

		float smean = 0.0;
		int scounter = 0;

		for (ItS.GoToBegin(), ItSM.GoToBegin(); !ItS.IsAtEnd(); ++ItS, ++ItSM)
		{
			if (ItSM.Get() > 0 && ItS.Get() > 0)
			{
				scounter++;
				smean += ItS.Get();
			}
		}

		if (scounter>0) smean = smean / scounter;

		float smeanDiff = smean - gmean;

		//Shift the mean intensity of the slice to the global mean intensity of the stack

		OutputImageType::RegionType wholeSliceRegion3D;
		wholeSliceRegion3D = roi;

		InputImageType::IndexType  wholeSliceRegionIndex3D = wholeSliceRegion3D.GetIndex();
		InputImageType::SizeType   wholeSliceRegionSize3D = wholeSliceRegion3D.GetSize();

		wholeSliceRegionIndex3D[2] = i;
		wholeSliceRegionSize3D[2] = 1;

		wholeSliceRegion3D.SetIndex(wholeSliceRegionIndex3D);
		wholeSliceRegion3D.SetSize(wholeSliceRegionSize3D);

		itk::ImageRegionIterator<OutputImageType> ItO(outputImage, wholeSliceRegion3D);

		for (ItS.GoToBegin(), ItSM.GoToBegin(), ItO.GoToBegin(); !ItS.IsAtEnd(); ++ItS, ++ItSM, ++ItO)
		{
			if (ItS.Get() > smeanDiff)
			{
				ItO.Set((ItS.Get() - smeanDiff));
			}
			else
			{
				ItO.Set(0);
			}
		}



	}


	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName(outputFile);
	writer->SetInput(outputImage);

	try 
	{
		writer->Update();
	}
	catch (itk::ExceptionObject &err)
	{
		throw "error writing outputFile";
		return false;
	}

	return true;
}

