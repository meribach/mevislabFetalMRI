
#ifndef mialsrtkIntensityStandardizationWrapper_H
#define mialsrtkIntensityStandardizationWrapper_H


#if defined(_MSC_VER)
    //  Microsoft 
#define IntensityStandardizationWrapper_EXPORT __declspec(dllexport)

#elif defined(__GNUC__)
    //  GCC
    #define IntensityStandardizationWrapper_EXPORT __attribute__((visibility("default")))
#endif

class IntensityStandardization;


class IntensityStandardizationWrapper_EXPORT IntensityStandardizationWrapper
{
  public:
    IntensityStandardizationWrapper(std::vector< std::string > & _inputFileNames, std::vector< std::string > & _outputFileNames, float _maxIntensity);
    IntensityStandardizationWrapper(std::vector< std::string > & _inputFileNames, std::vector< std::string > & _outputFileNames);
    IntensityStandardizationWrapper(std::vector< std::string > & _inputFileNames, float _maxIntensity);
    IntensityStandardizationWrapper(std::vector< std::string > & _inputFileNames);
    ~IntensityStandardizationWrapper();


    std::vector< std::string > inputFileNames;
    std::vector< std::string > outputFileNames;
    float maxIntensity;

	bool runIntensityStandardization();
	bool boolExit;

  private:

    IntensityStandardization* wrap_IntensityStandardization;

};





#endif mialsrtkIntensityStandardizationWrapper_H
