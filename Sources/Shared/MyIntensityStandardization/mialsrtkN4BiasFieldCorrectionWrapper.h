#ifndef mialsrtkN4BiasFieldCorrectionWrapper_H
#define mialsrtkN4BiasFieldCorrectionWrapper_H

#if defined(_MSC_VER)
//  Microsoft 
#define mialsrtkN4BiasFieldCorrectionWrapper_EXPORT __declspec(dllexport)

#elif defined(__GNUC__)
//  GCC
#define mialsrtkN4BiasFieldCorrectionWrapper_EXPORT __attribute__((visibility("default")))
#endif

class mialsrtkN4BiasFieldCorrection;


class mialsrtkN4BiasFieldCorrectionWrapper_EXPORT mialsrtkN4BiasFieldCorrectionWrapper
{
public:
	mialsrtkN4BiasFieldCorrectionWrapper(const char* const _inputFile, const char* const _maskFile, const char* const _outputFile, const char* const _outputBiasFile);

	~mialsrtkN4BiasFieldCorrectionWrapper();

	const char* inputFile;
	const char* maskFile;
	const char* outputFile;
	const char* outputBiasFile;

	unsigned int ShrinkFactor;
	bool runN4BiasFieldCorrection();
	bool boolExit;

private:
	mialsrtkN4BiasFieldCorrection* wrap_mialsrtkN4BiasFieldCorrection;
};

#endif mialsrtkN4BiasFieldCorrectionWrapper_H