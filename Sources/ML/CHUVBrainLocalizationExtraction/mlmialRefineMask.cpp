//----------------------------------------------------------------------------------
//! The ML module class mialRefineMask.
/*!
// \file   
// \author  FetalMRI
// \date    2018-07-17
//
// 
*/
//----------------------------------------------------------------------------------

#include "mlmialRefineMask.h"
#include "mialsrtkRefineHRMaskByIntersectionWrapper.h"
#include "mlMessagingBackgroundTask.h"
#include <omp.h>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>

ML_START_NAMESPACE

class mialRefineMaskBackgroundTask : public MessagingBackgroundTask
{
public:
	mialRefineMaskBackgroundTask(mialRefineMask *pmialRefineMaskModule) :MessagingBackgroundTask(static_cast<void *>(pmialRefineMaskModule)),
		m_ownerModule(pmialRefineMaskModule), m_pmialRefineMaskModule(pmialRefineMaskModule)
	{
	}
	mialRefineMaskBackgroundTask(const mialRefineMaskBackgroundTask &w) :MessagingBackgroundTask(static_cast<void *>(w.m_pmialRefineMaskModule)),
		m_ownerModule(w.m_ownerModule), m_pmialRefineMaskModule(w.m_pmialRefineMaskModule)
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
			mlInfo(__FUNCTION__) << "Started Refine Mask";
			m_pmialRefineMaskModule->runRefineMask();

		}
		catch (...)
		{
		}

		callMethodOnGUI(m_ownerModule, &mialRefineMask::postComputation);
	}

private:
	DistantObject<mialRefineMask> m_ownerModule;
	mialRefineMask *m_pmialRefineMaskModule;

};

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(mialRefineMask, Module);

//----------------------------------------------------------------------------------

mialRefineMask::mialRefineMask() : Module(0, 0), m_pBGRefineMaskWorker(NULL), m_pBGRefineMaskWorkerThread(NULL)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add fields to the module and set their values.
  _inputFilesFld = addString("inputFiles", "");
  _transformInFld = addString("transformIn", "");
  _maskFilesFld = addString("maskFiles", "");
  _outputHRFileFld = addString("outputHRFile", "");
  _outputLRFilesFld = addString("outputLRFiles", "");
  _referenceFileFld = addString("referenceFile", "");
  _radiusDilationFld = addInt("radiusDilation", true);
  _useStapleFld = addBool("useStaple", true);

  _statusFld = addString("status", "");
  _startTaskFld = addTrigger("startTask");
  _startTaskModalFld = addTrigger("startTaskModal");
  _inProgressFld = addBool("inProgress", false);
  _outputSucceedFld = addBool("outputSucceed", false);

  clear();

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();


}

//----------------------------------------------------------------------------------
void mialRefineMask::clear()
{
	//reset variable
	splitinputFile.clear();
	splitmaskFile.clear();
	splittransformin.clear();
	splitoutputLRFile.clear();
	std::cout << "dynamic vector cleared" << std::endl;
	_outputSucceedFld->setBoolValue(false);

}

mialRefineMask::~mialRefineMask()
{
	if (m_pBGRefineMaskWorker)
	{
		delete (m_pBGRefineMaskWorker);
		m_pBGRefineMaskWorker = NULL;
	}

	if (m_pBGRefineMaskWorkerThread)
	{
		delete m_pBGRefineMaskWorkerThread;
		m_pBGRefineMaskWorkerThread;
	}
}

void mialRefineMask::handleNotification(Field* field)
{
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
		s = _transformInFld->getStringValue() + "--";
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//remove whitespace
			boost::trim(token);
			//std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splittransformin.push_back(token);
		}

		pos = 0;
		token = "";
		s = _outputLRFilesFld->getStringValue() + "--";
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//remove whitespace
			boost::trim(token);
			//std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splitoutputLRFile.push_back(token);
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

		if (splitinputFile.size() != splittransformin.size() || splitinputFile.size() != splitmaskFile.size())
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
			if (m_pBGRefineMaskWorker)
				std::cout << "Refine Mask Worker killed" << std::endl;
			delete (m_pBGRefineMaskWorker);
			m_pBGRefineMaskWorker = new mialRefineMaskBackgroundTask(this);
			std::cout << "background task created" << std::endl;
			_inProgressFld->setBoolValue(true);
			if (m_pBGRefineMaskWorkerThread)
				std::cout << "Refine Mask thread killed" << std::endl;
			delete m_pBGRefineMaskWorkerThread;
			m_pBGRefineMaskWorkerThread = new boost::thread(*m_pBGRefineMaskWorker);
			_statusFld->setStringValue("Refine Mask Running");
		}
		else if (field == _startTaskModalFld)
		{
			runRefineMask();
		}
	}
}


void mialRefineMask::runRefineMask()
{


	const char* tempOutput;
	std::string temp1Output;

	const char* temprefFile;
	std::string temp1refFile;

	const char* temprefMask;
	std::string temp1refMask;

	const char* tempDebugDir;
	std::string temp1DebugDir;


	temp1Output = _outputHRFileFld->getStringValue();
	tempOutput = temp1Output.c_str();

	temp1refFile = _referenceFileFld->getStringValue();
	temprefFile = temp1refFile.c_str();

	mialsrtkRefineHRMaskByIntersectionWrapper* useRefineMaskWrapper = new mialsrtkRefineHRMaskByIntersectionWrapper(splitinputFile, splittransformin, splitmaskFile, tempOutput, splitoutputLRFile, temprefFile, _radiusDilationFld->getIntValue(), _useStapleFld->getBoolValue());

	std::cout << "wrapper construct" << std::endl;

	useRefineMaskWrapper->runRefineMask();
	if (useRefineMaskWrapper->boolExit)
	{
		std::cout << "should have done Refine Mask" << std::endl;
	}
	else
	{
		mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "mialRefineMask Failed : ";
	}

	_outputSucceedFld->setBoolValue(true);
	delete useRefineMaskWrapper;
	useRefineMaskWrapper = NULL;


}


void mialRefineMask::postComputation()
{
	_inProgressFld->setBoolValue(false);
	_statusFld->setStringValue("Refine Mask Done");
	std::cout << "Refine Mask Done" << std::endl;
}



ML_END_NAMESPACE