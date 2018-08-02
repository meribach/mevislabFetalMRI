#include <tclap/CmdLine.h>
#include "mialsrtkRefineHRMaskByIntersectionWrapper.h"
#include "mialsrtkRefineHRMaskByIntersection.h"



mialsrtkRefineHRMaskByIntersectionWrapper::mialsrtkRefineHRMaskByIntersectionWrapper(std::vector< std::string > _inputFiles, std::vector< std::string > _transformIn, std::vector< std::string > _maskFiles, const char* _outputHRFile,
	std::vector< std::string > _outputLRFiles, const char* _referenceFile, int _radiusDilation, bool _useStaple)
{
	inputFiles = _inputFiles;
	transformIn = _transformIn;
	maskFiles = _maskFiles;
	outputHRFile = _outputHRFile;
	outputLRFiles = _outputLRFiles;
	referenceFile = _referenceFile;
	radiusDilation = _radiusDilation;
	useStaple = _useStaple;
}


mialsrtkRefineHRMaskByIntersectionWrapper::~mialsrtkRefineHRMaskByIntersectionWrapper()
{

	inputFiles.clear();
	transformIn.clear();
	maskFiles.clear();
	outputHRFile = NULL;
	outputLRFiles.clear();
	referenceFile = NULL;
	radiusDilation = NULL;
	useStaple = NULL;

}


bool mialsrtkRefineHRMaskByIntersectionWrapper::runRefineMask()
{
	try
	{
		std::cout << "Refine Mask Wrapper start" << std::endl;
		wrap_RefineMask = new mialsrtkRefineHRMaskByIntersection(inputFiles, transformIn, maskFiles, outputHRFile, outputLRFiles, referenceFile, radiusDilation, useStaple);
		boolExit = wrap_RefineMask->runRefineMask();
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

int RefineMaskEntryPoint(int argc, char *argv[])
{

	try {

		// Parse arguments
		TCLAP::CmdLine cmd("Refine The super resolution mask based on the intersection of the mask.", ' ', "Unversioned");

		// Input LR images
		TCLAP::MultiArg<std::string> inputArg("i", "input", "Low-resolution image file", true, "string", cmd);

		// Input LR masks
		TCLAP::MultiArg<std::string> maskArg("m", "mask", "low-resolution image mask file", true, "string", cmd);

		// Input LR masks
		TCLAP::MultiArg<std::string> outLRMaskArg("O", "output-lrmask", "output low-resolution image mask file", true, "string", cmd);

		// Input motion parameters
		TCLAP::MultiArg<std::string> transArg("t", "transform", "transform file", true, "string", cmd);

		// Ouput HR image
		TCLAP::ValueArg<std::string> outArg("o", "output-mask", "Super resolution output mask", true, "", "string", cmd);

		// Input reconstructed image for initialization
		TCLAP::ValueArg<std::string> refArg("r", "reference", "Reconstructed image for reference. "
			"Typically the output of SR program is used.", true, "", "string", cmd);

		// Radius of the structuring element (ball) used for dilation of each slice
		TCLAP::ValueArg<int> radiusDilationArg("", "radius-dilation", "Radius of the structuring element (ball) used for binary morphological dilation.", true, 0, "int", cmd);

		TCLAP::SwitchArg stapleArg("", "use-staple", "Use STAPLE for voting (Majority voting is used by default)", cmd, false);

		//TCLAP::ValueArg<std::string>debugDirArg("","debug","Directory where  SR reconstructed image at each outer loop of the reconstruction optimization is saved",false,"","string",cmd);

		// Parse the argv array.
		cmd.parse(argc, argv);

	}
	catch (TCLAP::ArgException &e)  // catch any exceptions
	{
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}
}