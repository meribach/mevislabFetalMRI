//----------------------------------------------------------------------------------
//! The ML module class mialImageReconstruction.
/*!
// \file   
// \author  FetalMRI
// \date    2018-06-11
//
// 
*/
//----------------------------------------------------------------------------------

#include "mlmialImageReconstruction.h"

#include "mialsrtkImageReconstructionWrapper.h"
#include "mlMessagingBackgroundTask.h"
#include <omp.h>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>


ML_START_NAMESPACE

class mialImageReconstructionBackgroundTask : public MessagingBackgroundTask
{
public:
	mialImageReconstructionBackgroundTask(mialImageReconstruction *pmialImageReconstructionModule) :MessagingBackgroundTask(static_cast<void *>(pmialImageReconstructionModule)),
		m_ownerModule(pmialImageReconstructionModule), m_pmialImageReconstructionModule(pmialImageReconstructionModule)
	{
	}
	mialImageReconstructionBackgroundTask(const mialImageReconstructionBackgroundTask &w) :MessagingBackgroundTask(static_cast<void *>(w.m_pmialImageReconstructionModule)),
		m_ownerModule(w.m_ownerModule), m_pmialImageReconstructionModule(w.m_pmialImageReconstructionModule)
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
			mlInfo(__FUNCTION__) << "Started ImageReconstruction";
			m_pmialImageReconstructionModule->ReconstructHRImage();

		}
		catch (...)
		{
		}

		callMethodOnGUI(m_ownerModule, &mialImageReconstruction::postComputation);
	}

private:
	DistantObject<mialImageReconstruction> m_ownerModule;
	mialImageReconstruction *m_pmialImageReconstructionModule;

};

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(mialImageReconstruction, Module);

//----------------------------------------------------------------------------------

mialImageReconstruction::mialImageReconstruction() : Module(0, 0), m_pBGImageReconstructionWorker(NULL), m_pBGImageReconstructionWorkerThread(NULL)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add fields to the module and set their values.
  _inputFilesFld = addString("inputFiles", "");
  _maskFilesFld = addString("maskFiles", "");
  _outputFileFld = addString("outputFile", "");
  _refFileFld = addString("refFile", "");
  _transformoutFilesFld = addString("transformoutFiles", "");
  _resampledFilesFld = addString("resampledFiles", "");
  _combinedMasksFld = addString("combinedMasks", "");
  _itMaxFld = addInt("itMax", 10);
  _espilonFld = addDouble("espilon", 0.0001);
  _marginFld = addDouble("margin", 0);
  _rigid3DFld = addBool("rigid3D", false);
  _noregFld = addBool("noreg", false);
  _debluringFld = addBool("debluring", false);
  static const char * const methodRoiCalculationValues[] = { "box", "mask", "all" };
  _methodRoiCalculationFld = addEnum("methodRoiCalculation", methodRoiCalculationValues, 3);
  _methodRoiCalculationFld->setEnumValue(1);
  _ImageBaseOfReconFld = addInt("ImageBaseOfRecon", 0);
  static const char * const MetricToUseValues[] = { "NC", "MI" };
  _MetricToUseFld = addEnum("MetricToUse", MetricToUseValues, 2);
  _MetricToUseFld->setEnumValue(0);
  _nbIterationsRegistrationFld = addInt("nbIterationsRegistration", 2000);
  _GradientMagnitudeToleranceRegistrationFld = addDouble("GradientMagnitudeToleranceRegistration", 0.0001);
  _MinStepLengthRegistrationFld = addDouble("MinStepLengthRegistration", 0.0001);
  _MaxStepLengthRegistrationFld = addDouble("MaxStepLengthRegistration", 0.2);
  _RelaxationFactorRegistrationFld = addDouble("RelaxationFactorRegistration", 0.5);
  _statusFld = addString("status", "");
  _startTaskFld = addTrigger("startTask");
  _startTaskModalFld = addTrigger("startTaskModal");
  _inProgressFld = addBool("inProgress", false);
  _outputSucceedFld = addBool("outputSucceed", false);
  // Reactivate calls of handleNotification on field changes.
  
  clear();

  handleNotificationOn();


}

mialImageReconstruction::~mialImageReconstruction()
{
	if (m_pBGImageReconstructionWorker)
	{
		delete (m_pBGImageReconstructionWorker);
		m_pBGImageReconstructionWorker = NULL;
	}

	if (m_pBGImageReconstructionWorkerThread)
	{
		delete m_pBGImageReconstructionWorkerThread;
		m_pBGImageReconstructionWorkerThread;
	}


}

//----------------------------------------------------------------------------------

