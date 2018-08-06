//----------------------------------------------------------------------------------
//! The ML module class mialsrtkMaskImage.
/*!
// \file   
// \author  FetalMRI
// \date    2018-08-06
//
// 
*/
//----------------------------------------------------------------------------------

#include "mlmialsrtkMaskImage.h"
#include "mialsrtkMaskImageWrapper.h"
#include "mlMessagingBackgroundTask.h"
#include <omp.h>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>

ML_START_NAMESPACE

class mialsrtkMaskImageBackgroundTask : public MessagingBackgroundTask
{
public:
	mialsrtkMaskImageBackgroundTask(mialsrtkMaskImage *pmialmaskImageModule) :MessagingBackgroundTask(static_cast<void *>(pmialmaskImageModule)),
		m_ownerModule(pmialmaskImageModule), m_pmialmaskImageModule(pmialmaskImageModule)
	{
	}

	mialsrtkMaskImageBackgroundTask(const mialsrtkMaskImageBackgroundTask &w) :MessagingBackgroundTask(static_cast<void *>(w.m_pmialmaskImageModule)),
		m_ownerModule(w.m_ownerModule), m_pmialmaskImageModule(w.m_pmialmaskImageModule)
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
			mlInfo(__FUNCTION__) << "Started masking Image";
			m_pmialmaskImageModule->runmaskImage();

		}
		catch (...)
		{
		}

		callMethodOnGUI(m_ownerModule, &mialsrtkMaskImage::postComputation);
	}

private:
	DistantObject<mialsrtkMaskImage> m_ownerModule;
	mialsrtkMaskImage *m_pmialmaskImageModule;

	//OrientImageResult _result;
};

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(mialsrtkMaskImage, Module);

//----------------------------------------------------------------------------------

mialsrtkMaskImage::mialsrtkMaskImage() : Module(0, 0), m_pBGMaskImageWorker(NULL), m_pBGMaskImageWorkerThread(NULL)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add fields to the module and set their values.
  _inputFileFld = addString("inputFile", "");
  _maskFileFld = addString("maskFile", "");
  _outputFileFld = addString("outputFile", "");

  _statusFld = addString("status", "");
  _startTaskFld = addTrigger("startTask");
  _startTaskModalFld = addTrigger("startTaskModal");
  _inProgressFld = addBool("inProgress", false);
  _outputSucceedFld = addBool("outputSucceed", false);

  clear();

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();


}


void mialsrtkMaskImage::clear()
{
	_outputSucceedFld->setBoolValue(false);
}


mialsrtkMaskImage::~mialsrtkMaskImage()
{
	if (m_pBGMaskImageWorker)
	{
		delete (m_pBGMaskImageWorker);
		m_pBGMaskImageWorker = NULL;
	}

	if (m_pBGMaskImageWorkerThread)
	{
		delete m_pBGMaskImageWorkerThread;
		m_pBGMaskImageWorkerThread;
	}
}

//----------------------------------------------------------------------------------

void mialsrtkMaskImage::handleNotification(Field* field)
{
  // Handle changes of module parameters and input image fields here.
	if (field == _startTaskFld || field == _startTaskModalFld)
	{
		clear();

		if (field == _startTaskFld)
		{
			//kill le background worker si il exist:
			if (m_pBGMaskImageWorker)
				std::cout << "masking Image Worker killed" << std::endl;
			delete (m_pBGMaskImageWorker);
			m_pBGMaskImageWorker = new mialsrtkMaskImageBackgroundTask(this);
			std::cout << "background task created" << std::endl;
			_inProgressFld->setBoolValue(true);
			_statusFld->setStringValue("masking Image Running");
			if (m_pBGMaskImageWorkerThread)
				std::cout << "masking image thread killed" << std::endl;
			delete m_pBGMaskImageWorkerThread;
			m_pBGMaskImageWorkerThread = new boost::thread(*m_pBGMaskImageWorker);
		}
		else if (field == _startTaskModalFld)
		{
			runmaskImage();
		}

	}
}

void mialsrtkMaskImage::runmaskImage()
{
	const char* tempOutput;
	std::string temp1Output;

	const char* tempInput;
	std::string temp1Input;

	const char* tempMask;
	std::string temp1Mask;

	temp1Output = _outputFileFld->getStringValue();
	tempOutput = temp1Output.c_str();

	temp1Mask = _maskFileFld->getStringValue();
	tempMask = temp1Mask.c_str();

	temp1Input = _inputFileFld->getStringValue();
	tempInput = temp1Input.c_str();


	mialsrtkMaskImageWrapper* usemialsrtkMaskImageWrapper = new mialsrtkMaskImageWrapper(tempInput, tempMask, tempOutput);

	std::cout << "wrapper construct" << std::endl;

	usemialsrtkMaskImageWrapper->runMaskImage();
	if (usemialsrtkMaskImageWrapper->boolExit)
	{
		std::cout << "should have done mialsrtkMaskImage" << std::endl;
	}
	else
	{
		mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "mialsrtkMaskImage Failed : ";
	}

	_outputSucceedFld->setBoolValue(true);
}


void mialsrtkMaskImage::postComputation()
{
	_inProgressFld->setBoolValue(false);
	_statusFld->setStringValue("Masking Image Done");
	std::cout << "Masking Image Done" << std::endl;
}

ML_END_NAMESPACE