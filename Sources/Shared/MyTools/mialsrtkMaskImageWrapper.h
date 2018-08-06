#ifndef mialsrtkMaskImageWrapper_H
#define mialsrtkMaskImageWrapper_H

#if defined(_MSC_VER)
//  Microsoft 
#define mialsrtkMaskImageWrapper_EXPORT __declspec(dllexport)

#elif defined(__GNUC__)
//  GCC
#define mialsrtkMaskImageWrapper_EXPORT __attribute__((visibility("default")))
#endif

class mialsrtkMaskImage;


class mialsrtkMaskImageWrapper_EXPORT mialsrtkMaskImageWrapper

{
public:
	mialsrtkMaskImageWrapper(const char* const _inputFile, const char* const _maskFile, const char* const _outputFile);
	~mialsrtkMaskImageWrapper();

	const char* inputFile;
	const char* maskFile;
	const char* outputFile;
	bool runMaskImage();
	bool boolExit;

private:
	mialsrtkMaskImage* wrap_mialsrtkMaskImage;

};


#endif mialsrtkMaskImageWrapper_H