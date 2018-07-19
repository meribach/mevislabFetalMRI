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

ML_START_NAMESPACE


//! 
class CHUVBRAINLOCALIZATIONEXTRACTION_EXPORT mialRefineMask : public Module
{
public:

  //! Constructor.
  mialRefineMask();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

private:

  // ----------------------------------------------------------
  //! \name Module field declarations
  //@{
  // ----------------------------------------------------------

  //! 
  StringField* _maskFilesFld;
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

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(mialRefineMask)
};


ML_END_NAMESPACE