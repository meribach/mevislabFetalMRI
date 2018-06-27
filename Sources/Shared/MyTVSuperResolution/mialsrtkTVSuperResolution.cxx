
/*=========================================================================

Program: Performs super-resolution image reconstruction based on exact Total Variation regularization
Language: C++
Date: $Date: 2015-30-09 $
Version: $Revision: 1 $
Author: $Sebastien Tourbier$

Copyright (c) 2017 Medical Image Analysis Laboratory (MIAL), Lausanne
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
==========================================================================*/


/*#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
*/

#include "mialsrtkTVSuperResolution.h"
/* Standard includes */
#include <sstream>  

#include <iostream>
#include <fstream> 
#include <string>
#include <stdlib.h> 

/* Itk includes */
#include "itkEuler3DTransform.h"
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

/*Btk includes*/
#include "mialsrtkSliceBySliceTransform.h"
//#include "btkSuperResolutionImageFilter.h"
#include "mialsrtkSuperResolutionRigidImageFilterWithImplicitGradientDescent.h"
#include "mialsrtkImageRegistrationFilter.h" //pour le moment c'est uniquement la NC dedans

#include "mialsrtkLowToHighImageResolutionMethod.h"

#include "itkImageDuplicator.h"

//#include "mialsrtkMaths.h"


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


mialsrtkTVSuperResolution::mialsrtkTVSuperResolution(std::vector< std::string > _inputFile, std::vector< std::string > _maskFile, const char* _outputFile, const char* _refFile,
	std::vector< std::string > _transformin, int _Iter, double _lambda, double _deltat, double _gamma, double _stepScale, double _innerConvThreshold,
	double _outerConvThreshold, int _numberOfLoops, int _numberOfBregmanLoops, bool _boxcar, bool _updateMotion, const char* _refMask,
	std::vector< std::string > _pre_input, std::vector< std::string > _outTransform, const char* _debugDir, bool _debluring, float _kernelRadiusMultiplicator)
{
	inputFile = _inputFile;
	maskFile = _maskFile;
	outputFile = _outputFile;
	refFile = _refFile;
	transformIn = _transformin;
	Iter = _Iter;
	lambda = _lambda;
	deltat = _deltat;
	gamma = _gamma;
	stepScale = _stepScale;
	innerConvThreshold = _innerConvThreshold;
	outerConvThreshold = _outerConvThreshold;
	numberOfBregmanLoops = _numberOfBregmanLoops;
	numberOfLoops = _numberOfLoops;
	boxcar = _boxcar;
	updateMotion = _updateMotion;
	refMask = _refMask,
	pre_input = _pre_input;
	outTransform = _outTransform;
	debugDir = _debugDir;
	debluring = _debluring;
	kernelRadiusMultiplicator = _kernelRadiusMultiplicator;

}

mialsrtkTVSuperResolution::~mialsrtkTVSuperResolution()
{

}


