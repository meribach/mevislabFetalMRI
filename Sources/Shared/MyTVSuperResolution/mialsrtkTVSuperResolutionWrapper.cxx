#include <tclap/CmdLine.h>
#include "mialsrtkTVSuperResolutionWrapper.h"
#include "mialsrtkTVSuperResolution.h"


mialsrtkTVSuperResolutionWrapper::mialsrtkTVSuperResolutionWrapper(std::vector< std::string > _inputFile, std::vector< std::string > _maskFile, const char* _outputFile, const char* _refFile,
	std::vector< std::string > _transformin, int _Iter, double _lambda, double _deltat, double _gamma, double _stepScale, double _innerConvThreshold,
	double _outerConvThreshold, int _numberOfLoops, int _numberOfBregmanLoops, bool _boxcar, bool _updateMotion, const char* _refMask,
	std::vector< std::string > _pre_input, std::vector< std::string > _outTransform, const char* _debugDir, bool _debluring, float _kernelRadiusMultiplicator,
	const char* const _MetricToUse, unsigned int _m_Iterations, double _m_GradientMagnitudeTolerance, double _m_MinStepLength,double _m_MaxStepLength, double _m_RelaxationFactor)
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

	MetricToUse = _MetricToUse;

	m_Iterations = _m_Iterations;
	m_GradientMagnitudeTolerance = _m_GradientMagnitudeTolerance;
	m_MinStepLength = _m_MinStepLength;
	m_MaxStepLength = _m_MaxStepLength;
	m_RelaxationFactor = _m_RelaxationFactor;

}


mialsrtkTVSuperResolutionWrapper::~mialsrtkTVSuperResolutionWrapper()
{
	inputFile.clear();
	maskFile.clear();
	outputFile = NULL;
	refFile = NULL;
	transformIn.clear();
	Iter = NULL;
	lambda = NULL;
	deltat = NULL;
	gamma = NULL;
	stepScale = NULL;
	innerConvThreshold = NULL;
	outerConvThreshold = NULL;
	numberOfBregmanLoops = NULL;
	numberOfLoops = NULL;
	boxcar = NULL;
	updateMotion = NULL;
	refMask = NULL,
	pre_input.clear();
	outTransform.clear();
	debugDir = NULL;
	debluring = NULL;
	kernelRadiusMultiplicator = NULL;

	MetricToUse = NULL;
	m_Iterations = NULL;
	m_GradientMagnitudeTolerance = NULL;
	m_MinStepLength = NULL;
	m_MaxStepLength = NULL;
	m_RelaxationFactor = NULL;

}

bool mialsrtkTVSuperResolutionWrapper::runTVSuperResolution()
{
	try
	{
		std::cout << "TV Super Resolution Wrapper start" << std::endl;
		std::cout << outputFile << std::endl;
		wrap_TVSuperResolution = new mialsrtkTVSuperResolution(inputFile, maskFile, outputFile, refFile, transformIn, Iter, lambda, deltat, gamma, stepScale, 
			innerConvThreshold,	outerConvThreshold, numberOfLoops, numberOfBregmanLoops, boxcar, updateMotion, refMask, pre_input, outTransform, debugDir,
			debluring, kernelRadiusMultiplicator, MetricToUse, m_Iterations, m_GradientMagnitudeTolerance, m_MinStepLength, m_MaxStepLength, m_RelaxationFactor);
		boolExit = wrap_TVSuperResolution->runTVSuperResolution();
		return boolExit;
	}
	catch (const char* str)
	{
		std::cout << "error caught by the wrapper" << std::endl;
		std::cout << str << std::endl;
		//std::cout << inputFile << std::endl;
		return false;
		//throw str;
	}



}

