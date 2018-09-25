//----------------------------------------------------------------------------------
//! The ML module class mialCorrectBiasFieldWithMotionApplied.
/*!
// \file   
// \author  FetalMRI
// \date    2018-09-24
//
// 
*/
//----------------------------------------------------------------------------------

#include "mlmialCorrectBiasFieldWithMotionApplied.h"
#include "mlMessagingBackgroundTask.h"
#include <omp.h>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string.hpp>


ML_START_NAMESPACE

class mialCorrectBiasFieldWithMotionAppliedBackgroundTask : public MessagingBackgroundTask
{
public:
	mialCorrectBiasFieldWithMotionAppliedBackgroundTask(mialCorrectBiasFieldWithMotionApplied *pmialCorrectBiasFieldWithMotionAppliedModule) :MessagingBackgroundTask(static_cast<void *>(pmialCorrectBiasFieldWithMotionAppliedModule)),
		m_ownerModule(pmialCorrectBiasFieldWithMotionAppliedModule), m_pmialCorrectBiasFieldWithMotionAppliedModule(pmialCorrectBiasFieldWithMotionAppliedModule)
	{
	}

	mialCorrectBiasFieldWithMotionAppliedBackgroundTask(const mialCorrectBiasFieldWithMotionAppliedBackgroundTask &w) :MessagingBackgroundTask(static_cast<void *>(w.m_pmialCorrectBiasFieldWithMotionAppliedModule)),
		m_ownerModule(w.m_ownerModule), m_pmialCorrectBiasFieldWithMotionAppliedModule(w.m_pmialCorrectBiasFieldWithMotionAppliedModule)
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
			mlInfo(__FUNCTION__) << "Started Slice By Slice Bias Field Estimation";
			m_pmialCorrectBiasFieldWithMotionAppliedModule->CorrectBiasAllInput();

		}
		catch (...)
		{
		}

		callMethodOnGUI(m_ownerModule, &mialCorrectBiasFieldWithMotionApplied::postComputation);
	}

private:
	DistantObject<mialCorrectBiasFieldWithMotionApplied> m_ownerModule;
	mialCorrectBiasFieldWithMotionApplied *m_pmialCorrectBiasFieldWithMotionAppliedModule;

};

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(mialCorrectBiasFieldWithMotionApplied, Module);

//----------------------------------------------------------------------------------
mialCorrectBiasFieldWithMotionApplied::~mialCorrectBiasFieldWithMotionApplied()
{
	if (m_pBGmialCorrectBiasFieldWithMotionAppliedWorker)
	{
		delete (m_pBGmialCorrectBiasFieldWithMotionAppliedWorker);
		m_pBGmialCorrectBiasFieldWithMotionAppliedWorker = NULL;
	}

	if (m_pmialCorrectBiasFieldWithMotionAppliedThread)
	{
		delete m_pmialCorrectBiasFieldWithMotionAppliedThread;
		m_pmialCorrectBiasFieldWithMotionAppliedThread;
	}
}


mialCorrectBiasFieldWithMotionApplied::mialCorrectBiasFieldWithMotionApplied() : Module(0, 0), m_pBGmialCorrectBiasFieldWithMotionAppliedWorker(NULL), m_pmialCorrectBiasFieldWithMotionAppliedThread(NULL)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add fields to the module and set their values.
  _inputImageFileFld = addString("inputImageFile", "");
  _maskImageFileFld = addString("maskImageFile", "");
  _transformInFileFld = addString("transformInFile", "");
  _BiasFieldInFileFld = addString("BiasFieldInFile", "");
  _outputImageFileFld = addString("outputImageFile", "");
  _BiasFieldOutputFileFld = addString("BiasFieldOutputFile", "");
  _statusFld = addString("status", "");
  _startTaskFld = addTrigger("startTask");
  _startTaskModalFld = addTrigger("startTaskModal");
  _inProgressFld = addBool("inProgress", false);
  _outputSucceedFld = addBool("outputSucceed", false);

  clear();
  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();


  // Activate inplace data buffers for output outputIndex and input inputIndex.
  // setOutputImageInplace(outputIndex, inputIndex);

  // Activate page data bypass from input inputIndex to output outputIndex.
  // Note that the module must still be able to calculate the output image.
  // setBypass(outputIndex, inputIndex);

  // By default modules only support scalar voxel types. If you want to
  // support other voxel types, you need to enable the general support with
  //
  //   setVoxelDataTypeSupport(ALL_REGISTERED_TYPES);
  //
  // and explicitly invalidate the output image for unsupported input types in
  // calculateOutputImageProperties.
  // You also need to use other multiplexing macros than 
  // ML_CALCULATEOUTPUTSUBIMAGE_NUM_INPUTS_N/x_SCALAR_TYPES_CPP, or forego these macros
  // completely and override the non-templated calculateOutputSubImage method and
  // do the necessary type multiplexing yourself. (Hint: It might be easier to use
  // new-style output image handler modules instead)

}

