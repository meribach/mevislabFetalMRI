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


#pragma once


#include "CHUVToolsSystem.h"
//#include <mlProcessAllPagesBackgroundTask.h>
//#include <mlBackgroundTaskBaseModule.h>
#include <mlModuleIncludes.h>
#include "mialsrtkOrientImageWrapper.h"



class reOrientWrapper;

//the background class that will call the mialOrientImage (not background task)
ML_START_NAMESPACE

class OrientImageBackgroundTask; //define in the cpp




//! 
class CHUVTOOLS_EXPORT mialOrientImage : public Module
{

	friend class OrientImageBackgroundTask; //to make OrientImageBackgroundTask having access to the mialOrientImage function and variable
public:

  //! Constructor.
  mialOrientImage();

  //! Destructor
  virtual ~mialOrientImage();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

private:

  // ----------------------------------------------------------
  //! \name Module field declarations
  //@{
  // ----------------------------------------------------------

  void clear();
  
  //! input file name
  StringField* _inputFileNameFld;

  //!Orientation
  StringField* _orientationFdl;

  //! output file name
  StringField* _outputFileNameFld;

  StringField* _statusFld;
  NotifyField* _startTaskFld;
  NotifyField* _startTaskModalFld;

  BoolField* _inProgressFld;
  BoolField* _outputSucceedFld;
  //@}
  std::vector<std::string> splitInputs;
  std::vector<std::string> splitOutputs;
  std::vector<std::string> splitOrientation;

  
  //reOrientWrapper* useReorientWrapper;

protected:
	void postComputation();
	void reOrientAllInput();
	OrientImageBackgroundTask * m_pBGOrientImageWorker;
	boost::thread *m_pOrientImageWorkerThread;

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(mialOrientImage)
};


ML_END_NAMESPACE