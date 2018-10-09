//----------------------------------------------------------------------------------
//! The ML module class mialTVSuperResolution.
/*!
// \file   
// \author  FetalMRI
// \date    2018-06-26
//
// 
*/
//----------------------------------------------------------------------------------

#include "mlmialTVSuperResolution.h"
#include "mialsrtkTVSuperResolutionWrapper.h"
#include "mlMessagingBackgroundTask.h"
#include <omp.h>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>

ML_START_NAMESPACE

class mialSuperResolutionBackgroundTask : public MessagingBackgroundTask
{
public:
	mialSuperResolutionBackgroundTask(mialTVSuperResolution *pmialTVSuperResolutionModule) :MessagingBackgroundTask(static_cast<void *>(pmialTVSuperResolutionModule)),
		m_ownerModule(pmialTVSuperResolutionModule), m_pmialTVSuperResolutionModule(pmialTVSuperResolutionModule)
	{
	}
	mialSuperResolutionBackgroundTask(const mialSuperResolutionBackgroundTask &w) :MessagingBackgroundTask(static_cast<void *>(w.m_pmialTVSuperResolutionModule)),
		m_ownerModule(w.m_ownerModule), m_pmialTVSuperResolutionModule(w.m_pmialTVSuperResolutionModule)
	{
	}

	//apparently boost::thread will look for the function operator
	void operator()()
	{
		run();
	}

	void run()
	{
		try
		{
			mlInfo(__FUNCTION__) << "Started TV SuperResolution";
			m_pmialTVSuperResolutionModule->runTVSuperResolution();

		}
		catch (...)
		{
		}

		callMethodOnGUI(m_ownerModule, &mialTVSuperResolution::postComputation);
	}

private:
	DistantObject<mialTVSuperResolution> m_ownerModule;
	mialTVSuperResolution *m_pmialTVSuperResolutionModule;

};

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(mialTVSuperResolution, Module);

//----------------------------------------------------------------------------------

mialTVSuperResolution::mialTVSuperResolution() : Module(0, 0), m_pBGSuperResolutionWorker(NULL), m_pBGSuperResolutionWorkerThread(NULL)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add fields to the module and set their values.
  _outputFileFld = addString("outputFile", "");
  _maskFilesFld = addString("maskFiles", "");
  _refFileFld = addString("refFile", "");
  _inputFilesFld = addString("inputFiles", "");
  _transformFilesInputFld = addString("transformFilesInput", "");
  _nbIterFld = addInt("nbIter", 50);
  _LambdaFld = addFloat("Lambda", 0.1);
  _DeltaTFld = addFloat("DeltaT", 0.1);
  _GammaFld = addFloat("Gamma", 1.0);
  _stepScaleFld = addFloat("stepScale", 1.0);
  _innerConvThresholdFld = addDouble("innerConvThreshold", 0.0001);
  _outerConvThresholdFld = addDouble("outerConvThreshold", 0.0001);
  _numberOfBregmanLoopsFld = addInt("numberOfBregmanLoops", 10);
  _numberOfLoopsFld = addInt("numberOfLoops", 5);
  _boxcarFld = addBool("boxcar", false);
  _updateMotionFld = addBool("updateMotion", false);
  _refMaskFld = addString("refMask", "");
  _pre_inputFld = addString("pre_input", "");
  _transformFilesOutputFld = addString("transformFilesOutput", "");
  _debugDirFld = addString("debugDir", "");
  _debluringFld = addBool("debluring", false);
  _kernelRadiusMultiplicatorFld = addFloat("kernelRadiusMultiplicator", 1.0);
  static const char * const CoregistrationMethodValues[] = { "NC", "MI" };
  _CoregistrationMethodFld = addEnum("CoregistrationMethod", CoregistrationMethodValues, 2);
  _CoregistrationMethodFld->setEnumValue(0);
  _NbIterationRegistrationFld = addInt("NbIterationRegistration", 2000);
  _GradientMagnitudeToleranceRegistrationFld = addDouble("GradientMagnitudeToleranceRegistration", 0.0001);
  _MinStepLengthRegistrationFld = addDouble("MinStepLengthRegistration", 0.0001);
  _RelaxationFactorRegistrationFld = addDouble("RelaxationFactorRegistration", 0.5);
  _MaxStepLengthRegistrationFld = addDouble("MaxStepLengthRegistration", 0.2);

  _statusFld = addString("status", "");
  _startTaskFld = addTrigger("startTask");
  _startTaskModalFld = addTrigger("startTaskModal");
  _inProgressFld = addBool("inProgress", false);
  _outputSucceedFld = addBool("outputSucceed", false);
  
  clear();

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();


  // Activate inplace data buffers for output outputIndex and input inputIndex.
  // setOutputImageInplace(outputIndex, inputIndex);

  // Activate page data bypass from input inputIndex to output outputIndex.
  // Note that the module must still be able to calculate the output image.
  // setBypass(outputIndex, inputIndex);

  // By default modules only support scalar voxel types. If you want to
  // support other voxel types, you need to enable the general support with
  //
  //   setVoxelDataTypeSupport(ALL_REGISTERED_TYPES);
  //
  // and explicitly invalidate the output image for unsupported input types in
  // calculateOutputImageProperties.
  // You also need to use other multiplexing macros than 
  // ML_CALCULATEOUTPUTSUBIMAGE_NUM_INPUTS_N/x_SCALAR_TYPES_CPP, or forego these macros
  // completely and override the non-templated calculateOutputSubImage method and
  // do the necessary type multiplexing yourself. (Hint: It might be easier to use
  // new-style output image handler modules instead)

}

