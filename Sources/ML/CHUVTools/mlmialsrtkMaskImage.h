//----------------------------------------------------------------------------------
//! The ML module class mialsrtkMaskImage.
/*!
// \file    
// \author  FetalMRI
// \date    2018-08-06
//
// 
*/
//----------------------------------------------------------------------------------


#pragma once


#include "CHUVToolsSystem.h"

#include <mlModuleIncludes.h>

class mialsrtkMaskImageWrapper;

ML_START_NAMESPACE

class mialsrtkMaskImageBackgroundTask;

//! 
class CHUVTOOLS_EXPORT mialsrtkMaskImage : public Module
{
	friend class mialsrtkMaskImageBackgroundTask;
public:

  //! Constructor.
  mialsrtkMaskImage();

  ~mialsrtkMaskImage();

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
  StringField* _statusFld;
  NotifyField* _startTaskFld;
  NotifyField* _startTaskModalFld;

  BoolField* _inProgressFld;
  BoolField* _outputSucceedFld;
  //@}
protected:
	void postComputation();
	void runmaskImage();
	mialsrtkMaskImageBackgroundTask * m_pBGMaskImageWorker;
	boost::thread *m_pBGMaskImageWorkerThread;
  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(mialsrtkMaskImage)
};


ML_END_NAMESPACE