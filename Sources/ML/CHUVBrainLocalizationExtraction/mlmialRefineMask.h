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


#pragma once


#include "CHUVBrainLocalizationExtractionSystem.h"

#include <mlModuleIncludes.h>
#include "mialsrtkRefineHRMaskByIntersectionWrapper.h"

class mialsrtkRefineHRMaskByIntersectionWrapper;

ML_START_NAMESPACE


class mialRefineMaskBackgroundTask;

//! 
class CHUVBRAINLOCALIZATIONEXTRACTION_EXPORT mialRefineMask : public Module
{

	friend class mialRefineMaskBackgroundTask;
public:

  //! Constructor.
  mialRefineMask();

  ~mialRefineMask();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

private:

  // ----------------------------------------------------------
  //! \name Module field declarations
  //@{
  // ----------------------------------------------------------
  void clear();
  //! 
  StringField* _inputFilesFld;
  //! 
  StringField* _transformInFld;
  //! 
  StringField* _maskFilesFld;
  //! 
  StringField* _outputHRFileFld;
  //! 
  StringField* _outputLRFilesFld;
  //! 
  StringField* _referenceFileFld;
  //! 
  BoolField* _radiusDilationFld;
  //! 
  BoolField* _useStapleFld;
  //@}

  StringField* _statusFld;
  NotifyField* _startTaskFld;
  NotifyField* _startTaskModalFld;
  BoolField* _inProgressFld;
  BoolField* _outputSucceedFld;

  std::vector< std::string > splitinputFile;
  std::vector< std::string > splitmaskFile;
  std::vector< std::string > splittransformin;
  std::vector< std::string > splitoutputLRFile;


protected:
	void postComputation();
	void runRefineMask();
	mialRefineMaskBackgroundTask * m_pBGRefineMaskWorker;
	boost::thread *m_pBGRefineMaskWorkerThread;
 
  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(mialRefineMask)
};


ML_END_NAMESPACE