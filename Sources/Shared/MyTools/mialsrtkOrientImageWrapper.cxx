#include "mialsrtkOrientImageWrapper.h"
#include <string>
#include <iostream>
#include "mialsrtkOrientImage.h"

bool reOrientWrapper::reOrient()
{
	//std::cout << "I use the reOrientWrapper " << std::endl;
	//std::cout << inputFile << outputFile << orientation << std::endl;
	try
	{
		wrap_MialOrientImage = new MialOrientImage(inputFile, outputFile, orientation);
		boolExit = wrap_MialOrientImage->reOrient();
		return boolExit;
	}
	catch(const char* str)
	{
		std::cout << "error cought by the wrapper" << std::endl;
		std::cout << inputFile << std::endl;
		return false;
		//throw str;
	}
}

reOrientWrapper::reOrientWrapper(const char* const _inputFile, const char* const _outputFile, const char* const _orientation)
{
	//std::cout << "wrapper construit" << std::endl;
	inputFile = _inputFile;
	outputFile=_outputFile;
	orientation=_orientation;
	boolExit = false;
}

reOrientWrapper::~reOrientWrapper()
{
	//std::cout << "detruit" << std::endl;
	inputFile = NULL;
	outputFile = NULL;
	orientation = NULL;
	boolExit = NULL;
}

int main()
{
	//here I add the TCLAP ?
	
	reOrientWrapper test("D:/Pierre/zurich/nifti/patient3/Image0.nii", "D:/Pierre/zurich/nifti/patient3/Image0Test.nii", "axial");
	//std::cout << test.inputFile << std::endl;
	bool boolOutput=test.reOrient();
	//std::cout << test.inputFile << std::endl;
	if (test.boolExit)
	{
	std::cout << "it works "<< test.boolExit << std::endl;
	}
	else
	{
	std::cout << "it didn't work " << test.boolExit << std::endl;
	}
	//return 0;
}



