#include "mialsrtkCorrectSliceIntensityWrapper.h"
#include "mialsrtkCorrectSliceIntensity.h"
#include <tclap/CmdLine.h>


CorrectSliceIntensityWrapper::CorrectSliceIntensityWrapper(const char* const _inputFile, const char* const _maskFile, const char* const _outputFile)
{
	inputFile = _inputFile;
	outputFile = _outputFile;
	maskFile = _maskFile;
	boolExit = false;
}


CorrectSliceIntensityWrapper::~CorrectSliceIntensityWrapper()
{
	inputFile = NULL;
	outputFile = NULL;
	maskFile = NULL;
	boolExit = NULL;

}

bool CorrectSliceIntensityWrapper::SliceIntensityCorrection()
{
	try
	{
		wrap_correctSliceIntensity = new correctSliceIntensity(inputFile, maskFile, outputFile);
		boolExit = wrap_correctSliceIntensity->SliceIntensityCorrection();
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


int mainCorrectSliceIntensityWrapper(int argc, char** argv)
{
	try {
		TCLAP::CmdLine cmd("Correct Slice Intensity", ' ', "1.0", true);
		TCLAP::ValueArg<std::string> inputImageArg("i", "image_file", "input image file (short)", true, "", "string");
		cmd.add(inputImageArg);
		TCLAP::ValueArg<std::string> outputImageArg("o", "output_file", "output image file (short)", true, "", "string");
		cmd.add(outputImageArg);
		TCLAP::ValueArg<std::string> inputMaskArg("m", "mask_file", "filename of the mask image", false, "", "string");
		cmd.add(inputMaskArg);

		cmd.parse(argc, argv);

		std::string input_file = inputImageArg.getValue();
		std::string output_file = outputImageArg.getValue();

		std::string mask_file = inputMaskArg.getValue();

		// Parse the args.

	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
	  std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	  return false;
	}

	CorrectSliceIntensityWrapper test("D:/Pierre/zurich/nifti/patient3/Image0.nii", "D:/Pierre/zurich/nifti/patient3/Image0Mask.nii", "D:/Pierre/zurich/nifti/patient3/Image0Test.nii");
	bool boolOutput = test.SliceIntensityCorrection();
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