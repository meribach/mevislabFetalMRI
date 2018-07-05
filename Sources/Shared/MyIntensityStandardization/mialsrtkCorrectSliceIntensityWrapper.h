
#ifndef mialsrtkCorrectSliceIntensityWrapper_H
#define mialsrtkCorrectSliceIntensityWrapper_H

//#include <string>

#if defined(_MSC_VER)
    //  Microsoft 
#define CorrectSliceIntensityWrapper_EXPORT __declspec(dllexport)

#elif defined(__GNUC__)
    //  GCC
    #define CorrectSliceIntensityWrapper_EXPORT __attribute__((visibility("default")))
#endif

class correctSliceIntensity;

class CorrectSliceIntensityWrapper_EXPORT CorrectSliceIntensityWrapper {

  public:
	  //three input parameters. inputfile maskfile et outputfile
	  //only one constructor it doesn't make sens to not have the mask file. can't correct the whole volume, intensity too inhomogeneous outside the fetus
	  //! default constructor
	  CorrectSliceIntensityWrapper(const char* const _inputFile, const char* const _maskFile, const char* const _outputFile);
	  //! default destructor
	  ~CorrectSliceIntensityWrapper();

	  const char* inputFile;
	  const char* outputFile;
	  const char* maskFile;
	  bool SliceIntensityCorrection();
	  bool boolExit;


  private:
	  correctSliceIntensity* wrap_correctSliceIntensity;
};

#endif mialsrtkCorrectSliceIntensityWrapper_H
