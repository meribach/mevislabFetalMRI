//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    
// \author  FetalMRI
// \date    2018-06-26
*/
//----------------------------------------------------------------------------------


#pragma once


// DLL export macro definition.
#ifdef CHUVTVSUPERRESOLUTION_EXPORTS
  // Use the CHUVTVSUPERRESOLUTION_EXPORT macro to export classes and functions.
  #define CHUVTVSUPERRESOLUTION_EXPORT ML_LIBRARY_EXPORT_ATTRIBUTE
#else
  // If included by external modules, exported symbols are declared as import symbols.
  #define CHUVTVSUPERRESOLUTION_EXPORT ML_LIBRARY_IMPORT_ATTRIBUTE
#endif
