//----------------------------------------------------------------------------------
//! The ML module class mialOrientImage.
/*!
// \file   
// \author  FetalMRI
// \date    2018-04-12
//
// 
*/
//----------------------------------------------------------------------------------

#include "mlmialOrientImage.h"
#include "mialsrtkOrientImageWrapper.h"
//#include "mlModuleBackgroundTask.h"
#include "mlMessagingBackgroundTask.h"
#include <omp.h>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>

ML_START_NAMESPACE

class OrientImageBackgroundTask : public MessagingBackgroundTask
{
public:
	OrientImageBackgroundTask(mialOrientImage *pmialOrientImageModule):MessagingBackgroundTask(static_cast<void *>(pmialOrientImageModule)),
		m_ownerModule(pmialOrientImageModule), m_pmialOrientImageModule(pmialOrientImageModule)
	{
	}

	OrientImageBackgroundTask(const OrientImageBackgroundTask &w):MessagingBackgroundTask(static_cast<void *>(w.m_pmialOrientImageModule)),
		m_ownerModule(w.m_ownerModule), m_pmialOrientImageModule(w.m_pmialOrientImageModule)
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
			mlInfo(__FUNCTION__) << "Started Reorientation";
			m_pmialOrientImageModule->reOrientAllInput();
			
		}
		catch(...)
		{
		}

		callMethodOnGUI(m_ownerModule, &mialOrientImage::postComputation);
	}

private:
	DistantObject<mialOrientImage> m_ownerModule;
	mialOrientImage *m_pmialOrientImageModule;

	//OrientImageResult _result;
};


//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(mialOrientImage, Module);

//----------------------------------------------------------------------------------

mialOrientImage::mialOrientImage() : Module(0, 0), m_pBGOrientImageWorker(NULL), m_pOrientImageWorkerThread(NULL)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add fields to the module and set their values.
  _inputFileNameFld = addString("inputFileName", "");
  _outputFileNameFld = addString("outputFileName", "");
  _orientationFdl = addString("orientation", "axial");
  _statusFld = addString("status", "");
  _startTaskFld = addTrigger("startTask");
  _inProgressFld = addBool("inProgress",false);
  _ouputSucceedFld = addBool("ouputSucceed",false);

  // Reactivate calls of handleNotification on field changes.
  clear();

  handleNotificationOn();

}


mialOrientImage::~mialOrientImage()
{
	if (m_pBGOrientImageWorker)
	{
		delete (m_pBGOrientImageWorker);
		m_pBGOrientImageWorker = NULL;
	}

	if (m_pOrientImageWorkerThread)
	{
		delete m_pOrientImageWorkerThread;
		m_pOrientImageWorkerThread;
	}

}

void mialOrientImage::clear()
{
	//reset variable
	splitInputs.clear();
	splitOrientation.clear();
	splitOutputs.clear();
	std::cout << "dynamic vector cleared" << std::endl;
	_ouputSucceedFld->setBoolValue(false);

}

//----------------------------------------------------------------------------------

void mialOrientImage::handleNotification(Field* field)
{
  // Handle changes of module parameters and input image fields here.
  bool touchOutputs = false;
  //BackgroundTaskBaseModule::handleNotification(field);

  if (field == _inputFileNameFld)
  {
    touchOutputs = true;
  }
  else if (field == _outputFileNameFld)
  {
    touchOutputs = true;
  }

  else if (field == _startTaskFld)
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
	  std::string s = _inputFileNameFld->getStringValue()+"--";
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


	  //orientation to end
	  pos = 0;
	  token = "";
	  s = _orientationFdl->getStringValue() + "--";
	  while ((pos = s.find(delimiter)) != std::string::npos) {
		  token = s.substr(0, pos);
		  //remove whitespace
		  boost::trim(token);
		  std::cout << token << std::endl;
		  s.erase(0, pos + delimiter.length());
		  splitOrientation.push_back(token);
	  }

	  std::cout << "I have splitted my orientation" << std::endl;

	  if (splitOrientation.size() != splitInputs.size() || splitInputs.size()!= splitOutputs.size() || splitOrientation.size() != splitOutputs.size())
	  {
		mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "number of inputs/outputs/orientation are different";
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
	  //std::cout << _orientationFdl->getStringValue() << std::endl;
	  //std::cout << _orientationFdl->getStringValue().compare("axial") << std::endl;
	  //loop on the std::vector splitOrientations
	  for (std::vector<int>::size_type i = 0; i != splitOrientation.size(); i++)
	  {
	    if (splitOrientation[i].compare("axial") != 0 && splitOrientation[i].compare("sagittal") != 0 && splitOrientation[i].compare("coronal") != 0)
	    {
		  mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "Orientation not Recognized, must be axial, sagittal or coronal  :  " << splitOrientation[i];
		  return;
	    }
	  }
	  std::cout << "Orientations Recognized" << std::endl;
	 
	  //kill le background worker si il exist:
	  if (m_pBGOrientImageWorker)
		  delete (m_pBGOrientImageWorker);
	  m_pBGOrientImageWorker = new OrientImageBackgroundTask(this);
	  _inProgressFld->setBoolValue(true);
	  _statusFld->setStringValue("mialOrientation running");
	  if (m_pOrientImageWorkerThread)
		  delete m_pOrientImageWorkerThread;
	  m_pOrientImageWorkerThread = new boost::thread(*m_pBGOrientImageWorker);
	  //touchOutputs = true;

	  //reOrientAllInput();
  }

}


void mialOrientImage::reOrientAllInput()
{

	std::cout << "inputFiles" << splitInputs.size() << std::endl;

	//here we use openmp
	unsigned int iterIm = splitInputs.size();
	omp_set_num_threads(7);
    #pragma omp parallel for private(iterIm)
	for (int iterIm = 0; iterIm < splitInputs.size(); iterIm++)
	{
		//std::cout << splitInputs[iterIm] << std::endl;
		//std::cout << splitOrientation[iterIm] << std::endl;
		//std::cout << splitOutputs[iterIm] << std::endl;
		//std::cout << "should reorient" << std::endl;
		reOrientWrapper* useReorientWrapper = new reOrientWrapper(splitInputs[iterIm].c_str(), splitOutputs[iterIm].c_str(), splitOrientation[iterIm].c_str());
		useReorientWrapper->reOrient();
		if (useReorientWrapper->boolExit)
		{
		  std::cout << "should have reorient" << splitInputs[iterIm].c_str() <<std::endl;
		}
		else
		{
			mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "Reorient Failed : " << splitInputs[iterIm].c_str();
		}
		
		//delete useReorientWrapper;
		//useReorientWrapper = NULL;
	}

	_ouputSucceedFld->setBoolValue(true);
}



//----------------------------------------------------------------------------------
//! Handle the finished task. This is called in the GUI thread and
//! allows to pass the results of the task back to the module.
//! The task can either have the status BackgroundTask::Finished
//! or BackgroundTask::Canceled at this stage.
//----------------------------------------------------------------------------------
void mialOrientImage::postComputation()
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
	std::cout << "mialOrientation Done" << std::endl;
	_statusFld->setStringValue("mialOrientation Done");
}

ML_END_NAMESPACE