int main()
{
	std::vector< std::string > inputTest;
	inputTest.push_back("D:/Pierre/FoetusTest/coronal2_worldmatrixModified_lr_reoriented_uni_bcorr.nii.gz");
	inputTest.push_back("D:/Pierre/FoetusTest/coronal1_worldmatrixModified_lr_reoriented_uni_bcorr.nii.gz");
	inputTest.push_back("D:/Pierre/FoetusTest/Axial2_worldmatrixModified_lr_reoriented_uni_bcorr.nii.gz");
	inputTest.push_back("D:/Pierre/FoetusTest/Axial1_worldmatrixModified_lr_reoriented_uni_bcorr.nii.gz");
	inputTest.push_back("D:/Pierre/FoetusTest/sagittal2_worldmatrixModified_lr_reoriented_uni_bcorr.nii.gz");
	inputTest.push_back("D:/Pierre/FoetusTest/sagittal1_worldmatrixModified_lr_reoriented_uni_bcorr.nii.gz");

	const char* refFile = "D:/Pierre/FoetusTest/SDI_debug.nii.gz";
	std::vector< std::string > maskFile;
	maskFile.push_back("D:/Pierre/FoetusTest/coronal2_worldmatrixModified_lr_brain_mask_reoriented.nii.gz");
	maskFile.push_back("D:/Pierre/FoetusTest/coronal1_worldmatrixModified_lr_brain_mask_reoriented.nii.gz");
	maskFile.push_back("D:/Pierre/FoetusTest/Axial2_worldmatrixModified_lr_brain_mask_reoriented.nii.gz");
	maskFile.push_back("D:/Pierre/FoetusTest/Axial1_worldmatrixModified_lr_brain_mask_reoriented.nii.gz");
	maskFile.push_back("D:/Pierre/FoetusTest/sagittal2_worldmatrixModified_lr_brain_mask_reoriented.nii.gz");
	maskFile.push_back("D:/Pierre/FoetusTest/sagittal1_worldmatrixModified_lr_brain_mask_reoriented.nii.gz");

	const char* outputFile = "D:/Pierre/FoetusTest/TV_Test.nii.gz";
	std::vector< std::string > transformIn;
	transformIn.push_back("D:/Pierre/FoetusTest/coronal2_worldmatrixModified_lr_reoriented_transform_6V_1.txt");
	transformIn.push_back("D:/Pierre/FoetusTest/coronal1_worldmatrixModified_lr_reoriented_transform_6V_1.txt");
	transformIn.push_back("D:/Pierre/FoetusTest/ Axial2_worldmatrixModified_lr_reoriented_transform_6V_1.txt");
	transformIn.push_back("D:/Pierre/FoetusTest/Axial1_worldmatrixModified_lr_reoriented_transform_6V_1.txt");
	transformIn.push_back("D:/Pierre/FoetusTest/sagittal2_worldmatrixModified_lr_reoriented_transform_6V_1.txt");
	transformIn.push_back("D:/Pierre/FoetusTest/sagittal1_worldmatrixModified_lr_reoriented_transform_6V_1.txt");

	std::vector< std::string > transformOut;
	std::vector< std::string > pre_input;
	mialsrtkTVSuperResolution test(inputTest, maskFile, outputFile, refFile, transformIn, 50,0.1,1,1,1, 0.0001, 0.0001,10,5,false,false,"", pre_input, transformOut, "", false,1.0, "NC", 2000, 0.0001, 0.0001, 0.2, 0.5);
	bool boolOutput = test.runTVSuperResolution();
	std::cout << boolOutput << std::endl;

}

