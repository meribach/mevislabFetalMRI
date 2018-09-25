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


#pragma once


#include "CHUVIntensityStandardizationSystem.h"

#include <mlModuleIncludes.h>
#include "mialsrtkCorrectBiasFieldWithMotionAppliedWrapper.h"

class mialsrtkCorrectBiasFieldWithMotionAppliedWrapper;

ML_START_NAMESPACE

class mialCorrectBiasFieldWithMotionAppliedBackgroundTask;
//! 
class CHUVINTENSITYSTANDARDIZATION_EXPORT mialCorrectBiasFieldWithMotionApplied : public Module
{
	friend class mialCorrectBiasFieldWithMotionAppliedBackgroundTask;

public:

  //! Constructor.
  mialCorrectBiasFieldWithMotionApplied();
  virtual ~mialCorrectBiasFieldWithMotionApplied();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

private:

  // ----------------------------------------------------------
  //! \name Module field declarations
  //@{
  // ----------------------------------------------------------
  void clear();
  //! 
  StringField* _inputImageFileFld;
  //! 
  StringField* _maskImageFileFld;
  //! 
  StringField* _transformInFileFld;
  //! 
  StringField* _BiasFieldInFileFld;
  //! 
  StringField* _outputImageFileFld;
  //! 
  StringField* _BiasFieldOutputFileFld;
  //! 
  StringField* _statusFld;
  NotifyField* _startTaskFld;
  NotifyField* _startTaskModalFld;
  BoolField* _inProgressFld;
  BoolField* _outputSucceedFld;

  std::vector<std::string> splitInputs;
  std::vector<std::string> splitOutputs;
  std::vector<std::string> splitMasks;
  std::vector<std::string> splitinputBiasField;
  std::vector<std::string> splitBiasOut;
  std::vector<std::string> splittransformIn;
  //@}

protected:
	void postComputation();
	void CorrectBiasAllInput();
	mialCorrectBiasFieldWithMotionAppliedBackgroundTask * m_pBGmialCorrectBiasFieldWithMotionAppliedWorker;
	boost::thread *m_pmialCorrectBiasFieldWithMotionAppliedThread;
  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(mialCorrectBiasFieldWithMotionApplied)
};


ML_END_NAMESPACE