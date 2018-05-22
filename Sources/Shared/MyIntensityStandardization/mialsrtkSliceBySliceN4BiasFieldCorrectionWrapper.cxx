#include "mialsrtkSliceBySliceN4BiasFieldCorrectionWrapper.h"
#include "mialsrtkSliceBySliceN4BiasFieldCorrection.h"


mialsrtkSliceBySliceN4BiasFieldCorrectionWrapper::mialsrtkSliceBySliceN4BiasFieldCorrectionWrapper(const char* const _inputImageFile, const char* const _inputMaskFile, const char* const _outputImageFile, const char* const _outputBiasFile)
{
	inputImageFile = _inputImageFile;
	inputMaskFile = _inputMaskFile;
	outputImageFile = _outputImageFile;
	outputBiasFile = _outputBiasFile;
	boolExit = false;

}



mialsrtkSliceBySliceN4BiasFieldCorrectionWrapper::~mialsrtkSliceBySliceN4BiasFieldCorrectionWrapper()
{
	inputImageFile = NULL;
	inputMaskFile = NULL;
	outputImageFile = NULL;
	outputBiasFile = NULL;

}


bool mialsrtkSliceBySliceN4BiasFieldCorrectionWrapper::runSliceBySliceN4BiasFieldCorrection()
{

	try
	{
		wrap_SliceBySliceN4BiasFieldCorrection = new SliceBySliceN4BiasFieldCorrection(inputImageFile, inputMaskFile, outputImageFile,outputBiasFile);
		boolExit = wrap_SliceBySliceN4BiasFieldCorrection->runSliceBySliceN4BiasFieldCorrection();
		return boolExit;
	}
	catch (const char* str)
	{
		std::cout << "error caught by the wrapper" << std::endl;
		std::cout << str << std::endl;
		std::cout << inputImageFile << std::endl;
		return false;
		//throw str;
	}

}