//----------------------------------------------------------------------------------
mialTVSuperResolution::~mialTVSuperResolution()
{
	if (m_pBGSuperResolutionWorker)
	{
		delete (m_pBGSuperResolutionWorker);
		m_pBGSuperResolutionWorker = NULL;
	}

	if (m_pBGSuperResolutionWorkerThread)
	{
		delete m_pBGSuperResolutionWorkerThread;
		m_pBGSuperResolutionWorkerThread;
	}

}


void mialTVSuperResolution::clear()
{	
	//reset variable
	splitinputFile.clear();
	splitmaskFile.clear();
	splittransformin.clear();
	splittransformout.clear();
	splitpreinput.clear();
	std::cout << "dynamic vector cleared" << std::endl;
	_outputSucceedFld->setBoolValue(false);

}


void mialTVSuperResolution::handleNotification(Field* field)
{
  // Handle changes of module parameters and input image fields here.
	if (field == _startTaskFld || field == _startTaskModalFld)
	{
		clear();
		std::string delimiter = "--";

		//inputfilename first
		size_t pos = 0;
		std::string token;
		std::string s = _inputFilesFld->getStringValue() + "--";
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//remove whitespace
			boost::trim(token);
			//std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splitinputFile.push_back(token);
		}

		std::cout << "I have splitted my input" << std::endl;



		pos = 0;
		token = "";
		s = _transformFilesInputFld->getStringValue() + "--";
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//remove whitespace
			boost::trim(token);
			//std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splittransformin.push_back(token);
		}

		
		if (!_transformFilesOutputFld->getStringValue().empty())
		{
			pos = 0;
			token = "";
			s = _transformFilesOutputFld->getStringValue() + "--";
			while ((pos = s.find(delimiter)) != std::string::npos) {
				token = s.substr(0, pos);
				//remove whitespace
				boost::trim(token);
				//std::cout << token << std::endl;
				s.erase(0, pos + delimiter.length());
				splittransformout.push_back(token);
			}
		}


		//I have split the transformout

		pos = 0;
		token = "";
		s = _maskFilesFld->getStringValue() + "--";
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//remove whitespace
			boost::trim(token);
			//std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splitmaskFile.push_back(token);
		}


		
		if (!_pre_inputFld->getStringValue().empty())
		{
			pos = 0;
			token = "";
			s = _pre_inputFld->getStringValue() + "--";
			while ((pos = s.find(delimiter)) != std::string::npos) {
				token = s.substr(0, pos);
				//remove whitespace
				boost::trim(token);
				//std::cout << token << std::endl;
				s.erase(0, pos + delimiter.length());
				splitpreinput.push_back(token);
			}
		}
		



		if (splitinputFile.size() != splittransformin.size()  || splitinputFile.size() != splitmaskFile.size())
		{
			mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "number of inputs/transformouts/masks are different";
			return;
		}


		//loop on the std::vector splitInputs
		for (std::vector<int>::size_type i = 0; i != splitinputFile.size(); i++)
		{
			if (!std::ifstream(splitinputFile[i]))
			{
				mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "File not Found  :  " << splitinputFile[i];
				return;
			}
		}
		std::cout << "Files Found" << std::endl;

		if (field == _startTaskFld)
		{
			//kill le background worker si il exist:
			if (m_pBGSuperResolutionWorker)
				std::cout << "TV Super Resolution Worker killed" << std::endl;
			delete (m_pBGSuperResolutionWorker);
			m_pBGSuperResolutionWorker = new mialSuperResolutionBackgroundTask(this);
			std::cout << "background task created" << std::endl;
			_inProgressFld->setBoolValue(true);
			_statusFld->setStringValue("TV SuperResolution Running");
			if (m_pBGSuperResolutionWorkerThread)
				std::cout << "TV SuperResolution thread killed" << std::endl;
			delete m_pBGSuperResolutionWorkerThread;
			m_pBGSuperResolutionWorkerThread = new boost::thread(*m_pBGSuperResolutionWorker);
		}
		else if (field == _startTaskModalFld)
		{
			runTVSuperResolution();
		}
	}


}


