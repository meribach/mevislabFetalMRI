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


#pragma once


#include "CHUVIntensityStandardizationSystem.h"

#include <mlModuleIncludes.h>
#include "mialsrtkN4BiasFieldCorrectionWrapper.h"

class mialsrtkN4BiasFieldCorrectionWrapper;

ML_START_NAMESPACE

class mialN4BiasFieldBackgroundTask;

//! 
class CHUVINTENSITYSTANDARDIZATION_EXPORT mialN4BiasField : public Module
{
	friend class mialN4BiasFieldBackgroundTask;
public:

  //! Constructor.
  mialN4BiasField();

  ~mialN4BiasField();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

private:

  // ----------------------------------------------------------
  //! \name Module field declarations
  //@{
  // ----------------------------------------------------------
  void clear();
  //! 
  StringField* _inputFileFld;
  //! 
  StringField* _maskFileFld;
  //! 
  StringField* _outputFileFld;
  //! 
  StringField* _outputBiasFieldFld;
  //@}


  StringField* _statusFld;
  NotifyField* _startTaskFld;
  NotifyField* _startTaskModalFld;
  BoolField* _inProgressFld;
  BoolField* _outputSucceedFld;

  // Implements interface for the runtime type system of the ML.

protected:
	void postComputation();
	void runN4BiasCorrection();
	mialN4BiasFieldBackgroundTask * m_pBGN4BiasCorrectionWorker;
	boost::thread *m_pBGN4BiasCorrectionWorkerThread;

  ML_MODULE_CLASS_HEADER(mialN4BiasField)
};


ML_END_NAMESPACE