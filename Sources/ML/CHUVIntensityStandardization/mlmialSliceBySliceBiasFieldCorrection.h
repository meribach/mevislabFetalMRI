//----------------------------------------------------------------------------------
//! The ML module class mialSliceBySliceBiasFieldCorrection.
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
#include "mialsrtkSliceBySliceCorrectBiasFieldWrapper.h"

class mialsrtkSliceBySliceCorrectBiasFieldWrapper;

ML_START_NAMESPACE

//! 
class CHUVINTENSITYSTANDARDIZATION_EXPORT mialSliceBySliceBiasFieldCorrection : public Module
{
	friend class mialSliceBySliceBiasFieldCorrectionBackgroundTask;
public:

  //! Constructor.
  mialSliceBySliceBiasFieldCorrection();

  virtual ~mialSliceBySliceBiasFieldCorrection();

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
  StringField* _inputMaskFileFld;
  //! 
  StringField* _inputBiasFieldFileFld;
  //! 
  StringField* _outputImageFileFld;
  //@}
  StringField* _statusFld;
  NotifyField* _startTaskFld;
  NotifyField* _startTaskModalFld;
  BoolField* _inProgressFld;
  BoolField* _outputSucceedFld;

  std::vector<std::string> splitInputs;
  std::vector<std::string> splitOutputs;
  std::vector<std::string> splitMasks;
  std::vector<std::string> splitinputBiasField;


protected:
	void postComputation();
	void CorrectBiasAllInput();
	mialSliceBySliceBiasFieldCorrectionBackgroundTask * m_pBGmialSliceBySliceBiasFieldCorrectionWorker;
	boost::thread *m_pmialSliceBySliceBiasFieldCorrectionThread;

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(mialSliceBySliceBiasFieldCorrection)
};


ML_END_NAMESPACE