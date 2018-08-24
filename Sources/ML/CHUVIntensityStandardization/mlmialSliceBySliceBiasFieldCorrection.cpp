//----------------------------------------------------------------------------------
//! The ML module class mialSliceBySliceBiasFieldCorrection.
/*!
// \file   
// \author  FetalMRI
// \date    2018-05-22
//
// 
*/
//----------------------------------------------------------------------------------

#include "mlmialSliceBySliceBiasFieldCorrection.h"
#include "mialsrtkSliceBySliceCorrectBiasFieldWrapper.h"
#include "mlMessagingBackgroundTask.h"
#include <omp.h>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>


ML_START_NAMESPACE

class mialSliceBySliceBiasFieldCorrectionBackgroundTask : public MessagingBackgroundTask
{
public:
	mialSliceBySliceBiasFieldCorrectionBackgroundTask(mialSliceBySliceBiasFieldCorrection *pmialSliceBySliceBiasFieldCorrectionModule) :MessagingBackgroundTask(static_cast<void *>(pmialSliceBySliceBiasFieldCorrectionModule)),
		m_ownerModule(pmialSliceBySliceBiasFieldCorrectionModule), m_pmialSliceBySliceBiasFieldCorrectionModule(pmialSliceBySliceBiasFieldCorrectionModule)
	{
	}

	mialSliceBySliceBiasFieldCorrectionBackgroundTask(const mialSliceBySliceBiasFieldCorrectionBackgroundTask &w) :MessagingBackgroundTask(static_cast<void *>(w.m_pmialSliceBySliceBiasFieldCorrectionModule)),
		m_ownerModule(w.m_ownerModule), m_pmialSliceBySliceBiasFieldCorrectionModule(w.m_pmialSliceBySliceBiasFieldCorrectionModule)
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
			mlInfo(__FUNCTION__) << "Started Slice by Slice Bias Field Correction";
			m_pmialSliceBySliceBiasFieldCorrectionModule->CorrectBiasAllInput();

		}
		catch (...)
		{
		}

		callMethodOnGUI(m_ownerModule, &mialSliceBySliceBiasFieldCorrection::postComputation);
	}

private:
	DistantObject<mialSliceBySliceBiasFieldCorrection> m_ownerModule;
	mialSliceBySliceBiasFieldCorrection *m_pmialSliceBySliceBiasFieldCorrectionModule;

};

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(mialSliceBySliceBiasFieldCorrection, Module);

//----------------------------------------------------------------------------------

mialSliceBySliceBiasFieldCorrection::mialSliceBySliceBiasFieldCorrection() : Module(0, 0), m_pBGmialSliceBySliceBiasFieldCorrectionWorker(NULL), m_pmialSliceBySliceBiasFieldCorrectionThread(NULL)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add fields to the module and set their values.
  _inputImageFileFld = addString("inputImageFile", "");
  _inputMaskFileFld = addString("inputMaskFile", "");
  _inputBiasFieldFileFld = addString("inputBiasFieldFile", "");
  _outputImageFileFld = addString("outputImageFile", "");
  _statusFld = addString("status", "");
  _startTaskFld = addTrigger("startTask");
  _startTaskModalFld = addTrigger("startTaskModal");
  _inProgressFld = addBool("inProgress", false);
  _outputSucceedFld = addBool("outputSucceed", false);

  clear();
  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();


}

mialSliceBySliceBiasFieldCorrection::~mialSliceBySliceBiasFieldCorrection()
{
	if (m_pBGmialSliceBySliceBiasFieldCorrectionWorker)
	{
		delete (m_pBGmialSliceBySliceBiasFieldCorrectionWorker);
		m_pBGmialSliceBySliceBiasFieldCorrectionWorker = NULL;
	}

	if (m_pmialSliceBySliceBiasFieldCorrectionThread)
	{
		delete m_pmialSliceBySliceBiasFieldCorrectionThread;
		m_pmialSliceBySliceBiasFieldCorrectionThread;
	}


}


//----------------------------------------------------------------------------------

void mialSliceBySliceBiasFieldCorrection::handleNotification(Field* field)
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
		s = _inputBiasFieldFileFld->getStringValue() + "--";
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//remove whitespace
			boost::trim(token);
			std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splitinputBiasField.push_back(token);
		}

		std::cout << "I have splitted my input bias field" << std::endl;


		//la je fais des tests inutiles mais bon tant pis pour le moment.
		if (splitMasks.size() != splitInputs.size() || splitInputs.size() != splitOutputs.size() || splitMasks.size() != splitOutputs.size()
			|| splitMasks.size() != splitinputBiasField.size() || splitInputs.size() != splitinputBiasField.size())
		{
			mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "number of inputs/outputs/masks/inputBias are different";
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
			if (m_pBGmialSliceBySliceBiasFieldCorrectionWorker)
				delete (m_pBGmialSliceBySliceBiasFieldCorrectionWorker);
			m_pBGmialSliceBySliceBiasFieldCorrectionWorker = new mialSliceBySliceBiasFieldCorrectionBackgroundTask(this);
			std::cout << "background task created" << std::endl;
			_inProgressFld->setBoolValue(true);
			_statusFld->setStringValue("mial Slice by Slice Bias Correction Running");
			if (m_pmialSliceBySliceBiasFieldCorrectionThread)
				delete m_pmialSliceBySliceBiasFieldCorrectionThread;
			m_pmialSliceBySliceBiasFieldCorrectionThread = new boost::thread(*m_pBGmialSliceBySliceBiasFieldCorrectionWorker);
			//touchOutputs = true;
		}
		else if (field == _startTaskModalFld)
		{
			CorrectBiasAllInput();
		}

	}
}

void mialSliceBySliceBiasFieldCorrection::clear()
{
	//reset variable
	splitInputs.clear();
	splitMasks.clear();
	splitOutputs.clear();
	splitinputBiasField.clear();
	std::cout << "mialSliceBySliceBiasFieldCorrection dynamic vector cleared" << std::endl;
	_outputSucceedFld->setBoolValue(false);

}

void mialSliceBySliceBiasFieldCorrection::postComputation()
{
	_inProgressFld->setBoolValue(false);
	//std::cout << "mial Slice by Slice Bias Correction Done" << std::endl;

}

void mialSliceBySliceBiasFieldCorrection::CorrectBiasAllInput()
{
	std::cout << "inputFiles" << splitInputs.size() << std::endl;

	//here we use openmp
	unsigned int iterIm = splitInputs.size();
	omp_set_num_threads(7);
#pragma omp parallel for private(iterIm)
	for (int iterIm = 0; iterIm < splitInputs.size(); iterIm++)
	{

		//here I call the full constructor
		mialsrtkSliceBySliceCorrectBiasFieldWrapper* usemialsrtkSliceBySliceCorrectBiasFieldWrapper = new mialsrtkSliceBySliceCorrectBiasFieldWrapper(splitInputs[iterIm].c_str(), splitMasks[iterIm].c_str(), splitinputBiasField[iterIm].c_str(), splitOutputs[iterIm].c_str());
		usemialsrtkSliceBySliceCorrectBiasFieldWrapper->runSliceBySliceCorrectBiasField();
		if (usemialsrtkSliceBySliceCorrectBiasFieldWrapper->boolExit)
		{
			std::cout << "should have correct bias" << splitInputs[iterIm].c_str() << std::endl;
		}
		else
		{
			mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "Slice by Slice Bias Correction Failed : " << splitInputs[iterIm].c_str();
		}
	}

	_statusFld->setStringValue("Slice by Slice Bias Correction Done");
	_outputSucceedFld->setBoolValue(true);

}



ML_END_NAMESPACE