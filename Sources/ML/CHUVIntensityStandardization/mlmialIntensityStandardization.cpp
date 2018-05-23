//----------------------------------------------------------------------------------
//! The ML module class mialIntensityStandardization.
/*!
// \file   
// \author  FetalMRI
// \date    2018-05-22
//
// 
*/
//----------------------------------------------------------------------------------

#include "mlmialIntensityStandardization.h"
#include "mlMessagingBackgroundTask.h"
#include <omp.h>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>

ML_START_NAMESPACE

class mialIntensityStandardizationBackgroundTask : public MessagingBackgroundTask
{
public:
	mialIntensityStandardizationBackgroundTask(mialIntensityStandardization *pmialIntensityStandardizationModule) :MessagingBackgroundTask(static_cast<void *>(pmialIntensityStandardizationModule)),
		m_ownerModule(pmialIntensityStandardizationModule), m_pmialIntensityStandardizationModule(pmialIntensityStandardizationModule)
	{
	}

	mialIntensityStandardizationBackgroundTask(const mialIntensityStandardizationBackgroundTask &w) :MessagingBackgroundTask(static_cast<void *>(w.m_pmialIntensityStandardizationModule)),
		m_ownerModule(w.m_ownerModule), m_pmialIntensityStandardizationModule(w.m_pmialIntensityStandardizationModule)
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
			m_pmialIntensityStandardizationModule->IntensityStandardizationAllInput();

		}
		catch (...)
		{
		}

		callMethodOnGUI(m_ownerModule, &mialIntensityStandardization::postComputation);
	}

private:
	DistantObject<mialIntensityStandardization> m_ownerModule;
	mialIntensityStandardization *m_pmialIntensityStandardizationModule;

};

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(mialIntensityStandardization, Module);

//----------------------------------------------------------------------------------

mialIntensityStandardization::mialIntensityStandardization() : Module(0, 0), m_pBGmialIntensityStandardizationWorker(NULL), m_pmialIntensityStandardizationWorkerThread(NULL)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add fields to the module and set their values.
  _inputFileNamesFld = addString("inputFileNames", "");
  _outputFileNamesFld = addString("outputFileNames", "");
  _maxIntensityFld = addFloat("maxIntensity", 255.0);
  _statusFld = addString("status", "");
  _startTaskFld = addTrigger("startTask");
  _inProgressFld = addBool("inProgress", false);
  _ouputSucceedFld = addBool("ouputSucceed", false);

  clear();
  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();


}

mialIntensityStandardization::~mialIntensityStandardization()
{
	if (m_pBGmialIntensityStandardizationWorker)
	{
		delete (m_pBGmialIntensityStandardizationWorker);
		m_pBGmialIntensityStandardizationWorker = NULL;
	}

	if (m_pmialIntensityStandardizationWorkerThread)
	{
		delete m_pmialIntensityStandardizationWorkerThread;
		m_pmialIntensityStandardizationWorkerThread;
	}


}


//----------------------------------------------------------------------------------

void mialIntensityStandardization::handleNotification(Field* field)
{
  // Handle changes of module parameters and input image fields here.
	if (field == _startTaskFld)
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
		std::string s = _inputFileNamesFld->getStringValue() + "--";
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
		s = _outputFileNamesFld->getStringValue() + "--";
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//remove whitespace
			boost::trim(token);
			std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splitOutputs.push_back(token);
		}

		std::cout << "I have splitted my output" << std::endl;

		if (splitInputs.size() != splitOutputs.size())
		{
			mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "number of inputs/outputs are different";
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


		//kill le background worker si il exist:
		if (m_pBGmialIntensityStandardizationWorker)
			delete (m_pBGmialIntensityStandardizationWorker);
		m_pBGmialIntensityStandardizationWorker = new mialIntensityStandardizationBackgroundTask(this);
		std::cout << "background task created" << std::endl;
		_inProgressFld->setBoolValue(true);
		_statusFld->setStringValue("mial Intensity Standardization Running");
		if (m_pmialIntensityStandardizationWorkerThread)
			delete m_pmialIntensityStandardizationWorkerThread;
		m_pmialIntensityStandardizationWorkerThread = new boost::thread(*m_pBGmialIntensityStandardizationWorker);
		//touchOutputs = true;

	}
}

void mialIntensityStandardization::clear()
{
	//reset variable
	splitInputs.clear();
	splitOutputs.clear();
	std::cout << "dynamic vector cleared" << std::endl;
	_ouputSucceedFld->setBoolValue(false);

}

void mialIntensityStandardization::postComputation()
{
	_inProgressFld->setBoolValue(false);
	std::cout << "mial Intensity Standardization Done" << std::endl;

}


void mialIntensityStandardization::IntensityStandardizationAllInput()
{
	std::cout << "inputFiles" << splitInputs.size() << std::endl;

	//here we use openmp
	unsigned int iterIm = splitInputs.size();
	//omp_set_num_threads(7);
    //#pragma omp parallel for private(iterIm)
	//for (int iterIm = 0; iterIm < splitInputs.size(); iterIm++)
	//{

		//here I call the full constructor
		IntensityStandardizationWrapper* useIntensityStandardizationWrapper = new IntensityStandardizationWrapper(splitInputs,splitOutputs, _maxIntensityFld->getFloatValue());
		useIntensityStandardizationWrapper->runIntensityStandardization();
		if (useIntensityStandardizationWrapper->boolExit)
		{
			std::cout << "should have set intensity between 0 and " + std::to_string(_maxIntensityFld->getFloatValue()) << std::endl;
		}
		else
		{
			mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "correctSliceIntensity Failed : ";
		}
	//}

	_ouputSucceedFld->setBoolValue(true);

}


ML_END_NAMESPACE