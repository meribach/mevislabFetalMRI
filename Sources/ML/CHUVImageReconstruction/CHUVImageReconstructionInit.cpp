//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    
// \author  FetalMRI
// \date    2018-06-11
*/
//----------------------------------------------------------------------------------

#include "CHUVImageReconstructionSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include <mlLibraryInitMacros.h>

// Include all module headers ...
#include "mlmialImageReconstruction.h"


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int CHUVImageReconstructionInit()
{
  // Add initClass calls from modules here.
  mialImageReconstruction::initClass();

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(CHUVImageReconstructionInit)