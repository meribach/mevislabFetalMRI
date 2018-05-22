#include "mialsrtkIntensityStandardization.h"
#include "mialsrtkIntensityStandardizationWrapper.h"
#include <tclap/CmdLine.h>


IntensityStandardizationWrapper::IntensityStandardizationWrapper(std::vector< std::string > & _inputFileNames, std::vector< std::string > & _outputFileNames, float _maxIntensity)
{
     inputFileNames = _inputFileNames;
     outputFileNames = _outputFileNames;
     maxIntensity = _maxIntensity;
}

IntensityStandardizationWrapper::IntensityStandardizationWrapper(std::vector< std::string > & _inputFileNames, std::vector< std::string > & _outputFileNames)
{
     inputFileNames = _inputFileNames;
     outputFileNames = _outputFileNames;
     maxIntensity = 255.0;

}

IntensityStandardizationWrapper::IntensityStandardizationWrapper(std::vector< std::string > & _inputFileNames, float _maxIntensity)
{
     inputFileNames = _inputFileNames;
     //outputFileNames = _outputFileNames;
     //loop over inoutFileNames to define outputFileNames
     for(unsigned int IterIm=0; IterIm< inputFileNames.size(); IterIm++)
     {
       int lastdot= inputFileNames[IterIm].find_last_of(".");
       if (lastdot != std::string::npos)
       {
         outputFileNames.push_back(inputFileNames[IterIm].substr(0,lastdot) + "_res.nii");
       }
       else
       {
         outputFileNames.push_back(inputFileNames[IterIm] + "_res.nii");
       }
     }
     maxIntensity = _maxIntensity;

}

IntensityStandardizationWrapper::IntensityStandardizationWrapper(std::vector< std::string > & _inputFileNames)
{

     inputFileNames = _inputFileNames;
     //outputFileNames = _outputFileNames;
     //loop over inoutFileNames to define outputFileNames
     for(unsigned int IterIm=0; IterIm< inputFileNames.size(); IterIm++)
     {
       int lastdot= inputFileNames[IterIm].find_last_of(".");
       if (lastdot != std::string::npos)
       {
         outputFileNames.push_back(inputFileNames[IterIm].substr(0,lastdot) + "_res.nii");
       }
       else
       {
         outputFileNames.push_back(inputFileNames[IterIm] + "_res.nii");
       }
     }
     maxIntensity = 255.0;

}

IntensityStandardizationWrapper::~IntensityStandardizationWrapper()
{
     inputFileNames.clear();
     outputFileNames.clear();
     maxIntensity= NULL;

}


bool IntensityStandardizationWrapper::runIntensityStandardization()
{
	try
	{
		wrap_IntensityStandardization = new IntensityStandardization(inputFileNames, outputFileNames, maxIntensity);
		boolExit = wrap_IntensityStandardization->runIntensityStandardization();
		return boolExit;
	}
	catch (const char* str)
	{
		std::cout << "error caught by the wrapper" << std::endl;
		std::cout << str << std::endl;
		//std::cout << inputFile << std::endl;
		return false;
		//throw str;
	}

}


int mainIntensityStandardization(int argc, char** argv)
{
	std::vector< std::string > inputFileNames;
	std::vector< std::string > outputFileNames;
	float maxIntensity;
  try
  {
  TCLAP::CmdLine cmd("Intensity standardization", ' ', "Unversioned");

  TCLAP::MultiArg<std::string> inputArg("i","input","input image file",true,"string",cmd);
  TCLAP::MultiArg<std::string> outputArg("o","output","output image file",true,"string",cmd);
  TCLAP::ValueArg<float> maxArg  ("","max","max intensity (255 by default)",false,255.0,"float",cmd);

  // Parse the argv array.
  cmd.parse( argc, argv );
  inputFileNames = inputArg.getValue();
  outputFileNames = outputArg.getValue();
  maxIntensity = maxArg.getValue();
  }
  catch (TCLAP::ArgException &e) // catch any exceptions
  {
   std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
   return false;
  }

  IntensityStandardizationWrapper test(inputFileNames, outputFileNames, maxIntensity);
  bool boolOutput = test.runIntensityStandardization();
  std::cout << boolOutput << std::endl;
  if (test.boolExit)
  {
	  std::cout << "it works " << test.boolExit << std::endl;
  }
  else
  {
	  std::cout << "it didn't work " << test.boolExit << std::endl;
  }
  return true;
 };
