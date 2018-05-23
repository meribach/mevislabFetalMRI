//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    
// \author  FetalMRI
// \date    2018-05-22
*/
//----------------------------------------------------------------------------------


#pragma once


// DLL export macro definition.
#ifdef CHUVINTENSITYSTANDARDIZATION_EXPORTS
  // Use the CHUVINTENSITYSTANDARDIZATION_EXPORT macro to export classes and functions.
  #define CHUVINTENSITYSTANDARDIZATION_EXPORT ML_LIBRARY_EXPORT_ATTRIBUTE
#else
  // If included by external modules, exported symbols are declared as import symbols.
  #define CHUVINTENSITYSTANDARDIZATION_EXPORT ML_LIBRARY_IMPORT_ATTRIBUTE
#endif
