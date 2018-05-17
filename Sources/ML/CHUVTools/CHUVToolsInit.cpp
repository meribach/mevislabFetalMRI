//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    
// \author  FetalMRI
// \date    2018-04-12
*/
//----------------------------------------------------------------------------------

#include "CHUVToolsSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include <mlLibraryInitMacros.h>

// Include all module headers ...
#include "mlmialOrientImage.h"
#include "mlmevisbtkDenoising.h"



ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int CHUVToolsInit()
{
  // Add initClass calls from modules here.
  mialOrientImage::initClass();
  mevisbtkDenoising::initClass();

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(CHUVToolsInit)