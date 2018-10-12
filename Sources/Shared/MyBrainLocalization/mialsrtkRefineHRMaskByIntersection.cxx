/*==========================================================================

  ©

  Date: 01/05/2015
  Author(s): Sebastien Tourbier (sebastien.tourbier@unil.ch)

==========================================================================*/

/*#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
*/

/* Standard includes */

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


#include "mialsrtkRefineHRMaskByIntersection.h"


/* Time profiling */
/*
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 0
#else
#include <time.h>
#endif

double getTime(void)
{
    struct timespec tv;

#ifdef __MACH__
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    if(clock_get_time(cclock, &mts) != 0) return 0;
    mach_port_deallocate(mach_task_self(), cclock);
    tv.tv_sec = mts.tv_sec;
    tv.tv_nsec = mts.tv_nsec;
#else
    if(clock_gettime(CLOCK_REALTIME, &tv) != 0) return 0;
#endif
    return (((double) tv.tv_sec) + (double) (tv.tv_nsec / 1000000000.0));
}
*/

mialsrtkRefineHRMaskByIntersection::mialsrtkRefineHRMaskByIntersection(std::vector< std::string > _inputFiles, std::vector< std::string > _transformIn, std::vector< std::string > _maskFiles, const char* _outputHRFile,
	std::vector< std::string > _outputLRFiles, const char* _referenceFile, int _radiusDilation, bool _useStaple)
{
inputFiles = _inputFiles;
transformIn = _transformIn;
maskFiles = _maskFiles;
outputHRFile = _outputHRFile;
outputLRFiles = _outputLRFiles;
referenceFile = _referenceFile;
radiusDilation = _radiusDilation;
useStaple = _useStaple;
}

mialsrtkRefineHRMaskByIntersection::~mialsrtkRefineHRMaskByIntersection()
{

inputFiles.clear();
transformIn.clear();
maskFiles.clear();
outputHRFile = NULL;
outputLRFiles.clear();
referenceFile = NULL;
radiusDilation = NULL;
useStaple = NULL;

}

