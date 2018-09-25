#include "mialsrtkCorrectBiasFieldWithMotionApplied.h"
#include "mialsrtkCorrectBiasFieldWithMotionAppliedWrapper.h"

#include <tclap/CmdLine.h>

mialsrtkCorrectBiasFieldWithMotionAppliedWrapper::mialsrtkCorrectBiasFieldWithMotionAppliedWrapper(const char* _inputFileName, const char* _maskFileName, const char* _transformFileName, const char* _inBiasFieldFileName, const char* _outImageFileName, const char* _outBiasFieldFileName)
{
	inputFileName = _inputFileName;
	maskFileName = _maskFileName;
	transformFileName = _transformFileName;
	inBiasFieldFileName = _inBiasFieldFileName;
	outImageFileName = _outImageFileName;
	outBiasFieldFileName = _outBiasFieldFileName;
}

mialsrtkCorrectBiasFieldWithMotionAppliedWrapper::~mialsrtkCorrectBiasFieldWithMotionAppliedWrapper()
{
	inputFileName = NULL;
	maskFileName = NULL;
	transformFileName = NULL;
	inBiasFieldFileName = NULL;
	outImageFileName = NULL;
	outBiasFieldFileName = NULL;

}

bool mialsrtkCorrectBiasFieldWithMotionAppliedWrapper::runCorrectBiasFieldWithMotionApplied()
{
	try
	{
		wrap_mialsrtkCorrectBiasFieldWithMotionApplied = new mialsrtkCorrectBiasFieldWithMotionApplied(inputFileName, maskFileName, transformFileName, inBiasFieldFileName, outImageFileName, outBiasFieldFileName);
		boolExit = wrap_mialsrtkCorrectBiasFieldWithMotionApplied->RunBiasCorrectionWithMotionApplied();
		return boolExit;
	}
	catch (const char* str)
	{
		std::cout << "error caught by the wrapper" << std::endl;
		std::cout << str << std::endl;
		std::cout << inputFileName << std::endl;
		return false;
		//throw str;
	}

}


int AnotherEntrypoint(int argc, char *argv[])
{
	const char *inputFileName = NULL;
	const char *maskFileName = NULL;
	const char *transformFileName = NULL;
	const char *inBiasFieldFileName = NULL;

	const char *outImageFileName = NULL;
	const char *outBiasFieldFileName = NULL;

	// Parse arguments

	TCLAP::CmdLine cmd("Register slice of a LR images to a template HR image", ' ', "Unversioned");

	TCLAP::ValueArg<std::string> inputArg("i", "input", "Input image file", true, "", "string", cmd);
	TCLAP::ValueArg<std::string> maskArg("m", "", "Input mask file", true, "", "string", cmd);

	TCLAP::ValueArg<std::string> transformArg("t", "transform", "Transform input file", true, "", "string", cmd);

	TCLAP::ValueArg<std::string> inputBFArg("", "input-bias-field", "Input bias field image file (Typically the bias field globally estimated in the HR reconstructed image)", true, "", "string", cmd);

	TCLAP::ValueArg<std::string> outputArg("o", "output", "Output bias field corrected image file", true, "", "string", cmd);

	TCLAP::ValueArg<std::string> outputBFArg("", "output-bias-field", "Output bias field image file", true, "", "string", cmd);

	// Parse the argv array.
	cmd.parse(argc, argv);

	inputFileName = inputArg.getValue().c_str();
	maskFileName = maskArg.getValue().c_str();
	transformFileName = transformArg.getValue().c_str();

	inBiasFieldFileName = inputBFArg.getValue().c_str();

	outImageFileName = outputArg.getValue().c_str();
	outBiasFieldFileName = outputBFArg.getValue().c_str();

}