bool mialsrtkTVSuperResolution::runTVSuperResolution()
{

        double gap = 0.0;

        const char *debugfilename = "SR_igd_debug_loop_";

        const char *outImage = NULL;
        const char *refImage = NULL;

        const char *refMask = NULL;

        const char *test = "undefined";

        std::vector< int > x1, y1, z1, x2, y2, z2;

        unsigned int iter;
        float lambda;
        float deltat = 1.0;
        float normD = 12.0; //due to Image dimension ?
        float theta_init = 1.0;
        float gamma = 1.0;

        float stepScale = 1.0;
        float tau_init = 1 / sqrt (12.0);
        float sigma_init = 1 / sqrt(12.0);

        double innerConvThreshold;
        double outerConvThreshold;

        int numberOfLoops;
        int numberOfBregmanLoops;

        double start_time_unix, end_time_unix, diff_time_unix;

        tau_init =  stepScale * tau_init;
        sigma_init = ( 1 / stepScale ) * sigma_init;

        if (( std::strncmp(refMask, "", 4) == 0 || pre_input.size() == 0 ) && updateMotion)
        {
		    throw "Execution abandonned - Motion Update during SR is enable but some required input are missing";
            return false;
        }


        // typedefs
        const   unsigned int    Dimension = 3;
        typedef mialsrtk::SliceBySliceTransformBase< double, Dimension > TransformBaseType;
        typedef mialsrtk::SliceBySliceTransform< double, Dimension > TransformType;
        typedef TransformType::Pointer                          TransformPointer;

        // Register the SliceBySlice transform (a non-default ITK transform) with the TransformFactory of ITK
        itk::TransformFactory<TransformType>::RegisterTransform();

        typedef float  PixelType;

        typedef itk::Image< PixelType, Dimension >  ImageType;
        typedef ImageType::Pointer                  ImagePointer;
        typedef std::vector<ImagePointer>           ImagePointerArray;

        typedef itk::Image< unsigned char, Dimension >  ImageMaskType;
        typedef itk::ImageFileReader< ImageMaskType > MaskReaderType;
        typedef itk::ImageMaskSpatialObject< Dimension > MaskType;

        typedef ImageType::RegionType               RegionType;
        typedef std::vector< RegionType >           RegionArrayType;

        typedef itk::ImageFileReader< ImageType >   ImageReaderType;
        typedef itk::ImageFileWriter< ImageType >   WriterType;

        typedef itk::TransformFileReader     TransformReaderType;
        typedef TransformReaderType::TransformListType * TransformListType;

        /* Registration type required in case of slice by slice transformations
  A rigid transformation is employed because there is not distortions like
  in diffusion imaging. We have performed a comparison of accuracy between
  both types of transformations. */
        typedef mialsrtk::SliceBySliceRigidRegistration<ImageType> RegistrationType;
        typedef RegistrationType::Pointer RegistrationPointer;

        /*// Rigid 3D transform definition (typically for reconstructions in adults)
        typedef itk::VersorRigid3DTransform< double > Rigid3DTransformType;
        typedef Rigid3DTransformType::Pointer   Rigid3DTransformPointer;
		*/

        typedef itk::Euler3DTransform< double > EulerTransformType;

        // Resampler type required in case of a slice by slice transform
        typedef mialsrtk::ResampleImageByInjectionFilter< ImageType, ImageType >  ResamplerByInjectionType;

        // Registration Metric
        typedef itk::NormalizedCorrelationImageToImageMetric< ImageType,ImageType > NCMetricType;

		typedef itk::MattesMutualInformationImageToImageMetric<ImageType, ImageType > MIMetricType;

        //typedef btk::ImageIntersectionCalculator<ImageType> IntersectionCalculatorType;
        //IntersectionCalculatorType::Pointer intersectionCalculator = IntersectionCalculatorType::New();

        // Interpolator used to compute the error metric between 2 registration iterations
        //typedef itk::LinearInterpolateImageFunction<ImageType,double>     InterpolatorType;
        typedef itk::BSplineInterpolateImageFunction<ImageType,double>     InterpolatorType;

        //typedef itk::CastImageFilter<ImageType,ImageMaskType> CasterType;

        // A helper class which creates an image which is perfect copy of the input image
        typedef itk::ImageDuplicator<ImageType> DuplicatorType;

        // Super resolution filter that solves the inverse problem
        typedef mialsrtk::SuperResolutionRigidImageFilterWithImplicitGradientDescent< ImageType, ImageType >  ResamplerType;
        ResamplerType::Pointer resampler = ResamplerType::New();

        typedef itk::OrientImageFilter<ImageType,ImageType> OrientImageFilterType;
        typedef itk::OrientImageFilter<ImageMaskType,ImageMaskType> OrientImageMaskFilterType;



        unsigned int numberOfImages = inputFile.size();

        std::vector<OrientImageFilterType::Pointer> orientImageFilter(numberOfImages);
        std::vector<OrientImageMaskFilterType::Pointer> orientMaskImageFilter(numberOfImages);

        std::vector< ImagePointer >         preImages(numberOfImages);
        std::vector< ImageMaskType::Pointer >     imageMasks(numberOfImages);
        std::vector< TransformPointer >     transforms(numberOfImages);
        std::vector< RegistrationPointer >  registration(numberOfImages);

        std::vector<MaskType::Pointer> masks(numberOfImages);


        std::vector< RegionType >           rois(numberOfImages);

        ImageType::IndexType  roiIndex;
        ImageType::SizeType   roiSize;

        // Filter setup
        for (unsigned int i=0; i<numberOfImages; i++)
        {
            // add image
            //std::cout<<"Reading image : "<< inputFile[i].c_str()<<std::endl;
            ImageReaderType::Pointer imageReader = ImageReaderType::New();
            imageReader -> SetFileName(inputFile[i].c_str() );
			try {
				imageReader->Update();
			}
			catch (itk::ExceptionObject &err)
			{
				throw "error opening inputFile";
				return false;
			}
            resampler -> AddInput(  imageReader -> GetOutput() );

            /*
            orientImageFilter[i] = OrientImageFilterType::New();
            orientImageFilter[i] -> UseImageDirectionOn();
            orientImageFilter[i] -> SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
            orientImageFilter[i] -> SetInput(imageReader -> GetOutput());
            orientImageFilter[i] -> Update();

            resampler -> AddInput( orientImageFilter[i] -> GetOutput() );
            */

            //registrationFilter -> AddImage(imageReader -> GetOutput());

            // add region
            if (maskFile.size() > 0 )
            {
                //std::cout<<"Reading mask image : "<<mask[i].c_str()<<std::endl;
                MaskReaderType::Pointer maskReader = MaskReaderType::New();
                maskReader -> SetFileName(maskFile[i].c_str() );
				try {
					maskReader->Update();
				}
				catch (itk::ExceptionObject &err)
				{
					throw "error opening maskFile";
					return false;
				}

                imageMasks[i] = maskReader  -> GetOutput();

                //MaskType::Pointer mask = MaskType::New();
                masks[i]= MaskType::New();
                masks[i] -> SetImage( imageMasks[i] );

                rois[i] = masks[i] -> GetAxisAlignedBoundingBoxRegion();
                //std::cout << "rois "<< i << " : "<<rois[i]<<std::endl;

                resampler -> AddMask( masks[i] );

                RegionType roi = masks[i] -> GetAxisAlignedBoundingBoxRegion();
                roiIndex = roi.GetIndex();
                roiSize  = roi.GetSize();

            } else
            {
                //std::cout<<"Creating a mask image (entire input image)"<<std::endl;
                roiSize  = imageReader -> GetOutput() -> GetLargestPossibleRegion().GetSize();
                roiIndex = imageReader -> GetOutput() -> GetLargestPossibleRegion().GetIndex();
            }

            RegionType imageRegion;
            imageRegion.SetIndex(roiIndex);
            imageRegion.SetSize (roiSize);
            resampler -> AddRegion( imageRegion );

            if ( pre_input.size() > 0)
            {
                // add image
               // std::cout<<"Reading pre-processed image : "<<pre_input[i].c_str()<<std::endl;
                ImageReaderType::Pointer preImageReader = ImageReaderType::New();
                preImageReader -> SetFileName( pre_input[i].c_str() );
				try
				{
					preImageReader->Update();
				}
				catch (itk::ExceptionObject &err)
				{
					throw "error opening pre_input file";
					return false;
				}
                preImages[i] = preImageReader -> GetOutput();

            }


            if (transformIn.size() > 0 )
            {
                //std::cout<<"Reading transform:"<<transform[i]<<std::endl;
                TransformReaderType::Pointer transformReader = TransformReaderType::New();
                transformReader -> SetFileName(transformIn[i] );
				try
				{
					transformReader->Update();
				}
				catch (itk::ExceptionObject &err)
				{
					throw "error opening transformIn file";
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


                for(unsigned int j=0; j< trans -> GetNumberOfSlices(); j++)
                    resampler -> SetTransform(i, j, trans -> GetSliceTransform(j) ) ;

            }

        }

        // Set the reference image
        //std::cout<<"Reading the reference image : "<<refImage<<std::endl;
        ImageReaderType::Pointer refReader = ImageReaderType::New();
        refReader -> SetFileName(refFile);
		try
		{
			refReader->Update();
		}
		catch (itk::ExceptionObject &err)
		{
			throw "error opening refFile file";
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

        // Set the mask of the reference image if provided
        ImageMaskType::Pointer imageMaskCombination;
        if(std::strncmp(refMask, "", 4) != 0)
        {
            MaskReaderType::Pointer refMaskReader = MaskReaderType::New();
            refMaskReader -> SetFileName( refMask );
			try
			{
				refMaskReader->Update();
			}
			catch (itk::ExceptionObject &err)
			{
				throw "error opening refMask file";
				return false;
			}


            imageMaskCombination = refMaskReader -> GetOutput();

            /*
            OrientImageMaskFilterType::Pointer orientRefMaskImageFilter = OrientImageMaskFilterType::New();
            orientRefMaskImageFilter -> UseImageDirectionOn();
            orientRefMaskImageFilter -> SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
            orientRefMaskImageFilter -> SetInput(refMaskReader -> GetOutput());
            orientRefMaskImageFilter -> Update();

            imageMaskCombination = orientRefMaskImageFilter -> GetOutput()
            */
        }

        //std::cout << "==========================================================================" << std::endl << std::endl;

        std::cout<<"Performing super resolution (TV using IGD) with the following settings: "<<std::endl<<std::endl;
        std::cout<<"# iterations max : "<<iter<<std::endl;
        std::cout<<"# loop max : "<<numberOfLoops<<std::endl<<std::endl;
        std::cout<<"Lambda (reg.) : "<<lambda<<std::endl;
        std::cout<<"Gamma : "<<gamma<<std::endl;
        std::cout<<"Delta t : "<< deltat << std::endl;
        std::cout<<"Convergence threshold (inner) : "<< innerConvThreshold << std::endl;
        std::cout<<"Convergence threshold (outer) : "<< outerConvThreshold << std::endl << std::endl;

        //resampler -> Print(std::cout);


        WriterType::Pointer debugwriter =  WriterType::New();

        /*
  debugwriter -> SetFileName( "/Users/sebastientourbier/Desktop/Patient01/SR/Manual/SR_Patient07_3V_noNLM_BCORR_NORM_after_init_loop.nii.gz" );
  debugwriter -> SetInput( resampler -> GetOutput() );
  debugwriter->Update();
  */

        // Initialization of SR optimization parameters
        double criterion = 0.0;

        float theta = 0.0;
        float sigma = 0.0;
        float tau = 0.0;

        // Registration parameters
        unsigned int itMax=2;
        double epsilon=1e-4;


        if (updateMotion)
        {
            std::cout << "Motion estimation enabled with the following settings: " << std::endl << std::endl;
            std::cout << "# iterations max : " << itMax << std::endl;
            std::cout << "Convergence threshold : " << epsilon << std::endl;
        }
        else
        {
            std::cout << "Motion estimation disabled" << std::endl;
        }

        //std::cout << "==========================================================================" << std::endl << std::endl;


        // Variables used if motion estimation is updated during SR
        ImagePointer hrImageInit;
        ImagePointer hrImageRef;
        ImagePointer hrImage;
        ImagePointer hrImageOld;

        //start_time_unix = mialsrtk::getTime();;

        // Bregman loops
        for (int j = 0; j < numberOfBregmanLoops; j++)
        {
            criterion = 1.0;

            std::cout << "Bregman loop init : "<< j << std::endl<<std::endl;
            std::cout<<"Theta : "<<theta_init<<std::endl;
            std::cout<<"Sigma : "<<sigma_init<<std::endl;
            std::cout<<"Tau : "<<tau_init<<std::endl<<std::endl;

            resampler -> SetCurrentBregmanLoop(j);
            resampler -> SetCurrentOuterIteration(0);

            resampler -> UseReferenceImageOn();

            if (j == 0)
            {
                //std::cout << "Initial HR image set to input image associated with flag -r." << std::endl;
                resampler -> SetReferenceImage( refReader -> GetOutput() );
            }
            else
            {
                //std::cout << "Initial HR image set from previous iteration." << std::endl;
                resampler -> SetReferenceImage( resampler -> GetOutput() );
            }

            resampler -> SetIterations(iter);
            resampler -> SetLambda( lambda );
            resampler -> SetGamma( gamma );
            resampler -> SetSigma( sigma_init );
            resampler -> SetTau( tau_init );
            resampler -> SetTheta( theta_init );
            resampler -> SetDeltat( deltat );

            resampler -> SetConvergenceThreshold( innerConvThreshold );

			//resampler -> SetkernelRadiusMultiplicator(kernelRadiusMultiplicator); ?
            resampler -> SetSliceGap( gap ); //?

            resampler -> SetUseDebluringPSF(debluring);

            /*
    std::cout << "Reference image at loop " << 0 << ": " << std::endl;
    std::cout << "Pointer : " << refReader ->GetOutput() << std::endl;
    std::cout << "Region : " << refReader ->GetOutput() ->GetLargestPossibleRegion() << std::endl;
    */

            //std::cout << "**************************************************************************" << std::endl << std::endl;

            if (boxcar )
                resampler -> SetPSF( ResamplerType::BOXCAR );
            resampler -> Update();

            //std::cout << "**************************************************************************" << std::endl << std::endl;

            theta = theta_init;
            sigma = sigma_init;
            tau = tau_init;

            vnl_vector<float> Z = resampler -> GetZVector();
            //vnl_vector<float> Y = resampler -> GetObservationsY();

            // Outer loops
            for (int i=0; i<numberOfLoops; i++)
            {
                //Motion estimation if enabled
                if(pre_input.size()>0 && updateMotion )
                {
                   // std::cout << "Update motion parameters (Slice by Slice)" << std::endl << std::endl;

                    DuplicatorType::Pointer duplicator = DuplicatorType::New();
                    duplicator->SetInputImage(resampler->GetOutput());
                    duplicator->Update();
                    hrImageRef = duplicator -> GetOutput();
                    hrImageRef -> DisconnectPipeline();

                    int im = numberOfImages;
                    float previousMetric = 0.0;
                    float currentMetric = 0.0;

                    //Iterative slice by slice registration
                    for(unsigned int it=1; it <= itMax; it++)
                    {
                        std::cout << "Iteration " << it << std::endl;// <<std::cout.flush();

                        // Start registration
#pragma omp parallel for private(im) schedule(dynamic)
                        for (im=0; im<numberOfImages; im++)
                        {
                            std::cout << "Registering image " << im << " / "<< numberOfImages <<" ... "; //std::cout.flush();

                            registration[im] = RegistrationType::New();
                            registration[im] -> SetFixedImage( preImages[im] );
                            registration[im] -> SetMovingImage( hrImageRef );
                            registration[im] -> SetImageMask( imageMasks[im] );
                            registration[im] -> SetTransform( transforms[im] );
							//here add the metric to use

                            try
                            {

                                registration[im] -> StartRegistration();

                            }
                            catch ( itk::ExceptionObject & err )
                            {

								throw "error registration[im] start registration";
								//return false;
                                // return EXIT_FAILURE;
                            }

                            transforms[im] = static_cast< TransformType* >(registration[im] -> GetTransform());


                            std::cout << "done. "; //std::cout.flush();

                        }

                        //std::cout << std::endl; //std::cout.flush();

                        // Inject images onto a regular HR grid
                        std::cout << "Injecting images ...  "; //std::cout.flush();
                        ResamplerByInjectionType::Pointer resamplerByInj = ResamplerByInjectionType::New();

                        for (unsigned int p=0; p<numberOfImages; p++)
                        {
                            //std::cout << "Add input " << p << " : " << std::endl;
                            resamplerByInj -> AddInput( preImages[p] );
                            resamplerByInj -> AddRegion( rois[p] );
                            resamplerByInj -> SetTransform(p, transforms[p].GetPointer()) ;
                        }

                        resamplerByInj -> UseReferenceImageOn();
                        resamplerByInj -> SetReferenceImage( hrImageRef );
                        resamplerByInj -> SetReferenceImageMask(imageMaskCombination);
                        resamplerByInj -> Update();

                        if(it > 1)
                            hrImageOld = hrImage;

                        hrImage = resamplerByInj -> GetOutput();

                        std::cout << "done. " << std::endl; //std::cout.flush();

                        // compute error
                        double delta = 0.0;
                        if (it > 1)
                        {
                            EulerTransformType::Pointer identity = EulerTransformType::New();
                            identity -> SetIdentity();

                            InterpolatorType::Pointer interpolator = InterpolatorType::New();

                            NCMetricType::Pointer nc = NCMetricType::New();
                            nc -> SetFixedImage(  hrImageOld );
                            nc -> SetMovingImage( hrImage );
                            nc -> SetFixedImageRegion( hrImageOld -> GetLargestPossibleRegion() );
                            nc -> SetTransform( identity );
                            nc -> SetInterpolator( interpolator );

                            nc -> Initialize();

                            previousMetric = currentMetric;
                            currentMetric = - nc -> GetValue( identity -> GetParameters() );

                            delta = (currentMetric - previousMetric) / previousMetric;

                            std::cout<<"previousMetric: "<<previousMetric<<", currentMetric: "<<currentMetric<< ", delta : " << delta <<std::endl;
                        }
                        else
                        {
                            delta = 1;
                        }

                        if (delta < epsilon) break;

                    }// End of iterative registration

                    // Update transforms
                    for (unsigned int lr=0; lr<numberOfImages; lr++)
                    {
                        for(unsigned int s=0; s< transforms[lr] -> GetNumberOfSlices(); s++)
                            resampler -> SetTransform(lr, s, transforms[lr] -> GetSliceTransform(s) ) ;
                    }

                    //std::cout << "**************************************************************************" << std::endl << std::endl;

                }// End of motion estimation

				//c'est quoi ce gros basard la ....

                resampler -> SetCurrentOuterIteration(i+1);
                std::cout << "Bregman loop : "<< j << " / TV Loop : "<< (resampler -> GetCurrentOuterIteration()) <<std::endl<<std::endl;

                //Update optimization parameters
                theta = resampler -> GetTheta();
                std::cout<<"Theta (outer) = "<<theta<<std::endl;
                resampler -> SetGamma( gamma );

                std::cout << "Sigma old / new = " << sigma << " / ";
                sigma = sigma / theta;
                std::cout<<sigma<<std::endl;
                resampler -> SetSigma( sigma  );

                std::cout << "Tau old / new  = " << tau << " / ";
                tau = theta * tau;
                std::cout<<tau<<std::endl;
                resampler -> SetTau( tau );

                resampler -> SetZVector(Z);

                //resampler -> SetTheta( theta );

                //resampler -> UpdateXest();
                //resampler -> SetXold();

                /*
      std::cout << "Reference image at loop " << i+1 << ": " << std::endl;
      std::cout << "Pointer : " << resampler ->GetOutput() << std::endl;
      std::cout << "Region : " << resampler ->GetOutput() ->GetLargestPossibleRegion() << std::endl;
      */

                std::cout << "New reference size : " << resampler -> GetOutput()->GetLargestPossibleRegion().GetNumberOfPixels() << std::endl;


                resampler -> SetReferenceImage( resampler -> GetOutput() );
                
				try
				{
				resampler -> Update();
				}
				catch (itk::ExceptionObject & err)
				{

					throw "error resampler";
					//return false;
					// return EXIT_FAILURE;
				}
                criterion = resampler -> GetCriterionValue();

                std::cout << std::endl << "Outer loop criterion = " << criterion << std::endl << std::endl;

                //resampler -> Print(std::cout);

				if (strcmp(debugDir, "") != 0)
				{
					std::ostringstream ss;
					
                    #ifdef _WIN32
					ss << debugDir << '\\' << debugfilename << (i+1) << ".nii.gz";
                    #else
					ss << debugDir << "/" << debugfilename << (i + 1) << ".nii.gz";
                    #endif
					

					//std::cout << "##################################################################" << std::endl;
					std::cout << "Debug mode : writing image " << ss.str() << std::endl;
					//std::cout << "##################################################################" << std::endl << std::endl;

					debugwriter->SetFileName(ss.str());
					debugwriter->SetInput(resampler->GetOutput());
					try
					{
						debugwriter->Update();
					}
					catch (itk::ExceptionObject & err)
					{

						throw "error debugWriter";
						//return false;
						// return EXIT_FAILURE;
					}

				}

                if(criterion < outerConvThreshold)
                {
                    std::cout << "Outer loop has converged after "<< resampler -> GetCurrentOuterIteration() <<" iterations! ( last value = " << criterion << " )"<< std::endl;
                    break;
                }

                //std::cout << "**************************************************************************" << std::endl << std::endl;

            }// End of outer loops

            //Update bregman variable
            //resampler -> UpdateZ();

        }//End of bregmann loops

        /*
		 //Save TVEnergy in CSV file
        const char * csvFileName="/home/tourbier/Desktop/NewbornWithGapForConvergence/tv_energy_inf.csv";
        bool writeHeaders = false;

        std::ifstream fin;
        fin.open(csvFileName,std::ios_base::out | std::ios_base::app);

        if(fin.is_open())
        {
            //Test if the file is empty. If so, we add an extra line for headers
            //std::cout << "Test if CSV  is empty. If so, we add an extra line for headers." << std::endl;
            int csvLength;

            fin.seekg(0, std::ios::end);
            csvLength = fin.tellg();

            fin.close();

            if(csvLength == 0)
            {
                writeHeaders = true;
                std::cout << "Write headers in CSV" << std::endl;
            }
            else
            {
                std::cout << "CSV empty ( length : " << int2str(csvLength) << std::endl;
            }


            //NOT WORKING ON MAC
            //if(fin.peek() == std::ifstream::traits_type::eof())
            //{
            //   writeHeaders = true;
            //    std::cout << "Write headers in CSV" << std::endl;
            //}
            //fin.close();
        }
        else
        {
            std::cout << "CSV file opening failed." << std::endl;
        }
		

        std::ofstream fout(csvFileName, std::ios_base::out | std::ios_base::app);

        if(writeHeaders)
        {
            fout << "Date" << "," << "Algo" << "," << "lambda" << "," << "step-scale" << "," << "deltat" << "," << "gamma" << ",";
            fout << "Innerloops" << "," << "InnerThreshold" << "," << "Outerloops" << "," << "OuterThreshold" << ",";
            fout << "TVEnergy";
            fout << std::endl;
        }

        fout << mialsrtk::getRealCurrentDate() << "," << "TV" << "," << lambda << "," << stepScale << "," << deltat << "," << gamma << ",";
        fout << iter << "," << innerConvThreshold << "," << numberOfLoops << "," << outerConvThreshold << ",";

        //Add value of TV energy to CSV
        fout << resampler -> GetTVEnergy();

        fout << std::endl;
        fout.close();

        std::cout << "Metrics saved in CSV" << std::endl;
		*/

        //std::cout << std::endl << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

        //

        //std::cout << "h1" << std::endl;
        //end_time_unix = mialsrtk::getTime();;
        //std::cout << "h2" << std::endl;
        //diff_time_unix = end_time_unix - start_time_unix;

        //mialsrtk::printTime("TV (IGD)",diff_time_unix);

        //double innerLoopRunTime = resampler -> GetInnerOptTime();
        //double initCostFunctionRunTime = resampler -> GetInitTime();

        //mialsrtk::printTime("Initialization for",initCostFunctionRunTime);
        //mialsrtk::printTime("Inner loop",innerLoopRunTime);

        // Write image

        WriterType::Pointer writer =  WriterType::New();
        writer -> SetFileName( outImage );
        writer -> SetInput( resampler -> GetOutput() );
        //writer -> SetInput( outputImage );

        if ( std::strncmp(outImage,"",4) != 0)
        {
            try
			{
			writer->Update();
			}
			catch (itk::ExceptionObject & excp)
			{
				//std::cerr << "Error while saving transform" << std::endl;
				//std::cerr << excp << std::endl;
				//std::cout << "[FAILED]" << std::endl;
				throw "Error while saving outImage";
				return false;
			}
        }

        // Write transforms

        typedef itk::TransformFileWriter TransformWriterType;

        if ( outTransform.size() > 0 && updateMotion )
        {
            for (unsigned int i=0; i<numberOfImages; i++)
            {
                TransformWriterType::Pointer transformWriter = TransformWriterType::New();
                transformWriter -> SetInput( transforms[i] );
                transformWriter -> SetFileName ( outTransform[i].c_str() );

                try
                {
                    //std::cout << "Writing " << outTransform[i].c_str() << " ... " ; std::cout.flush();
                    transformWriter -> Update();
                    //std::cout << " done! " << std::endl;
                }
                catch ( itk::ExceptionObject & excp )
                {
                    //std::cerr << "Error while saving transform" << std::endl;
                    //std::cerr << excp << std::endl;
                    //std::cout << "[FAILED]" << std::endl;
					throw "Error while saving transform";
					return false;
                }

            }
        }

    return true;
}

