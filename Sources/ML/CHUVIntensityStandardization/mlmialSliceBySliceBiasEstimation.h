//----------------------------------------------------------------------------------
//! The ML module class mialSliceBySliceBiasEstimation.
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
#include "mialsrtkSliceBySliceN4BiasFieldCorrectionWrapper.h"

class mialsrtkSliceBySliceN4BiasFieldCorrectionWrapper;

ML_START_NAMESPACE

class mialSliceBySliceBiasEstimationBackgroundTask;

//! 
class CHUVINTENSITYSTANDARDIZATION_EXPORT mialSliceBySliceBiasEstimation : public Module
{

	friend class mialSliceBySliceBiasEstimationBackgroundTask;
public:

  //! Constructor.
  mialSliceBySliceBiasEstimation();

  virtual ~mialSliceBySliceBiasEstimation();

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
  StringField* _outputImageFileFld;
  //! 
  StringField* _outputBiasFieldFileFld;
  //@}
  StringField* _statusFld;
  NotifyField* _startTaskFld;
  NotifyField* _startTaskModalFld;
  BoolField* _inProgressFld;
  BoolField* _outputSucceedFld;

  std::vector<std::string> splitInputs;
  std::vector<std::string> splitOutputs;
  std::vector<std::string> splitMasks;
  std::vector<std::string> splitoutputBiasField;

protected:
  void postComputation();
  void EstimateBiasAllInput();
  mialSliceBySliceBiasEstimationBackgroundTask * m_pBGmialSliceBySliceBiasEstimationWorker;
  boost::thread *m_pmialSliceBySliceBiasEstimationThread;
  

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(mialSliceBySliceBiasEstimation)
};


ML_END_NAMESPACE