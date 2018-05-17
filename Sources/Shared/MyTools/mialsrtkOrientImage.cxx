#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkOrientImageFilter.h>

//#include <tclap/CmdLine.h>

#include "mialsrtkOrientImage.h"


MialOrientImage::MialOrientImage(const char* const _inputFile, const char* const _outputFile, const char* const _orientation)
{
	//std::cout << " mial orient Image construit" << std::endl;
	inputFile = _inputFile;
	outputFile = _outputFile;
	orientation = _orientation;
}

MialOrientImage::~MialOrientImage()
{

	//std::cout << "detruit" << std::endl;
	//inputFile = NULL;
	//outputFile = NULL;
	//orientation = NULL;
}



bool MialOrientImage::reOrient()
{

  //std::cout << "it's here where it fails ?" << std::endl;
  typedef float PixelType;
  typedef itk::Image< PixelType, 3 >  ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;

  //std::cout << "Loading input image " << inputFile << std::endl;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputFile );
  
  //faut que je change tout ca avec des throw et par contre je fais des catchs dans le wrapper ?
  try
    {
	//std::cout << "I try" << std::endl;
    reader->Update();
    }
  catch ( itk::ExceptionObject &err)
    {
    //std::cout << "ExceptionObject caught !" << std::endl;
    //std::cout << err << std::endl;
	//change to non itk exception
	//char *error2pass = "error itk ExceptionObject";
	//strcat(error2pass,inputFile);
	throw inputFile;
	return false;
    }

  typedef itk::OrientImageFilter<ImageType,ImageType> OrientImageFilterType;
  OrientImageFilterType::Pointer orienter = OrientImageFilterType::New();

  orienter->UseImageDirectionOn();

  if (orientation == "axial") {
    //std::cout << "Orientation set to axial" << std::endl;
    orienter->SetDesiredCoordinateOrientationToAxial();
  } else if (orientation == "sagittal") {
    //std::cout << "Orientation set to sagittal" << std::endl;
    orienter->SetDesiredCoordinateOrientationToSagittal();
  } else if (orientation == "coronal") {
    //std::cout << "Orientation set to coronal" << std::endl;
    orienter->SetDesiredCoordinateOrientationToCoronal();
  } else if (orientation == "RIP") {
    //std::cout << "Orientation set to RIP" << std::endl;
    orienter->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RIP);
  } else {
    //std::cerr << "Invalid orientation : " << orientation << std::endl;
    //std::cerr << "Possible choices : axial sagittal coronal RIP" << std::endl;
    //exit(1);
  }

  orienter->SetInput(reader->GetOutput());
  orienter->Update();

  //std::cout << "Writing output image " << outputFile << std::endl;

  typedef itk::ImageFileWriter< ImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outputFile );

  try
    {
    writer->SetInput(orienter->GetOutput());
    writer->UseCompressionOn( );
    writer->Update();
    }
  catch ( itk::ExceptionObject &err)
    {
    //std::cout << "ExceptionObject caught !" << std::endl;
    //std::cout << err << std::endl;
	  throw inputFile; // "error itk ExceptionObject";
	  return false;
    }

  //a la place de return 0 il faudrait mettre une public variable OrientSuccess = 1
  return true;

}
