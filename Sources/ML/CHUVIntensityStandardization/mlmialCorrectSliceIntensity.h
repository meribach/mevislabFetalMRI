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


#pragma once


#include "CHUVIntensityStandardizationSystem.h"

#include <mlModuleIncludes.h>
#include "mialsrtkCorrectSliceIntensityWrapper.h"

class CorrectSliceIntensityWrapper;

ML_START_NAMESPACE

class mialCorrectSliceIntensityBackgroundTask;

//! 
class CHUVINTENSITYSTANDARDIZATION_EXPORT mialCorrectSliceIntensity : public Module
{
	friend class mialCorrectSliceIntensityBackgroundTask;

public:
  
  //! Constructor.
  mialCorrectSliceIntensity();

  virtual ~mialCorrectSliceIntensity();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

private:

  // ----------------------------------------------------------
  //! \name Module field declarations
  //@{
  // ----------------------------------------------------------
  void clear();
  
  //! path of input file
  StringField* _inputFileNameFld;
  //! path of output file
  StringField* _outputFileNameFld;
  //! path of mask file (input)
  StringField* _maskFileNameFld;
  //@}
  StringField* _statusFld;
  NotifyField* _startTaskFld;
  NotifyField* _startTaskModalFld;

  BoolField* _inProgressFld;
  BoolField* _outputSucceedFld;

  std::vector<std::string> splitInputs;
  std::vector<std::string> splitOutputs;
  std::vector<std::string> splitMasks;


protected:
  void postComputation();
  void CorrectSliceIntensityAllInput();
  mialCorrectSliceIntensityBackgroundTask * m_pBGmialCorrectSliceIntensityWorker;
  boost::thread *m_pmialCorrectSliceIntensityWorkerThread;
  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(mialCorrectSliceIntensity)
};


ML_END_NAMESPACE