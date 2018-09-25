#ifndef mialsrtkCorrectBiasFieldWithMotionAppliedWrapper_H
#define mialsrtkCorrectBiasFieldWithMotionAppliedWrapper_H



#if defined(_MSC_VER)
//  Microsoft 
#define mialsrtkCorrectBiasFieldWithMotionAppliedWrapper_EXPORT __declspec(dllexport)

#elif defined(__GNUC__)
//  GCC
#define mialsrtkSliceBySliceCorrectBiasFieldWrapper_EXPORT __attribute__((visibility("default")))
#endif

class mialsrtkCorrectBiasFieldWithMotionApplied;

class mialsrtkCorrectBiasFieldWithMotionAppliedWrapper_EXPORT mialsrtkCorrectBiasFieldWithMotionAppliedWrapper {

public:

	//! default constructor
	mialsrtkCorrectBiasFieldWithMotionAppliedWrapper(const char* _inputFileName, const char* _maskFileName, const char* _transformFileName, const char* _inBiasFieldFileName, const char* _outImageFileName, const char* _outBiasFieldFileName);

	//! default destructor
	~mialsrtkCorrectBiasFieldWithMotionAppliedWrapper();

	const char* inputFileName;
	const char* maskFileName;
	const char* transformFileName;
	const char* inBiasFieldFileName;
	const char* outImageFileName;
	const char* outBiasFieldFileName;

	bool runCorrectBiasFieldWithMotionApplied();
	bool boolExit;

private:
	mialsrtkCorrectBiasFieldWithMotionApplied* wrap_mialsrtkCorrectBiasFieldWithMotionApplied;

};


#endif mialsrtkCorrectBiasFieldWithMotionAppliedWrapper_H
