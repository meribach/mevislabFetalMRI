
#include <tclap/CmdLine.h>
#include "btkNLMDenoisingWrapper.h"
#include "btkNLMDenoising.h"

btkNLMDenoisingWrapper::btkNLMDenoisingWrapper(const char* const _inputFile, const char* const _outputFile, const char* const _maskFile, const char* const _refFile,
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
	boolExit = false;
}

btkNLMDenoisingWrapper::btkNLMDenoisingWrapper(const char* const _inputFile, const char* const _outputFile, const char* const _maskFile)
{
	inputFile = _inputFile;
	outputFile = _outputFile;
	maskFile = _maskFile;
	refFile = "";
	padding = 0;
	hwn = 1;
	hwvs = 5;
	beta = 1;
	block = 1;
	center = -1;
	optimized = 1;
	lowerMeanThreshold = 0.95;
	lowerVarianceThreshold = 0.5;
	differenceFile = "";
	localSmoothing = 0;
	boolExit = false;

}

btkNLMDenoisingWrapper::btkNLMDenoisingWrapper(const char* const _inputFile, const char* const _outputFile)
{
	inputFile = _inputFile;
	outputFile = _outputFile;
	maskFile = "";
	refFile = "";
	padding = 0;
	hwn = 1;
	hwvs = 5;
	beta = 1;
	block = 1;
	center = -1;
	optimized = 1;
	lowerMeanThreshold = 0.95;
	lowerVarianceThreshold = 0.5;
	differenceFile = "";
	localSmoothing = 0;
	boolExit = false;

}


btkNLMDenoisingWrapper::~btkNLMDenoisingWrapper()
{
	inputFile = NULL;
	outputFile = NULL;
	maskFile = NULL;
	refFile = NULL;
	padding = NULL;
	hwn = NULL;
	hwvs = NULL;
	beta = NULL;
	block = NULL;
	center = NULL;
	optimized = NULL;
	lowerMeanThreshold = NULL;
	lowerVarianceThreshold = NULL;
	differenceFile = NULL;
	localSmoothing = NULL;
	boolExit = NULL;
}

bool btkNLMDenoisingWrapper::runDenoise()
{
	try
	{
		/*if (std::strcmp(maskFile, "")==0)
		{
			std::cout << "no mask file" << std::endl;
		}
		else
		{
			std::cout << "mask file" << std::endl;
		}*/

		wrap_btkNLMDenoise = new btkNLMDenoise(inputFile, outputFile, maskFile, refFile, padding, hwn, hwvs, beta, block, center, optimized, lowerMeanThreshold, lowerVarianceThreshold, differenceFile, localSmoothing);
		boolExit = wrap_btkNLMDenoise->runDenoising();
		return boolExit;
	}
	catch (const char* str)
	{
		std::cout << "error caught by the wrapper" << std::endl;
		std::cout << str << std::endl;
		std::cout << inputFile << std::endl;
		return false;
		//throw str;
	}
	
}


int EntryPoint(int argc, char** argv)
{
	try {

		TCLAP::CmdLine cmd("Non-Local mean denoising: implementation of the method proposed by Coupé et al., IEEE TMI 2008 ", ' ', "1.0", true);

		TCLAP::ValueArg<std::string> inputImageArg("i", "image_file", "input image file (short)", true, "", "string");
		cmd.add(inputImageArg);
		TCLAP::ValueArg<std::string> outputImageArg("o", "output_file", "output image file (short)", true, "", "string");
		cmd.add(outputImageArg);
		TCLAP::ValueArg<std::string> inputMaskArg("m", "mask_file", "filename of the mask image", false, "", "string");
		cmd.add(inputMaskArg);
		TCLAP::ValueArg<std::string> inputReferenceArg("r", "ref_file", "filename of the reference image", false, "", "string");
		cmd.add(inputReferenceArg);
		TCLAP::ValueArg< float > paddingArg("p", "pad", "padding value (used if no mask image is provided, default is 0)", false, 0, "float");
		cmd.add(paddingArg);
		TCLAP::ValueArg< int > hwnArg("", "hwn", "patch half size (default is 1)", false, 1, "int");
		cmd.add(hwnArg);
		TCLAP::ValueArg< int > hwvsArg("", "hwvs", "half size of the volume search area, i.e. the spatial bandwidth (default is 5)", false, 5, "int");
		cmd.add(hwvsArg);
		TCLAP::ValueArg< float > betaArg("b", "beta", "beta: smoothing parameter (high beta produces smoother result, default is 1)", false, 1, "float");
		cmd.add(betaArg);
		TCLAP::ValueArg< int > blockArg("", "block", "0: pointwise, 1: blockwise, 2: fast blockwise (default is 1)", false, 1, "int");
		cmd.add(blockArg);
		TCLAP::ValueArg< int > centerArg("c", "center", "weight of the central patch (possible value: 0, 1, -1 (max)) (default is -1)", false, -1, "int");
		cmd.add(centerArg);
		TCLAP::ValueArg< int > optimizedArg("", "opt", "optimized mode (use mean and standard deviation of patches) (0: no, 1: yes) (default is 1)", false, 1, "int");
		cmd.add(optimizedArg);
		TCLAP::ValueArg< float > lowerMeanThresholdArg("", "lmt", "lower mean threshold (0.95 by default) -- for optimized mode only", false, 0.95, "float");
		cmd.add(lowerMeanThresholdArg);
		TCLAP::ValueArg< float > lowerVarianceThresholdArg("", "lvt", "lower variance threshold (0.5 by default) -- for optimized mode only", false, 0.5, "float");
		cmd.add(lowerVarianceThresholdArg);
		TCLAP::ValueArg<std::string> outputDifferenceImageArg("d", "difference_file", "filename of the difference image", false, "", "string");
		cmd.add(outputDifferenceImageArg);
		TCLAP::ValueArg< int > localArg("", "local", "Estimation of the smoothing parameter. 0: global, 1: local (default is 0)", false, 0, "int");
		cmd.add(localArg);


		// Parse the args.
		cmd.parse(argc, argv);

		// Get the value parsed by each arg.
		std::string input_file = inputImageArg.getValue();
		std::string output_file = outputImageArg.getValue();

		std::string mask_file = inputMaskArg.getValue();
		std::string ref_file = inputReferenceArg.getValue();
		float padding = paddingArg.getValue();
		int hwn = hwnArg.getValue();
		int hwvs = hwvsArg.getValue();
		float beta = betaArg.getValue();
		int block = blockArg.getValue();
		int center = centerArg.getValue();
		int optimized = optimizedArg.getValue();
		float lowerMeanThreshold = lowerMeanThresholdArg.getValue();
		float lowerVarianceThreshold = lowerVarianceThresholdArg.getValue();
		std::string difference_file = outputDifferenceImageArg.getValue();
		int localSmoothing = localArg.getValue();

	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
		return false;
	}

	btkNLMDenoisingWrapper test("D:/Pierre/zurich/nifti/patient3/Image0.nii", "D:/Pierre/zurich/nifti/patient3/Image0Test.nii");
	bool boolOutput = test.runDenoise();
	std::cout << boolOutput << std::endl;
	if (test.boolExit)
	{
		std::cout << "it works " << test.boolExit << std::endl;
	}
	else
	{
		std::cout << "it didn't work " << test.boolExit << std::endl;
	}
	return true;

}