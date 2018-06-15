//----------------------------------------------------------------------------------
//! The ML module class mialSliceBySliceBiasEstimation.
/*!
// \file   
// \author  FetalMRI
// \date    2018-05-22
//
// 
*/
//----------------------------------------------------------------------------------

#include "mlmialSliceBySliceBiasEstimation.h"

#include "mlMessagingBackgroundTask.h"
#include <omp.h>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>

ML_START_NAMESPACE

class mialSliceBySliceBiasEstimationBackgroundTask : public MessagingBackgroundTask
{
public:
	mialSliceBySliceBiasEstimationBackgroundTask(mialSliceBySliceBiasEstimation *pmialSliceBySliceBiasEstimationModule) :MessagingBackgroundTask(static_cast<void *>(pmialSliceBySliceBiasEstimationModule)),
		m_ownerModule(pmialSliceBySliceBiasEstimationModule), m_pmialSliceBySliceBiasEstimationModule(pmialSliceBySliceBiasEstimationModule)
	{
	}

	mialSliceBySliceBiasEstimationBackgroundTask(const mialSliceBySliceBiasEstimationBackgroundTask &w) :MessagingBackgroundTask(static_cast<void *>(w.m_pmialSliceBySliceBiasEstimationModule)),
		m_ownerModule(w.m_ownerModule), m_pmialSliceBySliceBiasEstimationModule(w.m_pmialSliceBySliceBiasEstimationModule)
	{
	}


	//----------------------------------------------------------------------------------
	//! Implement the processing, which is called from a different thread.
	//----------------------------------------------------------------------------------
	//apparently boost::thread will look for the function operator
	void operator()()
	{
		run();
	}

	void run()
	{
		try
		{
			mlInfo(__FUNCTION__) << "Started Correct Slice Intensity";
			m_pmialSliceBySliceBiasEstimationModule->EstimateBiasAllInput();

		}
		catch (...)
		{
		}

		callMethodOnGUI(m_ownerModule, &mialSliceBySliceBiasEstimation::postComputation);
	}

private:
	DistantObject<mialSliceBySliceBiasEstimation> m_ownerModule;
	mialSliceBySliceBiasEstimation *m_pmialSliceBySliceBiasEstimationModule;

};

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(mialSliceBySliceBiasEstimation, Module);

//----------------------------------------------------------------------------------

mialSliceBySliceBiasEstimation::mialSliceBySliceBiasEstimation() : Module(0, 0), m_pBGmialSliceBySliceBiasEstimationWorker(NULL), m_pmialSliceBySliceBiasEstimationThread(NULL)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add fields to the module and set their values.
  _inputImageFileFld = addString("inputImageFile", "");
  _inputMaskFileFld = addString("inputMaskFile", "");
  _outputImageFileFld = addString("outputImageFile", "");
  _outputBiasFieldFileFld = addString("outputBiasFieldFile", "");
  _statusFld = addString("status", "");
  _startTaskFld = addTrigger("startTask");
  _startTaskModalFld = addTrigger("startTaskModal");
  _inProgressFld = addBool("inProgress", false);
  _outputSucceedFld = addBool("outputSucceed", false);

  clear();
  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();


}

mialSliceBySliceBiasEstimation::~mialSliceBySliceBiasEstimation()
{
	if (m_pBGmialSliceBySliceBiasEstimationWorker)
	{
		delete (m_pBGmialSliceBySliceBiasEstimationWorker);
		m_pBGmialSliceBySliceBiasEstimationWorker = NULL;
	}

	if (m_pmialSliceBySliceBiasEstimationThread)
	{
		delete m_pmialSliceBySliceBiasEstimationThread;
		m_pmialSliceBySliceBiasEstimationThread;
	}

}

//----------------------------------------------------------------------------------

