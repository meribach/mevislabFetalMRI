//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    
// \author  FetalMRI
// \date    2018-08-06
*/
//----------------------------------------------------------------------------------


#pragma once


// DLL export macro definition.
#ifdef CHUVTOOLS_EXPORTS
  // Use the CHUVTOOLS_EXPORT macro to export classes and functions.
  #define CHUVTOOLS_EXPORT ML_LIBRARY_EXPORT_ATTRIBUTE
#else
  // If included by external modules, exported symbols are declared as import symbols.
  #define CHUVTOOLS_EXPORT ML_LIBRARY_IMPORT_ATTRIBUTE
#endif
