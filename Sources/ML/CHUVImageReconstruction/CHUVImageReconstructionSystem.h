//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    
// \author  FetalMRI
// \date    2018-06-11
*/
//----------------------------------------------------------------------------------


#pragma once


// DLL export macro definition.
#ifdef CHUVIMAGERECONSTRUCTION_EXPORTS
  // Use the CHUVIMAGERECONSTRUCTION_EXPORT macro to export classes and functions.
  #define CHUVIMAGERECONSTRUCTION_EXPORT ML_LIBRARY_EXPORT_ATTRIBUTE
#else
  // If included by external modules, exported symbols are declared as import symbols.
  #define CHUVIMAGERECONSTRUCTION_EXPORT ML_LIBRARY_IMPORT_ATTRIBUTE
#endif
