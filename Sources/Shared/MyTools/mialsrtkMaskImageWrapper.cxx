
#include "mialsrtkMaskImage.h"
#include "mialsrtkMaskImageWrapper.h"
#include <tclap/CmdLine.h>

mialsrtkMaskImageWrapper::mialsrtkMaskImageWrapper(const char* const _inputFile, const char* const _maskFile, const char* const _outputFile)
{
	inputFile = _inputFile;
	maskFile = _maskFile;
	outputFile = _outputFile;
}

mialsrtkMaskImageWrapper::~mialsrtkMaskImageWrapper()
{
	inputFile = NULL;
	maskFile = NULL;
	outputFile = NULL;
}


bool mialsrtkMaskImageWrapper::runMaskImage()
{
	try
	{
		std::cout << "MaskImage start" << std::endl;
		wrap_mialsrtkMaskImage = new mialsrtkMaskImage(inputFile, maskFile, outputFile);
		boolExit = wrap_mialsrtkMaskImage->runMaskImage();
		return boolExit;
	}
	catch (const char* str)
	{
		std::cout << "error caught by the wrapper" << std::endl;
		std::cout << str << std::endl;

		return false;

	}

}


int anotherEntryPointMaskImage(int argc, char** argv)
{
	try {

		std::string input;
		std::string mask;
		std::string output;

		// Parse arguments
		TCLAP::CmdLine cmd("Apply binary mask to input image and return the output masked image.", ' ', "1.0");

		// Ouput HR image
		TCLAP::ValueArg<std::string> inputArg("i", "input", "Input image file", true, "", "string", cmd);

		// Ouput HR image
		TCLAP::ValueArg<std::string> maskArg("m", "mask", "Input binary mask image file", true, "", "string", cmd);

		// Ouput HR image
		TCLAP::ValueArg<std::string> outputArg("o", "output", "Output masked image", true, "", "string", cmd);

		// Parse the argv array.
		cmd.parse(argc, argv);

		input = inputArg.getValue();
		mask = maskArg.getValue();
		output = outputArg.getValue();

	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		std::cerr << "Error: " << e.error() << " for arg " << e.argId() << std::endl;
	}
}