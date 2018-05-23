#ifndef mialsrtkIntensityStandardization_H
#define mialsrtkIntensityStandardization_H

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"

#include "itkStatisticsImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"


#include "itkMultiThreader.h"

#include "vcl_algorithm.h"

//#include "mialsrtkMaths.h"


class IntensityStandardization
{
   public:

		//! default constructor
		IntensityStandardization(std::vector< std::string > & _inputFileNames, std::vector< std::string > & _outputFileNames, float _maxIntensity);
		/*
		//! second constructor without max
		IntensityStandardization(std::vector< std::string > & _inputFileNames, std::vector< std::string > & _outputFileNames);
		//! third constructor wtihout outputFileName
		IntensityStandardization(std::vector< std::string > & _inputFileNames, float _maxIntensity);
		//! Fourth constructor without ouputFileName and max
		IntensityStandardization(std::vector< std::string > & _inputFileNames);
		*/

		//! default destructor
		~IntensityStandardization();

        std::vector< std::string > inputFileNames;
        std::vector< std::string > outputFileNames;
        float maxIntensity;

		bool runIntensityStandardization();
		std::vector<std::string> prompt_start(std::vector< std::string > & _inputFileNames, float _maxIntensity);


};




#endif mialsrtkIntensityStandardization_H
