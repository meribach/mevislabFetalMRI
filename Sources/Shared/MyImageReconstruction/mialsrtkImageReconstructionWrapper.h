#ifndef mialImageReconstructionWrapper_H
#define mialImageReconstructionWrapper_H

#include <string>
#include <vector>

# define mialImageReconstructionWrapper_EXPORT  __declspec(dllexport)



class mialImageReconstruction;


class mialImageReconstructionWrapper_EXPORT mialImageReconstructionWrapper
{
public:

  mialImageReconstructionWrapper(std::vector< std::string > _inputFile, const char* const _outputFile, std::vector< std::string > _maskFile, const char* _refFile, std::vector< std::string > _transformout,
	std::vector< std::string > _resampled, const char* const _combinedMasks, unsigned int _itMax, double _epsilon, double _margin, bool _rigid3D, bool _noreg, bool _debluring, bool _boxSwitch,
	bool _maskSwitch, bool _allSwitch, unsigned int _ImageBaseOfRecon, const char* const _MetricToUse, unsigned int _m_Iterations, double _m_GradientMagnitudeTolerance, double _m_MinStepLength,
	double _m_MaxStepLength, double _m_RelaxationFactor);

  ~mialImageReconstructionWrapper();


std::vector< std::string > inputFile;
const char* outputFile;
std::vector< std::string >  maskFile;
const char* refFile;
std::vector< std::string > transformout;
std::vector< std::string > resampled;
const char* combinedMask;
const char* roi;

unsigned int itMax;
double epsilon;
double margin;
bool rigid3D;
bool noreg;
bool debluring;

bool boxSwitch;
bool maskSwitch;
bool allSwitch;
unsigned int ImageBaseOfRecon;
const char* MetricToUse;

unsigned int m_Iterations;
double m_GradientMagnitudeTolerance;
double m_MinStepLength;
double m_MaxStepLength;
double m_RelaxationFactor;


bool runImageReconstruction();
bool boolExit;


private:
  mialImageReconstruction* wrap_ImageReconstruction;


};


#endif mialImageReconstructionWrapper_H
