//----------------------------------------------------------------------------------
//! The ML module class mialN4BialsField.
/*!
// \file   
// \author  FetalMRI
// \date    2018-08-03
//
// 
*/
//----------------------------------------------------------------------------------

#include "mlmialN4BiasField.h"
#include "mialsrtkN4BiasFieldCorrectionWrapper.h"
#include "mlMessagingBackgroundTask.h"
#include <omp.h>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>

ML_START_NAMESPACE

class mialN4BiasFieldBackgroundTask : public MessagingBackgroundTask
{
public:
	mialN4BiasFieldBackgroundTask(mialN4BiasField *pN4BiasCorrectionModule) :MessagingBackgroundTask(static_cast<void *>(pN4BiasCorrectionModule)),
		m_ownerModule(pN4BiasCorrectionModule), m_pN4BiasCorrectionModule(pN4BiasCorrectionModule)
	{
	}
	mialN4BiasFieldBackgroundTask(const mialN4BiasFieldBackgroundTask &w) :MessagingBackgroundTask(static_cast<void *>(w.m_pN4BiasCorrectionModule)),
		m_ownerModule(w.m_ownerModule), m_pN4BiasCorrectionModule(w.m_pN4BiasCorrectionModule)
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
			mlInfo(__FUNCTION__) << "Started N4BiasField Correction";
			m_pN4BiasCorrectionModule->runN4BiasCorrection();

		}
		catch (...)
		{
		}

		callMethodOnGUI(m_ownerModule, &mialN4BiasField::postComputation);
	}

private:
	DistantObject<mialN4BiasField> m_ownerModule;
	mialN4BiasField *m_pN4BiasCorrectionModule;

};


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(mialN4BiasField, Module);

//----------------------------------------------------------------------------------

mialN4BiasField::mialN4BiasField() : Module(0, 0), m_pBGN4BiasCorrectionWorker(NULL), m_pBGN4BiasCorrectionWorkerThread(NULL)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add fields to the module and set their values.
  _inputFileFld = addString("inputFile", "");
  _maskFileFld = addString("maskFile", "");
  _outputFileFld = addString("outputFile", "");
  _outputBiasFieldFld = addString("outputBiasField", "");

  _statusFld = addString("status", "");
  _startTaskFld = addTrigger("startTask");
  _startTaskModalFld = addTrigger("startTaskModal");
  _inProgressFld = addBool("inProgress", false);
  _outputSucceedFld = addBool("outputSucceed", false);

  clear();

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();


}

void mialN4BiasField::clear()
{
	_outputSucceedFld->setBoolValue(false);

}

mialN4BiasField::~mialN4BiasField()
{
	if (m_pBGN4BiasCorrectionWorker)
	{
		delete (m_pBGN4BiasCorrectionWorker);
		m_pBGN4BiasCorrectionWorker = NULL;
	}

	if (m_pBGN4BiasCorrectionWorkerThread)
	{
		delete m_pBGN4BiasCorrectionWorkerThread;
		m_pBGN4BiasCorrectionWorkerThread;
	}
}


//----------------------------------------------------------------------------------

void mialN4BiasField::handleNotification(Field* field)
{
  // Handle changes of module parameters and input image fields here.
	if (field == _startTaskFld || field == _startTaskModalFld)
	{
		clear();

		if (field == _startTaskFld)
		{
			//kill le background worker si il exist:
			if (m_pBGN4BiasCorrectionWorker)
				std::cout << "N4BiasField Correction Worker killed" << std::endl;
			delete (m_pBGN4BiasCorrectionWorker);
			m_pBGN4BiasCorrectionWorker = new mialN4BiasFieldBackgroundTask(this);
			std::cout << "background task created" << std::endl;
			_inProgressFld->setBoolValue(true);
			_statusFld->setStringValue("N4BiasField Correction Running");
			if (m_pBGN4BiasCorrectionWorkerThread)
				std::cout << "N4BiasField Correction thread killed" << std::endl;
			delete m_pBGN4BiasCorrectionWorkerThread;
			m_pBGN4BiasCorrectionWorkerThread = new boost::thread(*m_pBGN4BiasCorrectionWorker);
		}
		else if (field == _startTaskModalFld)
		{
			runN4BiasCorrection();
		}

	}
}


void mialN4BiasField::postComputation()
{
	_inProgressFld->setBoolValue(false);
	_statusFld->setStringValue("N4BiasField Correction Done");
	//std::cout << "N4BiasField Correction Done" << std::endl;
}

void mialN4BiasField::runN4BiasCorrection()
{
	const char* tempOutput;
	std::string temp1Output;

	const char* tempInput;
	std::string temp1Input;

	const char* tempMask;
	std::string temp1Mask;

	const char* tempoutputBias;
	std::string temp1outputBias;


	temp1Output = _outputFileFld->getStringValue();
	tempOutput = temp1Output.c_str();

	temp1Mask = _maskFileFld->getStringValue();
	tempMask = temp1Mask.c_str();

	temp1Input = _inputFileFld->getStringValue();
	tempInput = temp1Input.c_str();

	temp1outputBias = _outputBiasFieldFld->getStringValue();
	tempoutputBias = temp1outputBias.c_str();

	mialsrtkN4BiasFieldCorrectionWrapper* useN4BiasFieldCorrectionWrapper = new mialsrtkN4BiasFieldCorrectionWrapper(tempInput, tempMask, tempOutput, tempoutputBias);

	std::cout << "wrapper construct" << std::endl;

	useN4BiasFieldCorrectionWrapper->runN4BiasFieldCorrection();
	if (useN4BiasFieldCorrectionWrapper->boolExit)
	{
		std::cout << "should have done N4 BiasField Correction" << std::endl;
	}
	else
	{
		mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "mialN4BiasField Failed : ";
	}

	_outputSucceedFld->setBoolValue(true);
}


ML_END_NAMESPACE