//----------------------------------------------------------------------------------
//! The ML module class mialCorrectSliceIntensity.
/*!
// \file   
// \author  FetalMRI
// \date    2018-05-18
//
// 
*/
//----------------------------------------------------------------------------------

#include "mlmialCorrectSliceIntensity.h"
#include "mialsrtkCorrectSliceIntensityWrapper.h"
#include "mlMessagingBackgroundTask.h"
#include <omp.h>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>

ML_START_NAMESPACE

class mialCorrectSliceIntensityBackgroundTask : public MessagingBackgroundTask
{
public:
	mialCorrectSliceIntensityBackgroundTask(mialCorrectSliceIntensity *pmialCorrectSliceIntensityModule) :MessagingBackgroundTask(static_cast<void *>(pmialCorrectSliceIntensityModule)),
		m_ownerModule(pmialCorrectSliceIntensityModule), m_pmialCorrectSliceIntensityModule(pmialCorrectSliceIntensityModule)
	{
	}

	mialCorrectSliceIntensityBackgroundTask(const mialCorrectSliceIntensityBackgroundTask &w) :MessagingBackgroundTask(static_cast<void *>(w.m_pmialCorrectSliceIntensityModule)),
		m_ownerModule(w.m_ownerModule), m_pmialCorrectSliceIntensityModule(w.m_pmialCorrectSliceIntensityModule)
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
			m_pmialCorrectSliceIntensityModule->CorrectSliceIntensityAllInput();

		}
		catch (...)
		{
		}

		callMethodOnGUI(m_ownerModule, &mialCorrectSliceIntensity::postComputation);
	}

private:
	DistantObject<mialCorrectSliceIntensity> m_ownerModule;
	mialCorrectSliceIntensity *m_pmialCorrectSliceIntensityModule;

};


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(mialCorrectSliceIntensity, Module);

//----------------------------------------------------------------------------------

mialCorrectSliceIntensity::mialCorrectSliceIntensity() : Module(0, 0), m_pBGmialCorrectSliceIntensityWorker(NULL), m_pmialCorrectSliceIntensityWorkerThread(NULL)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add fields to the module and set their values.
  _inputFileNameFld = addString("inputFileName", "");
  _outputFileNameFld = addString("outputFileName", "");
  _maskFileNameFld = addString("maskFileName", "");
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
mialCorrectSliceIntensity::~mialCorrectSliceIntensity()
{
	if (m_pBGmialCorrectSliceIntensityWorker)
	{
		delete (m_pBGmialCorrectSliceIntensityWorker);
		m_pBGmialCorrectSliceIntensityWorker = NULL;
	}

	if (m_pmialCorrectSliceIntensityWorkerThread)
	{
		delete m_pmialCorrectSliceIntensityWorkerThread;
		m_pmialCorrectSliceIntensityWorkerThread;
	}

}



void mialCorrectSliceIntensity::handleNotification(Field* field)
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
		std::string s = _inputFileNameFld->getStringValue() + "--";
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//remove whitespace
			boost::trim(token);
			//std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splitInputs.push_back(token);
		}

		std::cout << "I have splitted my input" << std::endl;

		//then outputfilename
		pos = 0;
		token = "";
		s = _outputFileNameFld->getStringValue() + "--";
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//remove whitespace
			boost::trim(token);
			//std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splitOutputs.push_back(token);
		}

		std::cout << "I have splitted my output" << std::endl;



		if (_maskFileNameFld->getStringValue() == "")
		{
			std::cout << "no maskFiles" << std::endl;
			//I generate a std vector mask with empty strings same size as the input one
			for (int i = 0; i < splitInputs.size(); i++) {
				splitMasks.push_back("");
			}
		}
		else
		{
			//mask to end
			pos = 0;
			token = "";
			s = _maskFileNameFld->getStringValue() + "--";
			while ((pos = s.find(delimiter)) != std::string::npos) {
				token = s.substr(0, pos);
				//remove whitespace
				boost::trim(token);
				//std::cout << token << std::endl;
				s.erase(0, pos + delimiter.length());
				splitMasks.push_back(token);
			}
		}

		if (splitMasks.size() != splitInputs.size() || splitInputs.size() != splitOutputs.size() || splitMasks.size() != splitOutputs.size())
		{
			mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "number of inputs/outputs/masks are different";
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
			if (m_pBGmialCorrectSliceIntensityWorker)
				delete (m_pBGmialCorrectSliceIntensityWorker);
			m_pBGmialCorrectSliceIntensityWorker = new mialCorrectSliceIntensityBackgroundTask(this);
			std::cout << "background task created" << std::endl;
			_inProgressFld->setBoolValue(true);
			_statusFld->setStringValue("mial Correct Slice Intensity Running");
			if (m_pmialCorrectSliceIntensityWorkerThread)
				delete m_pmialCorrectSliceIntensityWorkerThread;
			m_pmialCorrectSliceIntensityWorkerThread = new boost::thread(*m_pBGmialCorrectSliceIntensityWorker);
			//touchOutputs = true;
		}
		else if (field == _startTaskModalFld)
		{
			CorrectSliceIntensityAllInput();
		}
	}

}


void mialCorrectSliceIntensity::clear()
{
	//reset variable
	splitInputs.clear();
	splitMasks.clear();
	splitOutputs.clear();
	std::cout << "mialCorrectSliceIntensity dynamic vector cleared" << std::endl;
	_outputSucceedFld->setBoolValue(false);

}

void mialCorrectSliceIntensity::postComputation()
{
	_inProgressFld->setBoolValue(false);
	//std::cout << "mial Correct Slice Intensity Done" << std::endl;

}

void mialCorrectSliceIntensity::CorrectSliceIntensityAllInput()
{
	std::cout << "inputFiles" << splitInputs.size() << std::endl;

	//here we use openmp
	unsigned int iterIm = splitInputs.size();
	omp_set_num_threads(7);
    #pragma omp parallel for private(iterIm)
	for (int iterIm = 0; iterIm < splitInputs.size(); iterIm++)
	{

		//here I call the full constructor
		CorrectSliceIntensityWrapper* useCorrectSliceIntensityWrapper = new CorrectSliceIntensityWrapper(splitInputs[iterIm].c_str(), splitMasks[iterIm].c_str(), splitOutputs[iterIm].c_str());
		useCorrectSliceIntensityWrapper->SliceIntensityCorrection();
		if (useCorrectSliceIntensityWrapper->boolExit)
		{
			std::cout << "should have correct slice intensity" << splitInputs[iterIm].c_str() << std::endl;
		}
		else
		{
			mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "correctSliceIntensity Failed : " << splitInputs[iterIm].c_str();
		}
		delete useCorrectSliceIntensityWrapper;
		useCorrectSliceIntensityWrapper = NULL;
	}


	_statusFld->setStringValue("mial Correct Slice Intensity Done");
	_outputSucceedFld->setBoolValue(true);

}


ML_END_NAMESPACE