bool mialsrtkRefineHRMaskByIntersection::runRefineMask()
{

        const char *test = "undefined";

        std::vector< int > x1, y1, z1, x2, y2, z2;

		// typedefs
        const   unsigned int    Dimension = 3;
        typedef float  PixelType;

        typedef itk::Image< PixelType, Dimension >  ImageType;
        typedef ImageType::Pointer                  ImagePointer;
        typedef std::vector<ImagePointer>           ImagePointerArray;

        typedef itk::Image< unsigned char, Dimension >  ImageMaskType;
        typedef itk::ImageFileReader< ImageMaskType > MaskReaderType;
        typedef itk::ImageMaskSpatialObject< Dimension > MaskType;

        typedef ImageType::RegionType               RegionType;
        typedef std::vector< RegionType >           RegionArrayType;
        
        typedef mialsrtk::SliceBySliceTransformBase< double, Dimension > TransformBaseType;
        typedef mialsrtk::SliceBySliceTransform< double, Dimension > TransformType;
        typedef TransformType::Pointer                          TransformPointer;

        // Register the SliceBySlice transform (a non-default ITK transform) with the TransformFactory of ITK
        itk::TransformFactory<TransformType>::RegisterTransform();

        typedef itk::ImageFileReader< ImageType >   ImageReaderType;
        typedef itk::ImageFileWriter< ImageType >   ImageWriterType;
        typedef itk::ImageFileWriter< ImageMaskType >   MaskWriterType;

        typedef itk::TransformFileReader     TransformReaderType;
        typedef TransformReaderType::TransformListType * TransformListType;

        // Rigid 3D transform definition (typically for reconstructions in adults)

        typedef itk::VersorRigid3DTransform< double > EulerTransformType;

        //typedef mialsrtk::ImageIntersectionCalculator<ImageType> IntersectionCalculatorType;
        //IntersectionCalculatorType::Pointer intersectionCalculator = IntersectionCalculatorType::New();

        // Interpolator used to compute the error metric between 2 registration iterations
        typedef itk::NearestNeighborInterpolateImageFunction<ImageMaskType,double>     NNMaskInterpolatorType;
        typedef itk::LinearInterpolateImageFunction<ImageType,double>     LinearImageInterpolatorType;
        //typedef itk::BSplineInterpolateImageFunction<ImageType,double>     BSplineInterpolatorType;

        typedef itk::ResampleImageFilter<ImageMaskType, ImageMaskType> ResamplerImageMaskFilterType;
        typedef itk::ResampleImageFilter<ImageType, ImageType> ResamplerImageFilterType;

        typedef itk::ExtractImageFilter<ImageMaskType, ImageMaskType> ExtractImageMaskFilterType;

        //typedef itk::CastImageFilter<ImageType,ImageMaskType> CasterType;

        // A helper class which creates an image which is perfect copy of the input image
        typedef itk::ImageDuplicator<ImageType> DuplicatorType;

        typedef itk::OrientImageFilter<ImageType,ImageType> OrientImageFilterType;
        typedef itk::OrientImageFilter<ImageMaskType,ImageMaskType> OrientImageMaskFilterType;

        typedef itk::ImageRegionIterator< ImageMaskType >  MaskIteratorType;
        typedef itk::ImageRegionIterator< ImageType >  IteratorType;
        typedef itk::ImageRegionIteratorWithIndex< ImageMaskType >  MaskIteratorTypeWithIndex;
        typedef itk::ImageRegionIteratorWithIndex< ImageType >  IteratorTypeWithIndex;

        typedef itk::AddImageFilter< ImageMaskType, ImageMaskType, ImageMaskType > AddImageMaskFilter;
        typedef itk::MultiplyImageFilter< ImageMaskType, ImageMaskType, ImageMaskType > MultiplyImageMaskFilterType;

        typedef itk::BinaryBallStructuringElement<ImageMaskType::PixelType, ImageMaskType::ImageDimension> StructuringElementType;
        typedef itk::BinaryDilateImageFilter <ImageMaskType, ImageMaskType, StructuringElementType> BinaryDilateImageFilterType;

        BinaryDilateImageFilterType::RadiusType radiusDilation2D;
        radiusDilation2D[0] = radiusDilation;
        radiusDilation2D[1] = radiusDilation;
        radiusDilation2D[2] = 0; // Dilate only in the plane of the slice

        BinaryDilateImageFilterType::RadiusType radiusDilation3D;
        radiusDilation3D[0] = radiusDilation;
        radiusDilation3D[1] = radiusDilation;
        radiusDilation3D[2] = radiusDilation;

        unsigned int numberOfImages = maskFiles.size();

        std::vector<OrientImageFilterType::Pointer> orientImageFilter(numberOfImages);
        std::vector<OrientImageMaskFilterType::Pointer> orientMaskImageFilter(numberOfImages);

        std::vector< ImageMaskType::Pointer >     imageMasks(numberOfImages);
        std::vector< TransformPointer >     transforms(numberOfImages);

        std::vector<MaskType::Pointer> masks(numberOfImages);
        std::vector< RegionType >           rois(numberOfImages);

        ImageType::IndexType  roiIndex;
        ImageType::SizeType   roiSize;

        // Filter setup
        for (unsigned int i=0; i<numberOfImages; i++)
        {

            ImageReaderType::Pointer imageReader = ImageReaderType::New();
            imageReader -> SetFileName(inputFiles[i].c_str() );
            try
			{
			imageReader -> Update();
			}
			catch (itk::ExceptionObject &err)
			{
				throw "error opening inputFile" + std::string(inputFiles[i].c_str());
				return false;
			}

            MaskReaderType::Pointer maskReader = MaskReaderType::New();
            maskReader -> SetFileName(maskFiles[i].c_str() );
            try
			{
			maskReader -> Update();
			}
			catch (itk::ExceptionObject &err)
			{
				throw "error opening maskFile" + std::string(maskFiles[i].c_str());
				return false;
			}
            imageMasks[i] = maskReader  -> GetOutput();

            /*
            orientMaskImageFilter[i] = OrientImageMaskFilterType::New();
            orientMaskImageFilter[i] -> UseImageDirectionOn();
            orientMaskImageFilter[i] -> SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
            orientMaskImageFilter[i] -> SetInput(maskReader -> GetOutput());
            orientMaskImageFilter[i] -> Update();

            imageMasks[i] = orientMaskImageFilter[i]  -> GetOutput();
            */

            //MaskType::Pointer mask = MaskType::New();
            masks[i]= MaskType::New();
            masks[i] -> SetImage( imageMasks[i] );

            rois[i] = masks[i] -> GetAxisAlignedBoundingBoxRegion();
            //std::cout << "rois "<< i << " : "<<rois[i]<<std::endl;

            TransformReaderType::Pointer transformReader = TransformReaderType::New();
            transformReader -> SetFileName(transformIn[i] );
			try
			{
				transformReader->Update();
			}
			catch (itk::ExceptionObject &err)
			{
				throw "error opening transformIn" + std::string(transformIn[i].c_str());
				return false;
			}
			

            TransformListType transformsList = transformReader->GetTransformList();
            TransformReaderType::TransformListType::const_iterator titr = transformsList->begin();
            TransformPointer trans = static_cast< TransformType * >( titr->GetPointer() );

            //transforms[i]= TransformType::New();
            transforms[i] = TransformType::New();
            transforms[i]=static_cast< TransformType * >( titr->GetPointer() );
            //transforms[i] -> SetImage( preImages[i] );
            //transforms[i] -> SetImage( orientImageFilter[i] -> GetOutput());
            transforms[i] -> SetImage( imageReader -> GetOutput());
        }

        // Set the reference image
        ImageReaderType::Pointer refReader = ImageReaderType::New();
        refReader -> SetFileName(referenceFile);
		try
		{
        refReader -> Update();
		}
		catch (itk::ExceptionObject &err)
		{
			throw "error opening referenceFile" + std::string(referenceFile);
			return false;
		}


        ImageType::Pointer referenceIm = refReader->GetOutput();

        /*
        OrientImageFilterType::Pointer orientRefImageFilter = OrientImageFilterType::New();
        orientRefImageFilter -> UseImageDirectionOn();
        orientRefImageFilter -> SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
        orientRefImageFilter -> SetInput(refReader -> GetOutput());
        orientRefImageFilter -> Update();

        ImageType::Pointer referenceIm = orientRefImageFilter->GetOutput();
        */

        ImageType::RegionType referenceRegion =  referenceIm->GetLargestPossibleRegion();

        //std::cout << "Reference region : " << referenceIm->GetLargestPossibleRegion() << std::endl;
        //std::cout << "Reference image size at loading : " << referenceIm->GetLargestPossibleRegion().GetNumberOfPixels() << std::endl;

        //start_time_unix = mialtk::getTime();;
        
        //Refine the output SR mask by the intersection of all LR mask
        //Should create an output HR mask
        //Should resample each LR mask by applying the slice transform, then copy the given slice in the HR mask...
        // Think about how to design the intersection...

        ImageMaskType::Pointer outImageMask = ImageMaskType::New();
        outImageMask->SetRegions(referenceIm->GetLargestPossibleRegion());
        outImageMask->Allocate();
        outImageMask->FillBuffer(0.0);

        outImageMask->SetOrigin(referenceIm->GetOrigin());
        outImageMask->SetSpacing(referenceIm->GetSpacing());
        outImageMask->SetDirection(referenceIm->GetDirection());

        IteratorType itRefIm(referenceIm,referenceIm->GetLargestPossibleRegion());


        //Extract each slices of the stack for subsequent faster injection

        std::vector< std::vector< ImageMaskType::Pointer > > StacksOfMaskSlices(numberOfImages);
        std::vector< std::vector< itk::VersorRigid3DTransform<double>::Pointer > > transformsArray(numberOfImages);
        std::vector< std::vector< itk::Transform<double>::Pointer > > invTransformsArray(numberOfImages);

        for(unsigned s=0; s<numberOfImages ; s++)
        {
            ImageMaskType::IndexType inputIndex = rois[s].GetIndex();
            ImageMaskType::SizeType  inputSize  = rois[s].GetSize();

            unsigned int i=inputIndex[2] + inputSize[2];

            //StacksOfSlices[s].set_size(inputSize[2]);

            //Loop over images of the current stack
            for ( unsigned int i=inputIndex[2]; i < inputIndex[2] + inputSize[2]; i++ )
            {
                //std::cout << "process image # " << s << " slice #" << i << std::endl;

                ImageMaskType::RegionType wholeSliceRegion;
                wholeSliceRegion = rois[s];

                ImageMaskType::IndexType  wholeSliceRegionIndex = wholeSliceRegion.GetIndex();
                ImageMaskType::SizeType   wholeSliceRegionSize  = wholeSliceRegion.GetSize();

                wholeSliceRegionIndex[2]= i;
                wholeSliceRegionSize[2] = 1;

                wholeSliceRegion.SetIndex(wholeSliceRegionIndex);
                wholeSliceRegion.SetSize(wholeSliceRegionSize);

                //Extract slice
                ExtractImageMaskFilterType::Pointer sliceExtractor = ExtractImageMaskFilterType::New();
                sliceExtractor->SetExtractionRegion(wholeSliceRegion);
                sliceExtractor->SetInput(imageMasks[s]);
#if ITK_VERSION_MAJOR >= 4
                sliceExtractor->SetDirectionCollapseToIdentity(); // This is required.
#endif
                sliceExtractor->Update();

                StacksOfMaskSlices[s].push_back(sliceExtractor->GetOutput());

                itk::VersorRigid3DTransform<double>::Pointer sliceTransformPtr = itk::VersorRigid3DTransform<double>::New();
                sliceTransformPtr = static_cast< itk::VersorRigid3DTransform<double>::Pointer >(transforms[s] -> GetSliceTransform(i));

                itk::VersorRigid3DTransform<double>::Pointer sliceInvTransformPtr = itk::VersorRigid3DTransform<double>::New();
                sliceInvTransformPtr -> SetCenter(sliceTransformPtr->GetCenter());
                sliceInvTransformPtr -> SetMatrix(sliceTransformPtr->GetMatrix());
                sliceInvTransformPtr -> SetOffset(sliceTransformPtr->GetOffset());

                //itk::VersorRigid3DTransform<double> * sliceTransform = static_cast< itk::VersorRigid3DTransform<double> * >(transforms[s] -> GetSliceTransform(i));

                transformsArray[s].push_back(sliceTransformPtr);

                //itk::VersorRigid3DTransform<double> * inverseSliceTransform = static_cast< itk::VersorRigid3DTransform<double> * >(transforms[s] -> GetSliceTransform(i) -> Clone());
                //itk::VersorRigid3DTransform<double>::Pointer inverseSliceTransform = itk::VersorRigid3DTransform<double>::New();
                //transforms[s] -> GetSliceTransform(i) -> GetInverse(inverseSliceTransform);

                invTransformsArray[s].push_back(sliceInvTransformPtr->GetInverseTransform());
            }
        }

        //Strictly speaking, this is not an injection process, but it's faster to do it this way
        ImageMaskType::PointType outputPoint;      //physical point in HR output image
        ImageMaskType::IndexType outputIndex;      //index in HR output image
        ImageMaskType::PointType transformedPoint; //Physical point location after applying affine transform
        itk::ContinuousIndex<double,3> inputContIndex;   //continuous index in LR image
        itk::ContinuousIndex<double,3>   interpolationContIndex;   //continuous index in LR image for interpolation (i.e. z = 0)
        //interpolationContIndex[2] = 0;

        int counter = 0;

        //Define a threshold for z coordinate based on FWHM = 2sqrt(2ln2)sigma = 2.3548 sigma
        float cst = sqrt(8*log(2.0));
        float deltaz = (2.0 / cst) * imageMasks[0]->GetSpacing()[2];

        int numberOfVoxels = outImageMask->GetLargestPossibleRegion().GetSize()[0]*outImageMask->GetLargestPossibleRegion().GetSize()[1]*outImageMask->GetLargestPossibleRegion().GetSize()[2];
        std::cout << "Number of voxels: " << numberOfVoxels << std::endl;

        NNMaskInterpolatorType::Pointer nnInterpolator = NNMaskInterpolatorType::New();

        std::vector< ImageMaskType::Pointer > outImageMasks(numberOfImages);

        for(unsigned int s=0; s<numberOfImages; s++)
        {

            outImageMasks[s] = ImageMaskType::New();
            outImageMasks[s]->SetRegions(referenceIm->GetLargestPossibleRegion());
            outImageMasks[s]->Allocate();
            outImageMasks[s]->FillBuffer(0.0);

            outImageMasks[s]->SetOrigin(referenceIm->GetOrigin());
            outImageMasks[s]->SetSpacing(referenceIm->GetSpacing());
            outImageMasks[s]->SetDirection(referenceIm->GetDirection());
            
            //unsigned int sizeX = m_ImageArray[0]->GetLargestPossibleRegion().GetSize()[0];
            //unsigned int sizeY = m_ImageArray[0]->GetLargestPossibleRegion().GetSize()[1];

            ImageMaskType::IndexType inputIndex = rois[s].GetIndex();
            ImageMaskType::SizeType  inputSize  = rois[s].GetSize();

            //TODO: Can we parallelize this ?
            //Iteration over the slices of the LR images

            unsigned int i=inputIndex[2] + inputSize[2];

            //Loop over slices of the current stack
            for ( unsigned int i=inputIndex[2]; i < inputIndex[2] + inputSize[2]; i++ )
            {
                //std::cout << "process image # " << s << " slice #" << i << std::endl;

                ImageMaskType::RegionType wholeSliceRegion;
                wholeSliceRegion = rois[s];

                ImageMaskType::IndexType  wholeSliceRegionIndex = wholeSliceRegion.GetIndex();
                ImageMaskType::SizeType   wholeSliceRegionSize  = wholeSliceRegion.GetSize();

                wholeSliceRegionIndex[2]= i;
                wholeSliceRegionSize[2] = 1;

                wholeSliceRegion.SetIndex(wholeSliceRegionIndex);
                wholeSliceRegion.SetSize(wholeSliceRegionSize);

                //Extract slice
                ExtractImageMaskFilterType::Pointer sliceExtractor = ExtractImageMaskFilterType::New();
                sliceExtractor->SetExtractionRegion(wholeSliceRegion);
                sliceExtractor->SetInput(imageMasks[s]);
#if ITK_VERSION_MAJOR >= 4
                sliceExtractor->SetDirectionCollapseToIdentity(); // This is required.
#endif
                sliceExtractor->Update();

                ResamplerImageMaskFilterType::Pointer sliceResampler = ResamplerImageMaskFilterType::New();
                sliceResampler -> SetInput(sliceExtractor->GetOutput());
                sliceResampler -> SetTransform(invTransformsArray[s][i-inputIndex[2]]);
                sliceResampler -> SetInterpolator(nnInterpolator);
                sliceResampler -> SetOutputParametersFromImage(outImageMasks[s]);
                sliceResampler -> Update();

                AddImageMaskFilter::Pointer addMaskFilter = AddImageMaskFilter::New();
                addMaskFilter -> SetInput1(outImageMasks[s]);
                addMaskFilter -> SetInput2(sliceResampler->GetOutput());
                addMaskFilter -> Update();

                outImageMasks[s] = addMaskFilter->GetOutput();

                //MultiplyImageMaskFilterType::Pointer multMaskFilter = MultiplyImageMaskFilterType::New();
                //multMaskFilter -> SetInput1(outImageMasks[s]);
                //multMaskFilter -> SetInput2(sliceResampler->GetOutput());
                //multMaskFilter -> Update();

                //outImageMasks[s] = multMaskFilter->GetOutput();
            }

            MaskIteratorTypeWithIndex itOutStackMask(outImageMasks[s],outImageMasks[s]->GetLargestPossibleRegion());
            for(itOutStackMask.GoToBegin(); !itOutStackMask.IsAtEnd(); ++itOutStackMask)
            {
                if(itOutStackMask.Get()>0.0) itOutStackMask.Set(1.0);
            }

           /* std::stringstream ssFile;
            ssFile << "/home/tourbier/Desktop/DbgMasks/hrResMask_" << s << ".nii.gz";

            MaskWriterType::Pointer writer = MaskWriterType::New();
            writer -> SetInput(outImageMasks[s]);
            writer -> SetFileName(ssFile.str().c_str());
            writer -> Update();
			*/

            if(useStaple)
            {
                AddImageMaskFilter::Pointer addMaskFilter2 = AddImageMaskFilter::New();
                addMaskFilter2 -> SetInput1(outImageMask);
                addMaskFilter2 -> SetInput2(outImageMasks[s]);
                addMaskFilter2 -> Update();

                outImageMask = addMaskFilter2->GetOutput();
            }
            
        }


        //Perform STAPLE to refine the HR brain mask
        typedef itk::Image< double , 4 > OutputSTAPLEImageType;
        typedef crl::MSTAPLEImageFilter<ImageMaskType, OutputSTAPLEImageType> MSTAPLEFilterType;
        MSTAPLEFilterType::Pointer staple = MSTAPLEFilterType::New();

        ImageType::Pointer outStapleImage = ImageType::New();
        outStapleImage->SetRegions(referenceIm->GetLargestPossibleRegion());
        outStapleImage->Allocate();
        outStapleImage->FillBuffer(0.0);
        outStapleImage->SetOrigin(referenceIm->GetOrigin());
        outStapleImage->SetSpacing(referenceIm->GetSpacing());
        outStapleImage->SetDirection(referenceIm->GetDirection());

        if(useStaple)
        {
            std::cout << "Perform STAPLE..." << std::endl;

            //typedef itk::Image< double , 4 > OutputSTAPLEImageType;
            //typedef crl::MSTAPLEImageFilter<ImageMaskType, OutputSTAPLEImageType> MSTAPLEFilterType;

            bool stapleMAP = false;
            bool stationaryPriorSet = false;
            bool initialExpertPerformanceSet = false;

            int underflowProtection = 0; //Underflow protection : 0 none, 1 strong, 2 extreme. Controls computation with none, some, or extremely extended precision. Useful with large numbers of input segmentations.
            bool useCompression = true; //Write out the reference standard using compression.
            bool assignConsensusVoxels = true; //Determines if voxels with label estimates that are the same amongst all inputs are assigned directly or used in the computation.
            bool startAtEStep = false; //Start at the E Step by estimating the reference standard (if true). Start at the M Step by estimating the parameters from the initial reference standard (if false).
            //bool startAtMStep = true; //Start at the M Step by estimating the parameters from the initial reference standard.

            double alpha = 0.0; //Alpha parameter of the beta distribution for MAP.
            double beta = 0.0; //Beta parameter of the beta distribution for MAP.

            int maxiterations = -1; //Maximum number of iterations. The E-M algorithm is terminated after this number of iterations has been computed.

            double relativeConvergence = 5e-07; //Relative convergence threshold, used to terminate the E-M algorithm when it has converged. Convergence is defined by relative changes in the mean trace of expert performance below this level.
            double stationaryWeight = 0.01; //Stationary prior weight, used to set the weight of stationary prior for each tissue class label with respect to spatially varying prior

            //MSTAPLEFilterType::Pointer staple = MSTAPLEFilterType::New();
            //staple->SetUnderflowProtection( underflowProtection );
            //staple->SetUseWriteCompression( useCompression );
            staple->SetAssignConsensusVoxels( assignConsensusVoxels );
            //staple->SetStartAtEStep( startAtEStep );

            //staple->SetMAPStaple(stapleMAP);
            
            if (stapleMAP)
            {
                staple->SetMAPAlpha(alpha);
                staple->SetMAPBeta(beta);
            }

            for(int s=0; s<numberOfImages; s++)
            {
                staple->SetInput(s,const_cast<ImageMaskType*>(outImageMasks[s].GetPointer()));
            }

            // Now apply the optional arguments to the object.
            if (maxiterations != -1) {
                staple->SetMaximumIterations(maxiterations);
            }

            //staple->SetRelativeConvergenceThreshold( relativeConvergence );
            //staple->SetStationaryPriorWeight( stationaryWeight );
            if ((stationaryWeight>1) || (stationaryWeight<0))
            {
                throw "Weight is not between 0 and 1";
                return false;
            }
            staple->SetNumberOfThreads(8);
            // Execute STAPLE
            try{
                staple->Update();
            }
            catch( itk::ExceptionObject & err )
            {
                throw "ExceptionObject caught ! staple part";
				return false;
            }

            //staple->Print(std::cout);
            //std::stringstream ssFileS;
            /*ssFileS << "/home/ch176971/Desktop/DbgMasks/staple2.nii.gz";

            itk::ImageFileWriter< OutputSTAPLEImageType >::Pointer writerS = itk::ImageFileWriter< OutputSTAPLEImageType >::New();
            writerS -> SetInput(staple->GetOutput());
            writerS -> SetFileName(ssFileS.str().c_str());
            writerS -> Update();
			*/


            //ImageMaskType::RegionType outRegion = outSTAPLE -> GetLargestPossibleRegion();

            //ImageMaskType::IndexType outIndex = outRegion.GetIndex();
            //ImageMaskType::SizeType  outSize  = outRegion.GetSize();

            OutputSTAPLEImageType::RegionType outStapleRegion = staple->GetOutput()->GetBufferedRegion();
            const unsigned int numberOfLabels = outStapleRegion.GetSize()[3];

            typedef itk::ImageLinearConstIteratorWithIndex< OutputSTAPLEImageType >  OutputSTAPLEIteratorTypeWithIndex;

            OutputSTAPLEIteratorTypeWithIndex itOutputSTAPLE(staple->GetOutput(), outStapleRegion);
            itOutputSTAPLE.SetDirection(3); //Walk along label dimension
            itOutputSTAPLE.GoToBegin();

            ImageMaskType::IndexType index3D;
            OutputSTAPLEImageType::IndexType index4D;



            //IteratorTypeWithIndex  itOutStapleIm(outStapleImage,outStapleImage->GetLargestPossibleRegion());
            //itOutStapleIm.GoToBegin();

            while(!itOutputSTAPLE.IsAtEnd())
            {
                itOutputSTAPLE.GoToBeginOfLine();

                while(!itOutputSTAPLE.IsAtEndOfLine())
                {
                    index4D = itOutputSTAPLE.GetIndex();

                    /*
                    if(index4D[3]==1 && itOutputSTAPLE.Get() >= 0.5)//Inside brain label
                    {
                        index3D[0] = index4D[0];
                        index3D[1] = index4D[1];
                        index3D[2] = index4D[2];

                        outImageMask->SetPixel( index3D, 1.0);
                    }
                    */

                    if(index4D[3]==1 && itOutputSTAPLE.Get() > 0.0)//Inside brain label
                    {
                        index3D[0] = index4D[0];
                        index3D[1] = index4D[1];
                        index3D[2] = index4D[2];

                        outImageMask->SetPixel( index3D, 1.0);
                        outStapleImage->SetPixel(index3D, itOutputSTAPLE.Get());

                    }

                    ++itOutputSTAPLE;
                }
                
                itOutputSTAPLE.NextLine();
            }
        }
        else //Perform majority voting to refine the HR brain mask
        {
            int mvThreshold = ceil(0.5 * numberOfImages);

            //std::cout << "Perform Majority Voting ,  thresh = " << mvThreshold << " ..." << std::endl;

            MaskIteratorTypeWithIndex itOutImageMask(outImageMask,outImageMask->GetLargestPossibleRegion());

            for(itOutImageMask.GoToBegin(); !itOutImageMask.IsAtEnd(); ++itOutImageMask)
            {
                if((numberOfImages % 2 == 0) && (itOutImageMask.Get() > mvThreshold))
                {
                    itOutImageMask.Set(1.0);
                }
                else if((numberOfImages % 2 != 0) && (itOutImageMask.Get() >= mvThreshold))
                {
                    itOutImageMask.Set(1.0);
                }
                else
                {
                    itOutImageMask.Set(0.0);
                }
            }
        }


        //std::stringstream ssFile3;
        /*ssFile3 << "/home/tourbier/Desktop/DbgMasks/hrResMask_beforeMRF.nii.gz";

        MaskWriterType::Pointer writer3 = MaskWriterType::New();
        writer3 -> SetInput(outImageMask);
        writer3 -> SetFileName(ssFile3.str().c_str());
        writer3 -> Update();
		*/
        //std::stringstream ssFile32;
        /*ssFile32 << "/home/tourbier/Desktop/DbgMasks/hrResMask_staple_beforeMRF.nii.gz";

        ImageWriterType::Pointer writer32 = ImageWriterType::New();
        writer32 -> SetInput(outStapleImage);
        writer32 -> SetFileName(ssFile32.str().c_str());
        writer32 -> Update();
		*/

        typedef itk::FixedArray< unsigned char,  1> FixedArrayMaskType;
        typedef itk::Image< FixedArrayMaskType, Dimension> ArrayImageMaskType;

        typedef itk::Image<itk::Vector<unsigned char,1>, Dimension> VectorImageMaskType;
        typedef itk::Image<itk::Vector<float,1>, Dimension> VectorImageType;

        // We convert the input into vector images
        //

        typedef VectorImageType::PixelType VectorImagePixelType;

        VectorImageType::Pointer vecImage = VectorImageType::New();
        vecImage->SetLargestPossibleRegion( outStapleImage->GetLargestPossibleRegion() );
        vecImage->SetBufferedRegion( outStapleImage->GetBufferedRegion() );
        vecImage->SetOrigin(outStapleImage->GetOrigin());
        vecImage->SetSpacing(outStapleImage->GetSpacing());
        vecImage->SetDirection(outStapleImage->GetDirection());
        vecImage->Allocate();

        //enum { VecImageDimension = VectorImageMaskType::ImageDimension };

        typedef itk::ImageRegionIterator< VectorImageType > VectorImageIterator;
        VectorImageIterator vecIt( vecImage, vecImage->GetBufferedRegion() );
        vecIt.GoToBegin();

        typedef itk::ImageRegionIterator< ImageType > ImageIterator;
        ImageIterator outIt( outStapleImage, outStapleImage->GetBufferedRegion() );
        outIt.GoToBegin();

         typedef itk::ImageRegionIterator< ImageMaskType > ImageMaskIterator;
        ImageMaskIterator outItMV(outImageMask,outImageMask->GetLargestPossibleRegion());
        outItMV.GoToBegin();

        //Set up the vector to store the image  data
        typedef VectorImageType::PixelType     DataVector;
        DataVector   dblVec;
        if (useStaple)
        {
            while ( !vecIt.IsAtEnd() )
            {

                dblVec[0] = outIt.Get();
                vecIt.Set(dblVec);
                ++vecIt;
                ++outIt;
            }
        }
        else
        {
            while ( !vecIt.IsAtEnd() )
            {
                dblVec[0] = (float) outItMV.Get();
                vecIt.Set(dblVec);
                ++vecIt;
                ++outItMV;
            }
        }

        //----------------------------------------------------------------------
        //Set membership function (Using the statistics objects)
        //----------------------------------------------------------------------
        typedef VectorImageType::PixelType         VectorImagePixelType;

        //typedef itk::Statistics::MahalanobisDistanceMembershipFunction< VectorImageMaskPixelType >  MembershipFunctionType;
        typedef itk::Statistics::DistanceToCentroidMembershipFunction< VectorImagePixelType > MembershipFunctionType;

        typedef MembershipFunctionType::Pointer MembershipFunctionPointer;
        typedef std::vector< MembershipFunctionPointer >    MembershipFunctionPointerVector;

        //----------------------------------------------------------------------
        //Set the decision rule
        //----------------------------------------------------------------------
        typedef itk::Statistics::DecisionRule::Pointer DecisionRuleBasePointer;
        typedef itk::Statistics::MinimumDecisionRule DecisionRuleType;
        DecisionRuleType::Pointer  myDecisionRule = DecisionRuleType::New();
        //std::cout << " site 3 " << std::endl;
        //----------------------------------------------------------------------
        // Set the classifier to be used and assigne the parameters for the
        // supervised classifier algorithm except the input image which is
        // grabbed from the Gibbs application pipeline.
        //----------------------------------------------------------------------
        //---------------------------------------------------------------------
        //  Software Guide : BeginLatex
        //
        //  Then we define the classifier that is needed
        //  for the Gibbs prior model to make correct segmenting decisions.
        //
        //  Software Guide : EndLatex
        // Software Guide : BeginCodeSnippet
        typedef itk::ImageClassifierBase< VectorImageType,ImageMaskType > ClassifierType;
        typedef ClassifierType::Pointer                    ClassifierPointer;
        ClassifierPointer myClassifier = ClassifierType::New();
        // Software Guide : EndCodeSnippet
        // Set the Classifier parameters
        myClassifier->SetNumberOfClasses(2);
        // Set the decison rule
        myClassifier->SetDecisionRule((DecisionRuleBasePointer) myDecisionRule );
        //Add the membership functions
        //for( unsigned int i=0; i<2; i++ )
        //{
        //    myClassifier->AddMembershipFunction( membershipFunctions[i] );
        //}

        double meanDistance = 0.5;
        MembershipFunctionType::CentroidType centroid(1);

        for(unsigned int k = 0; k < 2; k++)
        {
            MembershipFunctionType::Pointer membershipFunction = MembershipFunctionType::New();
            centroid[0] = k;
            membershipFunction->SetCentroid( centroid );
            myClassifier -> AddMembershipFunction( membershipFunction );
        }

        typedef itk::MRFImageFilter<  VectorImageType,  ImageMaskType> MRFImageFilterType;

        //Set the neighborhood radius.
        //For example, a neighborhood radius of 2 in a 3D image will result in a clique of size 5x5x5.
        //A neighborhood radius of 1 will result in a clique of size 3x3x3.
        int radius = 3;
        MRFImageFilterType::NeighborhoodRadiusType radiusMRF;
        radiusMRF[0] = radius;
        radiusMRF[1] = radius;
        radiusMRF[2] = 1;

        int numberOfNeighbors = (radius*2+1) * (radius*2+1) * (1*2+1);

        MRFImageFilterType::Pointer mrfFilter = MRFImageFilterType::New();

        mrfFilter -> SetInput(vecImage);

        mrfFilter -> SetNumberOfClasses(2);
        mrfFilter -> SetMaximumNumberOfIterations(50);
        mrfFilter -> SetErrorTolerance(1e-7);

        mrfFilter -> SetSmoothingFactor(30);

        mrfFilter -> SetNeighborhoodRadius(radiusMRF);

        mrfFilter->SetNumberOfThreads(8);

        std::vector< double > weights(numberOfNeighbors,1.0);
 

        double totalWeight = 0;
        for(std::vector< double >::const_iterator wcIt = weights.begin();
            wcIt != weights.end(); ++wcIt )
          {
          totalWeight += *wcIt;
          }
        for(std::vector< double >::iterator wIt = weights.begin();
            wIt != weights.end(); ++wIt )
          {
          *wIt = static_cast< double > ( (*wIt) * meanDistance / (2 * totalWeight));
          }
        mrfFilter->SetMRFNeighborhoodWeight( weights );

        mrfFilter -> SetClassifier(myClassifier);

        //std::cout << "Run Markov Random Field Filtering... "; std::cout.flush();

        mrfFilter -> Update();

        /*std::cout << "Number of Iterations : ";
        std::cout << mrfFilter->GetNumberOfIterations() << std::endl;
        std::cout << "Stop condition: " << std::endl;
        std::cout << "  (1) Maximum number of iterations " << std::endl;
        std::cout << "  (2) Error tolerance:  "  << std::endl;
        std::cout << mrfFilter->GetStopCondition() << std::endl;
		*/

        //Set up the vector to store the image  data

        typedef itk::ImageRegionIterator< ImageMaskType > ImageMaskIterator;

        ImageMaskIterator outMRFIt( mrfFilter->GetOutput(), mrfFilter->GetOutput()->GetBufferedRegion() );
        outMRFIt.GoToBegin();

        ImageMaskIterator outMaskIt( outImageMask, outImageMask->GetBufferedRegion() );
        outMaskIt.GoToBegin();

        while ( !outMRFIt.IsAtEnd() )
        {
            outMaskIt.Set(outMRFIt.Get());
            ++outMaskIt;
            ++outMRFIt;
        }


        /*std::stringstream ssFileMRF;
        ssFileMRF << "/home/tourbier/Desktop/DbgMasks/hrResMask_afterMRF.nii.gz";

        MaskWriterType::Pointer writerMRF = MaskWriterType::New();
        writerMRF -> SetInput(outImageMask.GetPointer());
        writerMRF -> SetFileName(ssFileMRF.str().c_str());
        writerMRF -> Update();
		*/


        //Refine the masks of LR stacks

        std::vector< ImageMaskType::Pointer > outLRImageMasks(numberOfImages);

        for(unsigned int s=0; s<numberOfImages; s++)
        {

            outLRImageMasks[s] = ImageMaskType::New();
            outLRImageMasks[s]->SetRegions(imageMasks[s]->GetLargestPossibleRegion());
            outLRImageMasks[s]->Allocate();
            outLRImageMasks[s]->FillBuffer(0.0);

            outLRImageMasks[s]->SetOrigin(imageMasks[s]->GetOrigin());
            outLRImageMasks[s]->SetSpacing(imageMasks[s]->GetSpacing());
            outLRImageMasks[s]->SetDirection(imageMasks[s]->GetDirection());

            /*std::cout << "Infos of Image LR # " << s << std::endl;
            std::cout << "Origin : " << imageMasks[s]->GetOrigin() << std::endl;
            std::cout << "Spacing : " << imageMasks[s]->GetSpacing() << std::endl;
            std::cout << "Direction : " << imageMasks[s]->GetDirection() << std::endl;
            */
            //unsigned int sizeX = m_ImageArray[0]->GetLargestPossibleRegion().GetSize()[0];
            //unsigned int sizeY = m_ImageArray[0]->GetLargestPossibleRegion().GetSize()[1];

            ImageMaskType::IndexType inputIndex = rois[s].GetIndex();
            ImageMaskType::SizeType  inputSize  = rois[s].GetSize();


            //Image of ones to select slice
            ImageMaskType::Pointer ones = ImageMaskType::New();
            ones->SetRegions(imageMasks[s]->GetLargestPossibleRegion());
            ones->Allocate();
            ones->FillBuffer(1.0);

            ones->SetOrigin(imageMasks[s]->GetOrigin());
            ones->SetSpacing(imageMasks[s]->GetSpacing());
            ones->SetDirection(imageMasks[s]->GetDirection());

            //TODO: Can we parallelize this ?
            //Iteration over the slices of the LR images

            unsigned int i=inputIndex[2] + inputSize[2];

            //Loop over images of the current stack
            for ( unsigned int i=inputIndex[2]; i < inputIndex[2] + inputSize[2]; i++ )
            {
                //std::cout << "process image # " << s << " slice #" << i << std::endl;



                ImageMaskType::RegionType wholeSliceRegion;
                wholeSliceRegion = rois[s];

                ImageMaskType::IndexType  wholeSliceRegionIndex = wholeSliceRegion.GetIndex();
                ImageMaskType::SizeType   wholeSliceRegionSize  = wholeSliceRegion.GetSize();

                wholeSliceRegionIndex[2]= i;
                wholeSliceRegionSize[2] = 1;

                wholeSliceRegion.SetIndex(wholeSliceRegionIndex);
                wholeSliceRegion.SetSize(wholeSliceRegionSize);

                //Extract slice
                ExtractImageMaskFilterType::Pointer sliceExtractor = ExtractImageMaskFilterType::New();
                sliceExtractor->SetExtractionRegion(wholeSliceRegion);
                sliceExtractor->SetInput(ones.GetPointer());
#if ITK_VERSION_MAJOR >= 4
                sliceExtractor->SetDirectionCollapseToIdentity(); // This is required.
#endif
                sliceExtractor->Update();

                ResamplerImageMaskFilterType::Pointer sliceResampler = ResamplerImageMaskFilterType::New();
                sliceResampler -> SetInput(sliceExtractor->GetOutput());
                sliceResampler -> SetTransform(invTransformsArray[s][i-inputIndex[2]]);
                sliceResampler -> SetInterpolator(nnInterpolator);
                sliceResampler -> SetOutputParametersFromImage(outImageMasks[s]);
                sliceResampler -> Update();

                StructuringElementType structuringElementDil;
                structuringElementDil.SetRadius( radiusDilation2D );
                structuringElementDil.CreateStructuringElement();

                MultiplyImageMaskFilterType::Pointer multiplyMaskFilter = MultiplyImageMaskFilterType::New();
                multiplyMaskFilter -> SetInput1(outImageMask.GetPointer());
                //multiplyMaskFilter -> SetInput1(dilateFilter->GetOutput());
                multiplyMaskFilter -> SetInput2(sliceResampler->GetOutput());
                multiplyMaskFilter -> Update();

                ResamplerImageMaskFilterType::Pointer sliceResamplerBack = ResamplerImageMaskFilterType::New();
                sliceResamplerBack -> SetInput(multiplyMaskFilter->GetOutput());
                //sliceResamplerBack -> SetInput(closingFilter->GetOutput());
                sliceResamplerBack -> SetTransform(transformsArray[s][i-inputIndex[2]]);
                sliceResamplerBack -> SetInterpolator(nnInterpolator);
                sliceResamplerBack -> SetOutputParametersFromImage(outLRImageMasks[s]);
                sliceResamplerBack -> Update();


                BinaryDilateImageFilterType::Pointer dilateFilter = BinaryDilateImageFilterType::New();
                dilateFilter->SetInput(sliceResamplerBack->GetOutput());
                dilateFilter->SetKernel(structuringElementDil);
                dilateFilter->SetForegroundValue(1.0);
                dilateFilter->Update();

                AddImageMaskFilter::Pointer addLRMaskFilter = AddImageMaskFilter::New();
                addLRMaskFilter -> SetInput1(outLRImageMasks[s]);
                addLRMaskFilter -> SetInput2(dilateFilter->GetOutput());
                addLRMaskFilter -> Update();

                outLRImageMasks[s] = addLRMaskFilter->GetOutput();
            }

            //std::stringstream ssFileLR;
            //ssFileLR << "/home/ch176971/Desktop/DbgMasks/lrRefMask_" << s << ".nii.gz";

            MaskWriterType::Pointer lrWriter = MaskWriterType::New();
            lrWriter -> SetInput(outLRImageMasks[s]);
            lrWriter -> SetFileName(outputLRFiles[s].c_str());
            try
			{
			lrWriter -> Update();
			}
			catch (itk::ExceptionObject &err)
			{
				throw "error writing outputLRFile" + std::string(outputLRFiles[s].c_str());
				return false;
			}
        }

        StructuringElementType structuringElementDilHR;
        structuringElementDilHR.SetRadius( radiusDilation3D );
        structuringElementDilHR.CreateStructuringElement();

        BinaryDilateImageFilterType::Pointer dilateFilterHRmask = BinaryDilateImageFilterType::New();
        dilateFilterHRmask -> SetInput( outImageMask.GetPointer() );
        dilateFilterHRmask -> SetKernel( structuringElementDilHR );
        dilateFilterHRmask -> SetForegroundValue(1.0);
        dilateFilterHRmask -> Update();

        //

        //std::cout << "h1" << std::endl;
        //end_time_unix = mialtk::getTime();;
        //std::cout << "h2" << std::endl;
        //diff_time_unix = end_time_unix - start_time_unix;

        //mialtk::printTime("TV (IGD)",diff_time_unix);


        // Write image
        //TODO Mask type!!!!!
		if (std::strncmp(outputHRFile, "", 4) != 0)
		{
        MaskWriterType::Pointer maskWriter =  MaskWriterType::New();
        maskWriter -> SetFileName(outputHRFile);
        //writer -> SetInput( resampler -> GetOutput() );
        maskWriter -> SetInput( dilateFilterHRmask -> GetOutput() );


            try
			{
			maskWriter->Update();
			}
			catch (itk::ExceptionObject &err)
			{
				throw "error writing outputHRFile" + std::string(outputHRFile);
				return false;
			}

        }

    return true;
}
