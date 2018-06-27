#ifndef mialsrtkTVSuperResolutionWrapper_H
#define mialsrtkTVSuperResolutionWrapper_H

#include <string>
#include <vector>

# define mialsrtkTVSuperResolutionWrapper_EXPORT  __declspec(dllexport)



class mialsrtkTVSuperResolution;


class mialsrtkTVSuperResolutionWrapper_EXPORT mialsrtkTVSuperResolutionWrapper
{
public:
	mialsrtkTVSuperResolutionWrapper(std::vector< std::string > _inputFile, std::vector< std::string > _maskFile, const char* _outputFile, const char* _refFile,
		std::vector< std::string > _transformin, int _Iter, double _lambda, double _deltat, double _gamma, double _stepScale, double _innerConvThreshold,
		double _outerConvThreshold, int _numberOfLoops, int _numberOfBregmanLoops, bool _boxcar, bool _updateMotion, const char* _refMask,
		std::vector< std::string > _pre_input, std::vector< std::string > _outTransform, const char* _debugDir, bool _debluring, float _kernelRadiusMultiplicator);

	~mialsrtkTVSuperResolutionWrapper();

	std::vector< std::string > inputFile;
	// Input LR masks
	std::vector< std::string > maskFile;
	// Ouput HR image
	const char* outputFile;
	const char* refFile;
	// Input motion parameters - Used only if initHR is disable, meaning that motion parameters were previously estimated
	std::vector< std::string > transformIn;
	int Iter;
	double lambda;
	double deltat;
	double gamma;
	double stepScale;
	double innerConvThreshold;
	double outerConvThreshold;
	int numberOfLoops;
	int numberOfBregmanLoops;
	bool boxcar;
	bool updateMotion;

	const char* refMask;
	std::vector< std::string > pre_input;
	std::vector< std::string > outTransform;																																			  // Output motion parameters -  Only used when motion estimation is updated during SR

	const char* debugDir;

	bool debluring;

	float kernelRadiusMultiplicator;


	bool runTVSuperResolution();
	bool boolExit;

private:
	mialsrtkTVSuperResolution* wrap_TVSuperResolution;


};


#endif mialsrtkTVSuperResolutionWrapper_H

