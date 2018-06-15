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


#pragma once


#include "CHUVIntensityStandardizationSystem.h"

#include <mlModuleIncludes.h>
#include "mialsrtkIntensityStandardizationWrapper.h"


class IntensityStandardizationWrapper;

ML_START_NAMESPACE

//! 
class CHUVINTENSITYSTANDARDIZATION_EXPORT mialIntensityStandardization : public Module
{

	friend class mialIntensityStandardizationBackgroundTask;
public:

  //! Constructor.
  mialIntensityStandardization();

  ~mialIntensityStandardization();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

private:

  // ----------------------------------------------------------
  //! \name Module field declarations
  //@{
  // ----------------------------------------------------------
  void clear();
  //! 
  StringField* _inputFileNamesFld;
  //! 
  StringField* _outputFileNamesFld;
  //! 
  FloatField* _maxIntensityFld;
  //@}
  StringField* _statusFld;
  NotifyField* _startTaskFld;
  NotifyField* _startTaskModalFld;

  BoolField* _inProgressFld;
  BoolField* _outputSucceedFld;

  std::vector<std::string> splitInputs;
  std::vector<std::string> splitOutputs;

protected:
  void postComputation();
  void IntensityStandardizationAllInput();
  mialIntensityStandardizationBackgroundTask * m_pBGmialIntensityStandardizationWorker;
  boost::thread *m_pmialIntensityStandardizationWorkerThread;

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(mialIntensityStandardization)
};


ML_END_NAMESPACE