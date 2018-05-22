#ifndef mialsrtkSliceBySliceN4BiasFieldCorrectionWrapper_H
#define mialsrtkSliceBySliceN4BiasFieldCorrectionWrapper_H

#include <string>

# define mialsrtkSliceBySliceN4BiasFieldCorrectionWrapper_EXPORT  __declspec(dllexport)

class SliceBySliceN4BiasFieldCorrection;

class mialsrtkSliceBySliceN4BiasFieldCorrectionWrapper_EXPORT mialsrtkSliceBySliceN4BiasFieldCorrectionWrapper {

public:

	//! default constructor
	mialsrtkSliceBySliceN4BiasFieldCorrectionWrapper(const char* const _inputImageFile, const char* const _inputMaskFile, const char* const _outputImageFile, const char* const _outputBiasFile);

	//second constructor without the outputBiasFile ?
	//mialsrtkSliceBySliceN4BiasFieldCorrectionWrapper(const char* const _inputFile, const char* const _outputFile, const char* const _outputImageFile);


	//! default destructor
	~mialsrtkSliceBySliceN4BiasFieldCorrectionWrapper();

	const char* inputImageFile;
	const char* inputMaskFile;
	const char* outputImageFile;
	const char* outputBiasFile;


	bool runSliceBySliceN4BiasFieldCorrection();
	bool boolExit;

private:
	SliceBySliceN4BiasFieldCorrection* wrap_SliceBySliceN4BiasFieldCorrection;

};


#endif mialsrtkSliceBySliceN4BiasFieldCorrectionWrapper_H