void mialTVSuperResolution::runTVSuperResolution()
{


	const char* tempOutput;
	std::string temp1Output;

	const char* tempmetricToUse;
	std::string temp1metricToUse;

	const char* temprefFile;
	std::string temp1refFile;

	const char* temprefMask;
	std::string temp1refMask;

	const char* tempDebugDir;
	std::string temp1DebugDir;


	temp1Output = _outputFileFld->getStringValue();
	tempOutput = temp1Output.c_str();

	temp1metricToUse = _CoregistrationMethodFld->getStringValue();
	tempmetricToUse = temp1metricToUse.c_str();

	temp1refFile = _refFileFld->getStringValue();
	temprefFile = temp1refFile.c_str();

	temp1refMask = _refMaskFld->getStringValue();
	temprefMask = temp1refMask.c_str();

	temp1DebugDir = _debugDirFld->getStringValue();
	tempDebugDir = temp1DebugDir.c_str();

	mialsrtkTVSuperResolutionWrapper* useTVSuperResolutionWrapper = new mialsrtkTVSuperResolutionWrapper(splitinputFile, splitmaskFile, tempOutput, temprefFile, 
		splittransformin, _nbIterFld->getIntValue(), _LambdaFld->getFloatValue(), _DeltaTFld->getFloatValue(), _GammaFld->getFloatValue(), _stepScaleFld->getFloatValue(),
		_innerConvThresholdFld->getDoubleValue(), _outerConvThresholdFld->getDoubleValue(), _numberOfLoopsFld->getIntValue(), _numberOfBregmanLoopsFld->getIntValue(),
		_boxcarFld->getBoolValue(), _updateMotionFld->getBoolValue(), temprefMask, splitpreinput, splittransformout, tempDebugDir, _debluringFld->getBoolValue(), 
		_kernelRadiusMultiplicatorFld->getFloatValue(), tempmetricToUse, _NbIterationRegistrationFld->getIntValue(), _GradientMagnitudeToleranceRegistrationFld->getDoubleValue(),
		_MinStepLengthRegistrationFld->getDoubleValue(), _MaxStepLengthRegistrationFld->getDoubleValue(), _RelaxationFactorRegistrationFld->getDoubleValue());

	std::cout << "wrapper construct" << std::endl;
	std::cout << useTVSuperResolutionWrapper->outputFile << std::endl;

	useTVSuperResolutionWrapper->runTVSuperResolution();
	if (useTVSuperResolutionWrapper->boolExit)
	{
		std::cout << "should have done the Super Resolution" << std::endl;
	}
	else
	{
		mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "mialTVSuperResolution Failed : ";
	}

	_outputSucceedFld->setBoolValue(true);


}


void mialTVSuperResolution::postComputation()
{
	_inProgressFld->setBoolValue(false);
	_statusFld->setStringValue("TV SuperResolution Done");
	std::cout << "TV SuperResolution Done" << std::endl;
}

ML_END_NAMESPACE