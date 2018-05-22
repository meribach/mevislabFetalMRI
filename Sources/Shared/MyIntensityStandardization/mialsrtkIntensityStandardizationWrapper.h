
#ifndef mialsrtkIntensityStandardizationWrapper_H
#define mialsrtkIntensityStandardizationWrapper_H


# define IntensityStandardizationWrapper_EXPORT  __declspec(dllexport)

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
