#include "mialsrtkSliceBySliceCorrectBiasFieldWrapper.h"
#include "mialsrtkSliceBySliceCorrectBiasField.h"

mialsrtkSliceBySliceCorrectBiasFieldWrapper::mialsrtkSliceBySliceCorrectBiasFieldWrapper(const char* const _inputImageFile, const char* const _inputMaskFile, const char* const _inputBiasFieldFile, const char* const _outputImageFile)
{
   inputImageFile = _inputImageFile;
   inputMaskFile = _inputMaskFile;
   inputBiasFieldFile = _inputBiasFieldFile;
   outputImageFile = _outputImageFile;
   boolExit = false;

}


mialsrtkSliceBySliceCorrectBiasFieldWrapper::~mialsrtkSliceBySliceCorrectBiasFieldWrapper()
{
   inputImageFile = NULL;
   inputMaskFile = NULL;
   inputBiasFieldFile = NULL;
   outputImageFile = NULL;
}

bool mialsrtkSliceBySliceCorrectBiasFieldWrapper::runSliceBySliceCorrectBiasField()
{
	try
	{
		wrap_SliceBySliceCorrectBiasField = new SliceBySliceCorrectBiasField(inputImageFile, inputMaskFile, inputBiasFieldFile,outputImageFile);
		boolExit = wrap_SliceBySliceCorrectBiasField->applyBiasFieldCorrection();
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
