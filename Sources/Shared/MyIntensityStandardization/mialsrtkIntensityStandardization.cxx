
/*=========================================================================

Program: Rescale image intensity by linear transformation
Language: C++
Date: $Date$
Version: 1.0
Author: Sebastien Tourbier

Copyright (c) 2017 Medical Image Analysis Laboratory (MIAL), Lausanne
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

=========================================================================*/
/* Standard includes */

#include "mialsrtkIntensityStandardization.h"
#include <omp.h>

IntensityStandardization::IntensityStandardization(std::vector< std::string > & _inputFileNames, std::vector< std::string > & _outputFileNames, float _maxIntensity)
{
        inputFileNames = _inputFileNames;
        outputFileNames = _outputFileNames;
        maxIntensity = _maxIntensity;
}


IntensityStandardization::IntensityStandardization(std::vector< std::string > & _inputFileNames, std::vector< std::string > & _outputFileNames)
{
        inputFileNames = _inputFileNames;
        outputFileNames = _outputFileNames;
        maxIntensity = 255.0;

}

IntensityStandardization::IntensityStandardization(std::vector< std::string > & _inputFileNames, float _maxIntensity)
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

IntensityStandardization::IntensityStandardization(std::vector< std::string > & _inputFileNames)
{
        inputFileNames = _inputFileNames;
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



std::vector<std::string> IntensityStandardization::prompt_start(std::vector< std::string > & _inputFileNames, float _maxIntensity)
{
    unsigned int numberOfImages = inputFileNames.size();

    std::vector< std::string > promptStart;
    promptStart.push_back(" Intensity Standardization Program ");
    promptStart.push_back("Number of images : " + std::to_string(inputFileNames.size()));

    promptStart.push_back("Intensity max : " + std::to_string(maxIntensity));


    for(unsigned int i=0; i < numberOfImages; i++)
    {
        promptStart.push_back(int2str(i) + ":" +inputFileNames[i]);
    }

    return promptStart;

};


bool IntensityStandardization::runIntensityStandardization()
{
  //prompt_start(inputFileNames,maxIntensity);

  //Typedef
  const unsigned int Dimension = 3;
  typedef float PixelType;
  typedef itk::Image< PixelType, Dimension > ImageType;
  typedef ImageType::Pointer ImagePointer;
  typedef ImageType::RegionType RegionType;
  typedef std::vector< RegionType > RegionArrayType;
  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  typedef itk::ImageFileWriter< ImageType > ImageWriterType;
  typedef itk::StatisticsImageFilter<ImageType> StatisticsImageFilterType;
  typedef itk::RescaleIntensityImageFilter< ImageType, ImageType > RescaleFilterType;

  // Number of images being evaluated
  unsigned int numberOfImages = inputFileNames.size();

  //Read reconstructed imagesrue
  std::vector<ImagePointer> images(numberOfImages);
  std::vector<float> maximums(numberOfImages);
  std::vector<float> minimums(numberOfImages);
  float global_max = 0.0;

  //Extract maximum intensity in all images
  //omp_set_num_threads(7);
  //#pragma omp parallel for private(IterIm) schedule(dynamic)
  for(unsigned int IterIm=0; IterIm< numberOfImages; IterIm++)
  {
       ImageReaderType::Pointer imReader = ImageReaderType::New();
       imReader -> SetFileName( inputFileNames[IterIm].c_str() );
       try
       {
       imReader -> Update();
       }
       catch (itk::ExceptionObject &err)
       {
          throw "error opening input files :" + inputFileNames[IterIm];
          return false;

       }
       images[IterIm] = imReader -> GetOutput();

       StatisticsImageFilterType::Pointer statisticsImageFilter = StatisticsImageFilterType::New ();
       statisticsImageFilter->SetInput(images[IterIm]);
       statisticsImageFilter->Update();

       maximums[IterIm] = statisticsImageFilter -> GetMaximum();
       minimums[IterIm] = statisticsImageFilter -> GetMinimum();

       //Save the max intensity in the overall set of images
       if(maximums[IterIm]>global_max)
         global_max = maximums[IterIm];

   }

   //Rescale intensity in all images.
   for(unsigned int IterIm=0; IterIm< numberOfImages; IterIm++)
   {
       float new_max = ( maximums[IterIm] / global_max ) * maxIntensity;
       //std::cout << "Rescale intensity of image # " << int2str(IterIm) << std::endl;
       RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
       rescaleFilter->SetInput(images[IterIm]);
       rescaleFilter->SetOutputMinimum(0);
       rescaleFilter->SetOutputMaximum(new_max);
       rescaleFilter->Update();
       images[IterIm] = rescaleFilter -> GetOutput();
       //std::cout << "Old range = [ " << minimums[IterIm] << "," << maximums[IterIm] << "],  New range = [0," << new_max<< "]" << std::endl;
   }

   //Write output images
   for(unsigned int i=0; i< numberOfImages; i++)
   {
        std::string outputFileName = "";

        if(outputFileNames.size() == 0)
        {
          int lastdot= inputFileNames[i].find_last_of(".");

          if (lastdot != std::string::npos)
          {
            outputFileName =  inputFileNames[i].substr(0,lastdot) + "_res.nii";
          }
          else
          {
            outputFileName =  inputFileNames[i] + "_res.nii";
          }
         }
         else
         {
            outputFileName = outputFileNames[i];
         }

         ImageWriterType::Pointer writer = ImageWriterType::New();
         writer -> SetFileName( outputFileName );
         writer -> SetInput( images[i] );
         try
         {
         writer -> Update();
         }
         catch (itk::ExceptionObject &err)
         {
          throw "error writing image" + outputFileName;
          return false;
         }

         //std::cout << "Output Image # " << int2str(i) << " saved as " << outputFileName << std::endl;
    }

    return true;

}
