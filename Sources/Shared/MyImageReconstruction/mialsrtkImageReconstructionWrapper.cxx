#include "mialsrtkImageReconstructionWrapper.h"
#include "mialsrtkImageReconstruction.h"

#include <iostream>
mialImageReconstructionWrapper::mialImageReconstructionWrapper(std::vector< std::string > _inputFile, const char* const _outputFile, std::vector< std::string > _maskFile, const char* _refFile, std::vector< std::string > _transformout,
	std::vector< std::string > _resampled, const char* const _combinedMasks, unsigned int _itMax, double _epsilon, double _margin, bool _rigid3D, bool _noreg, bool _debluring, bool _boxSwitch,
	bool _maskSwitch, bool _allSwitch, unsigned int _ImageBaseOfRecon, const char* const _MetricToUse, unsigned int _m_Iterations, double _m_GradientMagnitudeTolerance, double _m_MinStepLength,
	double _m_MaxStepLength, double _m_RelaxationFactor)
{

	inputFile = _inputFile;
	outputFile = _outputFile;
	maskFile = _maskFile;
	refFile = _refFile;
	transformout = _transformout;
	combinedMask = _combinedMasks;
	resampled = _resampled;
	itMax = _itMax;
	epsilon = _epsilon;
	margin = _margin;
	rigid3D = _rigid3D;
	noreg = _noreg;
	debluring = _debluring;
	boxSwitch = _boxSwitch;
	maskSwitch = _maskSwitch;
	allSwitch = _allSwitch;
	ImageBaseOfRecon = _ImageBaseOfRecon;
	MetricToUse = _MetricToUse;

	m_Iterations = _m_Iterations;
	m_GradientMagnitudeTolerance = _m_GradientMagnitudeTolerance;
	m_MinStepLength = _m_MinStepLength;
	m_MaxStepLength = _m_MaxStepLength;
	m_RelaxationFactor = _m_RelaxationFactor;

}

mialImageReconstructionWrapper::~mialImageReconstructionWrapper()
{
	inputFile.clear();
	outputFile = NULL;
	maskFile.clear();
	refFile = NULL;
	transformout.clear();
	combinedMask = NULL;
	resampled.clear();
	itMax = NULL;
	epsilon = NULL;
	margin = NULL;
	rigid3D = NULL;
	noreg = NULL;
	debluring = NULL;
	boxSwitch = NULL;
	maskSwitch = NULL;
	allSwitch = NULL;
	ImageBaseOfRecon = NULL;
	MetricToUse = NULL;

	m_Iterations = NULL;
	m_GradientMagnitudeTolerance = NULL;
	m_MinStepLength = NULL;
	m_MaxStepLength = NULL;
	m_RelaxationFactor = NULL;

}


