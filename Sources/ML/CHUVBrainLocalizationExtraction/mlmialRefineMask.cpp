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


ML_START_NAMESPACE

class mialRefineMaskBackgroundTask : public MessagingBackgroundTask
{
public:
	mialRefineMaskBackgroundTask(mialTVSuperResolution *pmialTVSuperResolutionModule) :MessagingBackgroundTask(static_cast<void *>(pmialTVSuperResolutionModule)),
		m_ownerModule(pmialTVSuperResolutionModule), m_pmialTVSuperResolutionModule(pmialTVSuperResolutionModule)
	{
	}
	mialRefineMaskBackgroundTask(const mialSuperResolutionBackgroundTask &w) :MessagingBackgroundTask(static_cast<void *>(w.m_pmialTVSuperResolutionModule)),
		m_ownerModule(w.m_ownerModule), m_pmialTVSuperResolutionModule(w.m_pmialTVSuperResolutionModule)
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
			mlInfo(__FUNCTION__) << "Started TV SuperResolution";
			m_pmialTVSuperResolutionModule->runTVSuperResolution();

		}
		catch (...)
		{
		}

		callMethodOnGUI(m_ownerModule, &mialTVSuperResolution::postComputation);
	}

private:
	DistantObject<mialRefineMask> m_ownerModule;
	mialTVSuperResolution *m_pmialTVSuperResolutionModule;

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
  _radiusDilationFld = addBool("radiusDilation", true);
  _useStapleFld = addBool("useStaple", false);

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
  // Handle changes of module parameters and input image fields here.
}

ML_END_NAMESPACE