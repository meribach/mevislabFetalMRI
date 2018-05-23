//----------------------------------------------------------------------------------
//! The ML module class mevisbtkDenoising.
/*!
// \file   
// \author  FetalMRI
// \date    2018-05-09
//
// 
*/
//----------------------------------------------------------------------------------

#include "mlmevisbtkDenoising.h"
#include "btkNLMDenoisingWrapper.h"
#include "mlMessagingBackgroundTask.h"
#include <omp.h>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>

ML_START_NAMESPACE

class BTKDenoiseBackgroundTask : public MessagingBackgroundTask
{
public:
	BTKDenoiseBackgroundTask(mevisbtkDenoising *pmevisbtkDenoisingModule) :MessagingBackgroundTask(static_cast<void *>(pmevisbtkDenoisingModule)),
		m_ownerModule(pmevisbtkDenoisingModule), m_pmevisbtkDenoisingModule(pmevisbtkDenoisingModule)
	{
	}

	BTKDenoiseBackgroundTask(const BTKDenoiseBackgroundTask &w) :MessagingBackgroundTask(static_cast<void *>(w.m_pmevisbtkDenoisingModule)),
		m_ownerModule(w.m_ownerModule), m_pmevisbtkDenoisingModule(w.m_pmevisbtkDenoisingModule)
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
			mlInfo(__FUNCTION__) << "Started BTKDenoising";
			m_pmevisbtkDenoisingModule->DenoiseAllInput();

		}
		catch (...)
		{
		}

		callMethodOnGUI(m_ownerModule, &mevisbtkDenoising::postComputation);
	}

private:
	DistantObject<mevisbtkDenoising> m_ownerModule;
	mevisbtkDenoising *m_pmevisbtkDenoisingModule;

	//OrientImageResult _result;
};


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(mevisbtkDenoising, Module);

//----------------------------------------------------------------------------------

mevisbtkDenoising::mevisbtkDenoising() : Module(0, 0), m_pBGBTKDenoiseWorker(NULL), m_pBTKDenoiseWorkerThread(NULL)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add fields to the module and set their values.
  _inputFileNameFld = addString("inputFileName", "");
  _outputFileNameFld = addString("outputFileName", "");
  _maskFileNameFld = addString("maskFileName", "");
  _referenceFileNameFld = addString("referenceFileName", "");
  _paddingValueFld = addFloat("paddingValue", 0);
  _patchHalfSizeFld = addInt("patchHalfSize", 1);
  _halfsizeVolumeSearchAreaFld = addInt("halfsizeVolumeSearchArea", 5);
  _smoothingParameterBetaFld = addFloat("smoothingParameterBeta", 1);
  static const char * const blockValues[] = { "0", "1", "2" };
  _blockFld = addEnum("block", blockValues, 3);
  _blockFld->setEnumValue(1);
  _optFld = addBool("opt", true);
  _lowerMeanThresholdFld = addFloat("lowerMeanThreshold", 0.95);
  _lowerVarianceThresholdFld = addFloat("lowerVarianceThreshold", 0.5);
  _differenceFileFld = addString("differenceFile", "");
  _smoothingParameterEstimationFld = addBool("smoothingParameterEstimation", false);
  static const char * const centerValues[] = { "0", "1", "-1" };
  _centerFld = addEnum("center", centerValues, 3);
  _centerFld->setEnumValue(2);
  _statusFld = addString("status", "");
  _startTaskFld = addTrigger("startTask");
  _inProgressFld = addBool("inProgress", false);
  _ouputSucceedFld = addBool("ouputSucceed", false);

  clear();

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();


}

//----------------------------------------------------------------------------------

