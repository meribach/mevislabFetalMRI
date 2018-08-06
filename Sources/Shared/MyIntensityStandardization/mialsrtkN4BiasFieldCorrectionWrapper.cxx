#include "mialsrtkN4BiasFieldCorrectionWrapper.h"
#include "mialsrtkN4BiasFieldCorrection.h

mialsrtkN4BiasFieldCorrectionWrapper::mialsrtkN4BiasFieldCorrectionWrapper(const char* const _inputFile, const char* const _maskFile, const char* const _outputFile, const char* const _outputBiasFile)
{
	inputFile = _inputFile;
	maskFile = _maskFile;
	outputFile = _outputFile;
	outputBiasFile = _outputBiasFile;
	ShrinkFactor = 4;
}


mialsrtkN4BiasFieldCorrectionWrapper::~mialsrtkN4BiasFieldCorrectionWrapper()
{
	inputFile = NULL;
	maskFile = NULL;
	outputFile = NULL;
	outputBiasFile = NULL;
	ShrinkFactor = NULL;
}


bool mialsrtkN4BiasFieldCorrectionWrapper::runN4BiasFieldCorrection()
{
	try
	{
		std::cout << "N4 BiasFieldCorrection start" << std::endl;
		wrap_mialsrtkN4BiasFieldCorrection = new mialsrtkN4BiasFieldCorrection(inputFile, maskFile, outputFile, outputBiasFile);
		boolExit = wrap_mialsrtkN4BiasFieldCorrection->runN4BiasFieldCorrection();
		return boolExit;
	}
	catch (const char* str)
	{
		std::cout << "error caught by the wrapper" << std::endl;
		std::cout << str << std::endl;

		return false;

	}
}