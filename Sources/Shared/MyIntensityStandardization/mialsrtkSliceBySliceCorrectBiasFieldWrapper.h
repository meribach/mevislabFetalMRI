#ifndef mialsrtkSliceBySliceCorrectBiasFieldWrapper_H
#define mialsrtkSliceBySliceCorrectBiasFieldWrapper_H

#include <string>

#if defined(_MSC_VER)
    //  Microsoft 
#define mialsrtkSliceBySliceCorrectBiasFieldWrapper_EXPORT __declspec(dllexport)

#elif defined(__GNUC__)
    //  GCC
    #define mialsrtkSliceBySliceCorrectBiasFieldWrapper_EXPORT __attribute__((visibility("default")))
#endif

class SliceBySliceCorrectBiasField;

class mialsrtkSliceBySliceCorrectBiasFieldWrapper_EXPORT mialsrtkSliceBySliceCorrectBiasFieldWrapper {

public:

	//! default constructor
	mialsrtkSliceBySliceCorrectBiasFieldWrapper(const char* const _inputImageFile, const char* const _inputMaskFile, const char* const _inputBiasFieldFile, const char* const _outputImageFile);

	//! default destructor
	~mialsrtkSliceBySliceCorrectBiasFieldWrapper();

	const char* inputImageFile;
	const char* inputMaskFile;
	const char* inputBiasFieldFile;
	const char* outputImageFile;

	bool runSliceBySliceCorrectBiasField();
	bool boolExit;

private:
	SliceBySliceCorrectBiasField* wrap_SliceBySliceCorrectBiasField;

};


#endif mialsrtkSliceBySliceCorrectBiasFieldWrapper_H
