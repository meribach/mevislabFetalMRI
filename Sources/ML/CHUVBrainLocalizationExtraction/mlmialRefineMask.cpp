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

#include "mlmialRefineMask.h"


ML_START_NAMESPACE

//! Implements code for the runtime type system of the ML
ML_MODULE_CLASS_SOURCE(mialRefineMask, Module);

//----------------------------------------------------------------------------------

mialRefineMask::mialRefineMask() : Module(0, 0)
{
  // Suppress calls of handleNotification on field changes to
  // avoid side effects during initialization phase.
  handleNotificationOff();

  // Add fields to the module and set their values.
  _maskFilesFld = addString("maskFiles", "");
  _transformInFld = addString("transformIn", "");
  _maskFilesFld = addString("maskFiles", "");
  _outputHRFileFld = addString("outputHRFile", "");
  _outputLRFilesFld = addString("outputLRFiles", "");
  _referenceFileFld = addString("referenceFile", "");
  _radiusDilationFld = addBool("radiusDilation", true);
  _useStapleFld = addBool("useStaple", false);

  // Reactivate calls of handleNotification on field changes.
  handleNotificationOn();


}

//----------------------------------------------------------------------------------

void mialRefineMask::handleNotification(Field* field)
{
  // Handle changes of module parameters and input image fields here.
}

ML_END_NAMESPACE