void mialImageReconstruction::handleNotification(Field* field)
{
	if (field == _startTaskFld || field == _startTaskModalFld)
	{

		clear();
		//for now we try with only one filepath.
		//for more than one, have to split the string  
		//std::cout << std::ifstream(_inputFileNameFld->getStringValue()).good() << std::endl;
		//faire le split et verif meme nombre de valeur
		std::string delimiter = "--";

		//inputfilename first
		size_t pos = 0;
		std::string token;
		std::string s = _inputFilesFld->getStringValue() + "--";
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//remove whitespace
			boost::trim(token);
			std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splitinputFile.push_back(token);
		}

		std::cout << "I have splitted my input" << std::endl;

		//then outputfilename
		/*pos = 0;
		token = "";
		s = _outputFileNameFld->getStringValue() + "--";
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//remove whitespace
			boost::trim(token);
			std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splitOutputs.push_back(token);
		}

		std::cout << "I have splitted my output" << std::endl;*/



            /*
			std::cout << "no maskFiles" << std::endl;
			//I generate a std vector mask with empty strings same size as the input one
			std::cout << _methodRoiCalculationFld->getStringValue() << std::endl;
			std::cout << _methodRoiCalculationFld->getStringValue().c_str() << std::endl;
			std::cout << strncmp(_methodRoiCalculationFld->getStringValue().c_str(), "mask", 10) << std::endl;
			*/
		
		if (strncmp(_methodRoiCalculationFld->getStringValue().c_str(), "mask", 10) ==0)
		{
			pos = 0;
			token = "";
			s = _maskFilesFld->getStringValue() + "--";
			while ((pos = s.find(delimiter)) != std::string::npos) {
				token = s.substr(0, pos);
				//remove whitespace
				boost::trim(token);
				std::cout << token << std::endl;
				s.erase(0, pos + delimiter.length());
				splitmaskFile.push_back(token);
			}

			if (splitinputFile.size() != splitmaskFile.size())
			{
				mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "number of inputs/masks are different";
				return;
			}
		}

		
		pos = 0;
		token = "";
		s = _transformoutFilesFld->getStringValue() + "--";
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//remove whitespace
			boost::trim(token);
			std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splittransformout.push_back(token);
		}

		//I have split the transformout

		if (!_resampledFilesFld->getStringValue().empty())
		{
			pos = 0;
			token = "";
			s = _resampledFilesFld->getStringValue() + "--";
			while ((pos = s.find(delimiter)) != std::string::npos) {
				token = s.substr(0, pos);
				//remove whitespace
				boost::trim(token);
				std::cout << token << std::endl;
				s.erase(0, pos + delimiter.length());
				splitresampled.push_back(token);
			}

		}

		if (splitinputFile.size() != splittransformout.size())
		{
			mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "number of inputs/transformouts are different";
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
			if (m_pBGImageReconstructionWorker)
				std::cout << "Image Reconstruction Worker killed" << std::endl;
			delete (m_pBGImageReconstructionWorker);
			m_pBGImageReconstructionWorker = new mialImageReconstructionBackgroundTask(this);
			std::cout << "background task created" << std::endl;
			_inProgressFld->setBoolValue(true);
			_statusFld->setStringValue("Image Reconstruction Running");
			if (m_pBGImageReconstructionWorkerThread)
				std::cout << "Image Reconstruction thread killed" << std::endl;
			delete m_pBGImageReconstructionWorkerThread;
			m_pBGImageReconstructionWorkerThread = new boost::thread(*m_pBGImageReconstructionWorker);
		}
		else if (field == _startTaskModalFld)
		{
			ReconstructHRImage();
		}

	}
}

void mialImageReconstruction::clear()
{
	//reset variable
	splitinputFile.clear();
	splitmaskFile.clear();
	splittransformout.clear();
	splitresampled.clear();
	std::cout << "dynamic vector cleared" << std::endl;
	_outputSucceedFld->setBoolValue(false);


}

void mialImageReconstruction::ReconstructHRImage()
{
  if (strncmp(_methodRoiCalculationFld->getStringValue().c_str(), "mask", 10) == 0)
  {maskBool = true;} else {maskBool = false;}

  if (strncmp(_methodRoiCalculationFld->getStringValue().c_str(), "box", 10) == 0)
  {boxBool = true;} else { boxBool = false;}

  if (strncmp(_methodRoiCalculationFld->getStringValue().c_str(), "all", 10) == 0)
  {allBool = true;} else { allBool = false;}
  
  mialImageReconstructionWrapper* useImageReconstructionWrapper = new mialImageReconstructionWrapper(splitinputFile,_outputFileFld->getStringValue().c_str(),splitmaskFile,_refFileFld->getStringValue().c_str(),
	  splittransformout,splitresampled,_combinedMasksFld->getStringValue().c_str(),_itMaxFld->getIntValue(),_espilonFld->getDoubleValue(),_marginFld->getDoubleValue(), _rigid3DFld->getBoolValue(),
	  _noregFld->getBoolValue(),_debluringFld->getBoolValue(), boxBool, maskBool, allBool,_ImageBaseOfReconFld->getIntValue(),_MetricToUseFld->getStringValue().c_str(),
	  _nbIterationsRegistrationFld->getIntValue(),_GradientMagnitudeToleranceRegistrationFld->getDoubleValue(),_MinStepLengthRegistrationFld->getDoubleValue(),
	  _MaxStepLengthRegistrationFld->getDoubleValue(),_RelaxationFactorRegistrationFld->getDoubleValue());
  useImageReconstructionWrapper->runImageReconstruction();
  if (useImageReconstructionWrapper->boolExit)
	{
		std::cout << "should have reconstruct the HR Image" << std::endl;
	}
  else
	{
		mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "mialImageReconstruction Failed : " ;
	}

  _outputSucceedFld->setBoolValue(true);

}


void mialImageReconstruction::postComputation()
{
	_inProgressFld->setBoolValue(false);
	_statusFld->setStringValue("Image Reconstruction Done");
	std::cout << "Image Reconstruction Done" << std::endl;
}


ML_END_NAMESPACE