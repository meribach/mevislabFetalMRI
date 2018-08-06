//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    
// \author  FetalMRI
// \date    2018-05-22
*/
//----------------------------------------------------------------------------------

#include "CHUVIntensityStandardizationSystem.h"

// Include definition of ML_INIT_LIBRARY.
#include <mlLibraryInitMacros.h>

// Include all module headers ...
#include "mlmialIntensityStandardization.h"
#include "mlmialCorrectSliceIntensity.h"
#include "mlmialSliceBySliceBiasEstimation.h"
#include "mlmialSliceBySliceBiasFieldCorrection.h"
#include "mlmialN4BiasField.h"


ML_START_NAMESPACE

//----------------------------------------------------------------------------------
//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
//----------------------------------------------------------------------------------
int CHUVIntensityStandardizationInit()
{
  // Add initClass calls from modules here.
  mialIntensityStandardization::initClass();
  mialCorrectSliceIntensity::initClass();
  mialSliceBySliceBiasEstimation::initClass();
  mialSliceBySliceBiasFieldCorrection::initClass();
  mialN4BiasField::initClass();

  return 1;
}

ML_END_NAMESPACE


//! Calls the init method implemented above during load of shared library.
ML_INIT_LIBRARY(CHUVIntensityStandardizationInit)