void mevisbtkDenoising::handleNotification(Field* field)
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
	std::string s = _inputFileNameFld->getStringValue() + "--";
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
	s = _outputFileNameFld->getStringValue() + "--";
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		//remove whitespace
		boost::trim(token);
		std::cout << token << std::endl;
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
			std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splitMasks.push_back(token);
		}
	}

    //test if refs and diff are ""
	//if not I have to split and check the size
	if (_referenceFileNameFld->getStringValue() == "")
	{
		std::cout << "no refs" << std::endl;
		//I generate a std vector refs with empty strings
		for (int i = 0; i < splitInputs.size(); i++) {
			splitRefs.push_back("");
		}
	}
	else
	{
		pos = 0;
		token = "";
		s = _referenceFileNameFld->getStringValue() + "--";
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//remove whitespace
			boost::trim(token);
			std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splitRefs.push_back(token);
		}

	}

	if (_differenceFileFld->getStringValue() == "")
	{
		std::cout << "no diffFile" << std::endl;
		//I generate a std vector diff with empty strings
		for (int i = 0; i < splitInputs.size(); i++) {
			splitDiffs.push_back("");
		}
	}
	else
	{
		pos = 0;
		token = "";
		s = _differenceFileFld->getStringValue() + "--";
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//remove whitespace
			boost::trim(token);
			std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splitDiffs.push_back(token);
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


		//kill le background worker si il exist:
		if (m_pBGBTKDenoiseWorker)
			delete (m_pBGBTKDenoiseWorker);
		m_pBGBTKDenoiseWorker = new BTKDenoiseBackgroundTask(this);
		std::cout << "background task created" << std::endl;
		_inProgressFld->setBoolValue(true);
		_statusFld->setStringValue("Denoising Running");
		if (m_pBTKDenoiseWorkerThread)
			delete m_pBTKDenoiseWorkerThread;
		m_pBTKDenoiseWorkerThread = new boost::thread(*m_pBGBTKDenoiseWorker);
		//touchOutputs = true;

		//DenoiseAllInput();
	}
}


mevisbtkDenoising::~mevisbtkDenoising()
{
	if (m_pBGBTKDenoiseWorker)
	{
		delete (m_pBGBTKDenoiseWorker);
		m_pBGBTKDenoiseWorker = NULL;
	}

	if (m_pBTKDenoiseWorkerThread)
	{
		delete m_pBTKDenoiseWorkerThread;
		m_pBTKDenoiseWorkerThread;
	}

}

void mevisbtkDenoising::clear()
{
	//reset variable
	splitInputs.clear();
	splitMasks.clear();
	splitOutputs.clear();
	splitRefs.clear();
	splitDiffs.clear();
	std::cout << "dynamic vector cleared" << std::endl;
	_ouputSucceedFld->setBoolValue(false);

}


void mevisbtkDenoising::DenoiseAllInput()
{
	std::cout << "inputFiles" << splitInputs.size() << std::endl;

	//here we use openmp
	unsigned int iterIm = splitInputs.size();
	omp_set_num_threads(7);
    #pragma omp parallel for private(iterIm)
	for (int iterIm = 0; iterIm < splitInputs.size(); iterIm++)
	{
	
        //here I call the full constructor
		btkNLMDenoisingWrapper* usebtkDenoiseWrapper = new btkNLMDenoisingWrapper(splitInputs[iterIm].c_str(), splitOutputs[iterIm].c_str(),
			splitMasks[iterIm].c_str(), splitRefs[iterIm].c_str(), _paddingValueFld->getFloatValue(), _patchHalfSizeFld->getIntValue(),
			_halfsizeVolumeSearchAreaFld->getIntValue(),_smoothingParameterBetaFld->getFloatValue(), _blockFld->getEnumValue(),
			_centerFld->getEnumValue(), int(_optFld->getBoolValue()), _lowerMeanThresholdFld->getFloatValue(), _lowerVarianceThresholdFld->getFloatValue(),
			splitDiffs[iterIm].c_str(), int(_smoothingParameterEstimationFld->getBoolValue()));
		usebtkDenoiseWrapper->runDenoise();
		if (usebtkDenoiseWrapper->boolExit)
		{
			std::cout << "should have denoise" << splitInputs[iterIm].c_str() << std::endl;
		}
		else
		{
			mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "btkDenoising Failed : " << splitInputs[iterIm].c_str();
		}
	}

	_ouputSucceedFld->setBoolValue(true);
}


void mevisbtkDenoising::postComputation()
{
	/*
	if (!task->isCanceled())
	{
	// Get the handler from the finished task:
	ProcessAllPagesHandler* handler = static_cast<ProcessAllPagesBackgroundTask*>(task)->getHandler();

	// Cast the handler to the derived class:
	ProcessAllPagesExampleHandler* myHandler = static_cast<ProcessAllPagesExampleHandler*>(handler);

	mlInfo("taskFinished") << "Result min: " << myHandler->getMin() << " max: " << myHandler->getMax();

	// We are in the GUI thread, so it is safe to set the fields with the results:
	_minField->setDoubleValue(myHandler->getMin());
	_maxField->setDoubleValue(myHandler->getMax());
	}
	else
	{
	mlInfo("taskFinished") << "Task was canceled";

	_minField->setDoubleValue(0);
	_maxField->setDoubleValue(0);
	}
	*/
	//BackgroundTaskBaseModule::taskFinished(task);
	_inProgressFld->setBoolValue(false);
	_statusFld->setStringValue("Denoising Done");
	std::cout << "btkDenoising Done" << std::endl;
}

ML_END_NAMESPACE