bool mialImageReconstructionWrapper::runImageReconstruction()
{
	try
	{
		std::cout << "Image Recon Wrapper start" << std::endl;
		std::cout << outputFile << std::endl;
		std::cout << "test" << std::endl;
		std::cout << MetricToUse << std::endl;
		wrap_ImageReconstruction = new mialImageReconstruction(inputFile, outputFile, maskFile, refFile, transformout, resampled, combinedMask,
			itMax, epsilon, margin, rigid3D, noreg, debluring, boxSwitch, maskSwitch, allSwitch, ImageBaseOfRecon, MetricToUse, m_Iterations,
	        m_GradientMagnitudeTolerance, m_MinStepLength, m_MaxStepLength, m_RelaxationFactor);
		boolExit = wrap_ImageReconstruction->runImageReconstruction();
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


int anotherEntryPointProblem()
{
	std::vector< std::string > inputTest;
	inputTest.push_back("D:/Pierre/FoetusTest/coronal2_worldmatrixModified_lr_NLM_reoriented_uni_bcorr.nii.gz");
	inputTest.push_back("D:/Pierre/FoetusTest/coronal1_worldmatrixModified_lr_NLM_reoriented_uni_bcorr.nii.gz");
	inputTest.push_back("D:/Pierre/FoetusTest/Axial2_worldmatrixModified_lr_NLM_reoriented_uni_bcorr.nii.gz");
	inputTest.push_back("D:/Pierre/FoetusTest/Axial1_worldmatrixModified_lr_NLM_reoriented_uni_bcorr.nii.gz");
	inputTest.push_back("D:/Pierre/FoetusTest/sagittal2_worldmatrixModified_lr_NLM_reoriented_uni_bcorr.nii.gz");
	inputTest.push_back("D:/Pierre/FoetusTest/sagittal1_worldmatrixModified_lr_NLM_reoriented_uni_bcorr.nii.gz");

	const char* outputFile = "D:/Pierre/FoetusTest/SDI_debug.nii.gz";
	std::vector< std::string > maskFile;
	maskFile.push_back("D:/Pierre/FoetusTest/coronal2_worldmatrixModified_lr_brain_mask_reoriented.nii.gz");
	maskFile.push_back("D:/Pierre/FoetusTest/coronal1_worldmatrixModified_lr_brain_mask_reoriented.nii.gz");
	maskFile.push_back("D:/Pierre/FoetusTest/Axial2_worldmatrixModified_lr_brain_mask_reoriented.nii.gz");
	maskFile.push_back("D:/Pierre/FoetusTest/Axial1_worldmatrixModified_lr_brain_mask_reoriented.nii.gz");
	maskFile.push_back("D:/Pierre/FoetusTest/sagittal2_worldmatrixModified_lr_brain_mask_reoriented.nii.gz");
	maskFile.push_back("D:/Pierre/FoetusTest/sagittal1_worldmatrixModified_lr_brain_mask_reoriented.nii.gz");

	const char* refFile = "";
	std::vector< std::string > transformout;
	transformout.push_back("D:/Pierre/FoetusTest/coronal2_worldmatrixModified_lr_reoriented_transform_6V_1.txt");
	transformout.push_back("D:/Pierre/FoetusTest/coronal1_worldmatrixModified_lr_reoriented_transform_6V_1.txt");
	transformout.push_back("D:/Pierre/FoetusTest/ Axial2_worldmatrixModified_lr_reoriented_transform_6V_1.txt");
	transformout.push_back("D:/Pierre/FoetusTest/Axial1_worldmatrixModified_lr_reoriented_transform_6V_1.txt");
	transformout.push_back("D:/Pierre/FoetusTest/sagittal2_worldmatrixModified_lr_reoriented_transform_6V_1.txt");
	transformout.push_back("D:/Pierre/FoetusTest/sagittal1_worldmatrixModified_lr_reoriented_transform_6V_1.txt");

	std::vector< std::string > resampled;
	mialImageReconstructionWrapper test(inputTest,outputFile, maskFile, "",transformout, resampled,"",10,0.0001,0,0,0,0,0,1,0,0,"NC",2000,0.0001,0.0001,0.2,0.5);
	bool boolOutput = test.runImageReconstruction();
	std::cout << boolOutput << std::endl;

}

/*int EntryPointImageReconstruction( int argc, char *argv[] )
{
try {
  TCLAP::MultiArg<std::string> inputArg("i","input","Image file",true,"std::string",cmd);
  TCLAP::MultiArg<std::string> maskArg("m","","Mask file",false,"string",cmd);
  TCLAP::MultiArg<std::string> transformArg("t","transform","Transform output "
      "file",false,"string",cmd);
  TCLAP::MultiArg<std::string> resampledArg("","ir","Resampled image with initial "
      "transform (this is an output to check initial transform consistency)",false,"string",cmd);
  TCLAP::ValueArg<std::string> outArg("o","output","High resolution image",true,
      "","string",cmd);
  TCLAP::ValueArg<std::string> refArg("r","reference","Reference Image",false, "","string",cmd);

  TCLAP::ValueArg<std::string> combinedMaskArg("","combinedMasks","All image "
      "masks combined in a single one",false,"","string",cmd);
  TCLAP::ValueArg<unsigned int> iterArg("n","iter","Maximum number of iterations"
      " (default 10)",false, 10,"unsigned int",cmd);
  TCLAP::ValueArg<double> epsilonArg("e","epsilon","Minimal percent change between "
      "two iterations considered as convergence. (default 0.0001)",false, 1e-4,
      "double",cmd);

  TCLAP::ValueArg<double> marginArg("","margin","Adds a margin to the reconstructed "
      "images to compensate for a small FOV in the reference. The value must be "
      "provided in millimeters (default 0).",false, 0.0,"double",cmd);

  TCLAP::SwitchArg  boxSwitchArg("","box","Use intersections for roi calculation",false);
  TCLAP::SwitchArg  maskSwitchArg("","mask","Use masks for roi calculation",false);
  TCLAP::SwitchArg  allSwitchArg("","all","Use the whole image FOV",false);

  TCLAP::SwitchArg  rigid3DSwitchArg("","3D","Use of 3D rigid transforms."
      " Recommended for adult subjects.", cmd, false);

  TCLAP::SwitchArg  noregSwitchArg("","noreg","No registration is performed, the "
      "image is reconstructed with the identity transform. This option is important "
      "to have a reference for performance assessment. ", cmd, false);

  // Flag that set deblurring Kernel for SDI
  TCLAP::SwitchArg  debluringArg("","debluring","Flag to set deblurring kernel for SDI (double the neighborhood)"
                                          " (by default it is disable.).",cmd,false);

  std::vector<TCLAP::Arg*>  xorlist;
  xorlist.push_back(&boxSwitchArg);
  xorlist.push_back(&maskSwitchArg);
  xorlist.push_back(&allSwitchArg);

  cmd.xorAdd( xorlist );

  // Parse the argv array.
  cmd.parse( argc, argv );

  input = inputArg.getValue();
  mask = maskArg.getValue();
  outImage = outArg.getValue().c_str();
  refImage = refArg.getValue();
  combinedMask = combinedMaskArg.getValue().c_str();
  transform = transformArg.getValue();
  resampled = resampledArg.getValue();
  itMax = iterArg.getValue();
  epsilon = epsilonArg.getValue();
  margin = marginArg.getValue();

  bool rigid3D = rigid3DSwitchArg.getValue();
  bool noreg   = noregSwitchArg.getValue();

  } catch (TCLAP::ArgException &e)  // catch any exceptions
  { std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }
}
*/