//----------------------------------------------------------------------------------

void mialCorrectBiasFieldWithMotionApplied::handleNotification(Field* field)
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
		s = _maskImageFileFld->getStringValue() + "--";
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
		s = _BiasFieldInFileFld->getStringValue() + "--";
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//remove whitespace
			boost::trim(token);
			std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splitinputBiasField.push_back(token);
		}

		std::cout << "I have splitted my input bias field" << std::endl;
		
		pos = 0;
		token = "";
		s =	_transformInFileFld->getStringValue() + "--";
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//remove whitespace
			boost::trim(token);
			std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splittransformIn.push_back(token);
		}

		std::cout << "I have splitted my input transform" << std::endl;

		pos = 0;
		token = "";
		s = _BiasFieldOutputFileFld->getStringValue() + "--";
		while ((pos = s.find(delimiter)) != std::string::npos) {
			token = s.substr(0, pos);
			//remove whitespace
			boost::trim(token);
			std::cout << token << std::endl;
			s.erase(0, pos + delimiter.length());
			splitBiasOut.push_back(token);
		}

		std::cout << "I have splitted my output bias field" << std::endl;

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
			if (m_pBGmialCorrectBiasFieldWithMotionAppliedWorker)
				delete (m_pBGmialCorrectBiasFieldWithMotionAppliedWorker);
			m_pBGmialCorrectBiasFieldWithMotionAppliedWorker = new mialCorrectBiasFieldWithMotionAppliedBackgroundTask(this);
			std::cout << "background task created" << std::endl;
			_inProgressFld->setBoolValue(true);
			_statusFld->setStringValue("mial Bias Correction With Motion Running");
			if (m_pmialCorrectBiasFieldWithMotionAppliedThread)
				delete m_pmialCorrectBiasFieldWithMotionAppliedThread;
			m_pmialCorrectBiasFieldWithMotionAppliedThread = new boost::thread(*m_pBGmialCorrectBiasFieldWithMotionAppliedWorker);
			//touchOutputs = true;
		}
		else if (field == _startTaskModalFld)
		{
			CorrectBiasAllInput();
		}

	}
}


void mialCorrectBiasFieldWithMotionApplied::clear()
{
	splitInputs.clear();
	splitMasks.clear();
	splitOutputs.clear();
	splitinputBiasField.clear();
	splitBiasOut.clear();
	splittransformIn.clear();
	_outputSucceedFld->setBoolValue(false);
}

void mialCorrectBiasFieldWithMotionApplied::CorrectBiasAllInput()
{
	std::cout << "inputFiles" << splitInputs.size() << std::endl;

	//here we use openmp
	unsigned int iterIm = splitInputs.size();
	omp_set_num_threads(7);
    #pragma omp parallel for private(iterIm)
	for (int iterIm = 0; iterIm < splitInputs.size(); iterIm++)
	{

		//here I call the full constructor
		mialsrtkCorrectBiasFieldWithMotionAppliedWrapper* usemialsrtkCorrectBiasFieldWithMotionAppliedWrapper = new mialsrtkCorrectBiasFieldWithMotionAppliedWrapper(splitInputs[iterIm].c_str(), splitMasks[iterIm].c_str(), splittransformIn[iterIm].c_str(), splitinputBiasField[iterIm].c_str(), splitOutputs[iterIm].c_str(), splitBiasOut[iterIm].c_str());
		usemialsrtkCorrectBiasFieldWithMotionAppliedWrapper->runCorrectBiasFieldWithMotionApplied();
		if (usemialsrtkCorrectBiasFieldWithMotionAppliedWrapper->boolExit)
		{
			std::cout << "should have correct bias with motion applied" << splitInputs[iterIm].c_str() << std::endl;
		}
		else
		{
			mlError(__FUNCTION__, ML_UNKNOWN_EXCEPTION) << "Bias Correction With Motion Applied Failed : " << splitInputs[iterIm].c_str();
		}
	}

	_statusFld->setStringValue("Bias Correction With Motion Applied Done");
	_outputSucceedFld->setBoolValue(true);

}


void mialCorrectBiasFieldWithMotionApplied::postComputation()
{
	_inProgressFld->setBoolValue(false);
}


ML_END_NAMESPACE