int EntryPointTVSuperResolution(int argc, char *argv[])
{
	try {

		std::vector< std::string > input;
		std::vector< std::string > pre_input;

		std::vector< std::string > mask;
		std::vector< std::string > transform;
		std::vector< std::string > outTransform;

		double gap = 0.0;

		const char *debugDir = NULL;
		const char *debugfilename = "SR_igd_debug_loop_";

		const char *outImage = NULL;
		const char *refImage = NULL;

		const char *refMask = NULL;

		const char *test = "undefined";

		std::vector< int > x1, y1, z1, x2, y2, z2;

		unsigned int iter;
		float lambda;
		float deltat = 1.0;
		float normD = 12.0;
		float theta_init = 1.0;
		float gamma = 1.0;

		float stepScale = 1.0;
		float tau_init = 1 / sqrt(12.0);
		float sigma_init = 1 / sqrt(12.0);

		double innerConvThreshold;
		double outerConvThreshold;

		int numberOfLoops;
		int numberOfBregmanLoops;

		//double start_time_unix, end_time_unix, diff_time_unix;

		// Parse arguments
		TCLAP::CmdLine cmd("Apply super-resolution algorithm using one or multiple input images.", ' ', "Unversioned");

		// Input LR images
		TCLAP::MultiArg<std::string> inputArg("i", "input", "Low-resolution image file", true, "string", cmd);
		// Input LR masks
		TCLAP::MultiArg<std::string> maskArg("m", "mask", "low-resolution image mask file", false, "string", cmd);
		// Ouput HR image
		TCLAP::ValueArg<std::string> outArg("o", "output", "Super resolution output image", true, "", "string", cmd);

		// Input reconstructed image for initialization
		TCLAP::ValueArg<std::string> refArg("r", "reconstructed", "Reconstructed image for initialization. "
			"Typically the output of btkImageReconstruction is used.", true, "", "string", cmd);
		// Input motion parameters - Used only if initHR is disable, meaning that motion parameters were previously estimated
		TCLAP::MultiArg<std::string> transArg("t", "in-transform", "transform file", false, "string", cmd);

		//Optimization parameters
		TCLAP::ValueArg<int> iterArg("", "iter", "Number of inner iterations (default = 50)", false, 50, "int", cmd);
		TCLAP::ValueArg<float> lambdaArg("", "lambda", "Regularization factor (default = 0.1)", false, 0.1, "float", cmd);
		TCLAP::ValueArg<float> deltatArg("", "deltat", "Parameter deltat (default = 1.0)", false, 1.0, "float", cmd);
		TCLAP::ValueArg<float> gammaArg("", "gamma", "Parameter gamma (default = 1.0)", false, 1.0, "float", cmd);
		TCLAP::ValueArg<float> stepScaleArg("", "step-scale", "Parameter step scale (default = 1.0)", false, 1.0, "float", cmd);
		TCLAP::ValueArg<double> innerLoopThresholdArg("", "inner-thresh", "Inner loop convergence threshold (default = 1e-4)", false, 1e-4, "double", cmd);
		TCLAP::ValueArg<double> outerLoopThresholdArg("", "outer-thresh", "Outer loop convergence threshold (default = 1e-4)", false, 1e-4, "double", cmd);
		TCLAP::ValueArg<int> loopArg("", "loop", "Number of loops (SR/denoising) (default = 5)", false, 5, "int", cmd);
		TCLAP::ValueArg<int> bregmanLoopArg("", "bregman-loop", "Number of Bregman loops (default = 10)", false, 10, "int", cmd);

		TCLAP::SwitchArg  boxcarSwitchArg("", "boxcar", "A boxcar-shaped PSF is assumed as imaging model"
			" (by default a Gaussian-shaped PSF is employed.).", cmd, false);

		// Flag that enables the update of motion parameters during SR
		TCLAP::SwitchArg  updateMotionSwitchArg("", "update-motion", "Flag to enable the update of motion parameters during SR"
			" (by default it is disable.).", cmd, false);

		// Arguments only used when motion estimation is updated during SR
		// Mask of the reconstructed image for initialization
		TCLAP::ValueArg<std::string> refMaskArg("", "mask-reconstructed", "Mask of the reconstructed image for initialization - Only used when motion estimation is updated during SR ", false, "", "string", cmd);
		//Input preprocessed images for the first reconstruction -
		TCLAP::MultiArg<std::string> preInputArg("", "pre-input", "Low-resolution pre-processed image file - Only used when motion estimation is updated during SR", false, "string", cmd); // Used only if initHR is enable
																																															// Output motion parameters -  Only used when motion estimation is updated during SR
		TCLAP::MultiArg<std::string> outTransArg("", "out-transform", "output transform file - Only used when motion estimation is updated during SR", false, "string", cmd);

		TCLAP::ValueArg<std::string>debugDirArg("", "debug", "Directory where  SR reconstructed image at each outer loop of the reconstruction optimization is saved", false, "", "string", cmd);

		// Flag that set deblurring PSF during SR
		TCLAP::SwitchArg  debluringArg("", "debluring", "Flag to set deblurring PSF during SR (double the neighborhood)"
			" (by default it is disable.).", cmd, false);


		// Parse the argv array.
		cmd.parse(argc, argv);

		input = inputArg.getValue();
		mask = maskArg.getValue();
		outImage = outArg.getValue().c_str();

		refImage = refArg.getValue().c_str();
		transform = transArg.getValue();

		refMask = refMaskArg.getValue().c_str();
		pre_input = preInputArg.getValue();
		outTransform = outTransArg.getValue();

		iter = iterArg.getValue();
		lambda = lambdaArg.getValue();
		deltat = deltatArg.getValue();
		gamma = gammaArg.getValue();
		innerConvThreshold = innerLoopThresholdArg.getValue();
		outerConvThreshold = outerLoopThresholdArg.getValue();
		numberOfLoops = loopArg.getValue();
		numberOfBregmanLoops = bregmanLoopArg.getValue();
		stepScale = stepScaleArg.getValue();

		}
		catch (TCLAP::ArgException &e)  // catch any exceptions
		{
				std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
		}


}