void mialSliceBySliceBiasEstimation::handleNotification(Field* field)
{
  // Handle changes of module parameters and input image fields here.
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
		std::string s = _inputImageFileFld->getStringValue() + "--";
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//remove whitespace
			boost::trim(token);
			std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splitInputs.push_back(token);
		}

		std::cout << "I have splitted my input" << std::endl;

		//then outputfilename
		pos = 0;
		token = "";
		s = _outputImageFileFld->getStringValue() + "--";
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//remove whitespace
			boost::trim(token);
			std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splitOutputs.push_back(token);
		}

		std::cout << "I have splitted my output" << std::endl;


		pos = 0;
		token = "";
		s = _inputMaskFileFld->getStringValue() + "--";
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//remove whitespace
			boost::trim(token);
			std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splitMasks.push_back(token);
		}
		
		std::cout << "I have splitted my mask" << std::endl;

		pos = 0;
		token = "";
		s = _outputBiasFieldFileFld->getStringValue() + "--";
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//remove whitespace
			boost::trim(token);
			std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splitoutputBiasField.push_back(token);
		}

		std::cout << "I have splitted my output bias field" << std::endl;


		//la je fais des tests inutiles mais bon tant pis pour le moment.
		if (splitMasks.size() != splitInputs.size() || splitInputs.size() != splitOutputs.size() || splitMasks.size() != splitOutputs.size()
			|| splitMasks.size() != splitoutputBiasField.size() || splitInputs.size() != splitoutputBiasField.size())
		{
			mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "number of inputs/outputs/masks/outputBias are different";
			return;
		}


		//loop on the std::vector splitInputs
		for (std::vector<int>::size_type i = 0; i != splitInputs.size(); i++)
		{
			if (!std::ifstream(splitInputs[i]))
			{
				mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "File not Found  :  " << splitInputs[i];
				return;
			}
		}
		std::cout << "Files Found" << std::endl;

		if (field == _startTaskFld)
		{
			//kill le background worker si il exist:
			if (m_pBGmialSliceBySliceBiasEstimationWorker)
				delete (m_pBGmialSliceBySliceBiasEstimationWorker);
			m_pBGmialSliceBySliceBiasEstimationWorker = new mialSliceBySliceBiasEstimationBackgroundTask(this);
			std::cout << "background task created" << std::endl;
			_inProgressFld->setBoolValue(true);
			_statusFld->setStringValue("mial Slice by Slice Bias Estimation Running");
			if (m_pmialSliceBySliceBiasEstimationThread)
				delete m_pmialSliceBySliceBiasEstimationThread;
			m_pmialSliceBySliceBiasEstimationThread = new boost::thread(*m_pBGmialSliceBySliceBiasEstimationWorker);
			//touchOutputs = true;
		}
		else if (field == _startTaskModalFld)
		{
			EstimateBiasAllInput();
		}
	}
}

void mialSliceBySliceBiasEstimation::clear()
{
	//reset variable
	splitInputs.clear();
	splitMasks.clear();
	splitOutputs.clear();
	splitoutputBiasField.clear();
	std::cout << "dynamic vector cleared" << std::endl;
	_outputSucceedFld->setBoolValue(false);

}

void mialSliceBySliceBiasEstimation::postComputation()
{
	_inProgressFld->setBoolValue(false);
	std::cout << "mial Slice by Slice Bias Estimation Done" << std::endl;
	_statusFld->setStringValue("Slice by Slice Bias Estimation Done");

}

void mialSliceBySliceBiasEstimation::EstimateBiasAllInput()
{
	std::cout << "inputFiles" << splitInputs.size() << std::endl;

	//here we use openmp
	unsigned int iterIm = splitInputs.size();
	omp_set_num_threads(7);
#pragma omp parallel for private(iterIm)
	for (int iterIm = 0; iterIm < splitInputs.size(); iterIm++)
	{

		//here I call the full constructor
		mialsrtkSliceBySliceN4BiasFieldCorrectionWrapper* usemialsrtkSliceBySliceN4BiasFieldCorrectionWrapper = new mialsrtkSliceBySliceN4BiasFieldCorrectionWrapper(splitInputs[iterIm].c_str(), splitMasks[iterIm].c_str(), splitOutputs[iterIm].c_str(), splitoutputBiasField[iterIm].c_str());
		usemialsrtkSliceBySliceN4BiasFieldCorrectionWrapper->runSliceBySliceN4BiasFieldCorrection();
		if (usemialsrtkSliceBySliceN4BiasFieldCorrectionWrapper->boolExit)
		{
			std::cout << "should have estimate bias" << splitInputs[iterIm].c_str() << std::endl;
		}
		else
		{
			mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "Slice by Slice Bias Estimation Failed : " << splitInputs[iterIm].c_str();
		}
	}

	_outputSucceedFld->setBoolValue(true);

}


ML_END_NAMESPACE