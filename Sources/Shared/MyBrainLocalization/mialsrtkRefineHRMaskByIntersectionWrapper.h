#ifndef mialsrtkRefineHRMaskByIntersectionWrapper_H
#define mialsrtkRefineHRMaskByIntersectionWrapper_H

#include <string>
#include <vector>

#if defined(_MSC_VER)
//  Microsoft 
#define mialsrtkRefineHRMaskByIntersectionWrapper_EXPORT __declspec(dllexport)

#elif defined(__GNUC__)
//  GCC
#define mialsrtkRefineHRMaskByIntersectionWrapper_EXPORT __attribute__((visibility("default")))
#endif

class mialsrtkRefineHRMaskByIntersection;


class mialsrtkRefineHRMaskByIntersectionWrapper_EXPORT mialsrtkRefineHRMaskByIntersectionWrapper
{
public:
	mialsrtkRefineHRMaskByIntersectionWrapper(std::vector< std::string > _inputFiles, std::vector< std::string > _transformIn, std::vector< std::string > _maskFiles, const char* _outputHRFile,
		std::vector< std::string > _outputLRFiles, const char* _referenceFile, int _radiusDilation, bool _useStaple);

	~mialsrtkRefineHRMaskByIntersectionWrapper();

	std::vector< std::string > inputFiles;
	std::vector< std::string > transformIn;
	std::vector< std::string > maskFiles;
	const char* outputHRFile;
	std::vector< std::string > outputLRFiles;
	const char* referenceFile;
	int radiusDilation;
	bool useStaple;

	bool runRefineMask();
	bool boolExit;

private:
	mialsrtkRefineHRMaskByIntersection* wrap_RefineMask;


};


#endif mialsrtkRefineHRMaskByIntersectionWrapper_H