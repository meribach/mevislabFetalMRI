//----------------------------------------------------------------------------------
//! Project global and OS specific declarations.
/*!
// \file    
// \author  FetalMRI
// \date    2018-07-17
*/
//----------------------------------------------------------------------------------


#pragma once


// DLL export macro definition.
#ifdef CHUVBRAINLOCALIZATIONEXTRACTION_EXPORTS
  // Use the CHUVBRAINLOCALIZATIONEXTRACTION_EXPORT macro to export classes and functions.
  #define CHUVBRAINLOCALIZATIONEXTRACTION_EXPORT ML_LIBRARY_EXPORT_ATTRIBUTE
#else
  // If included by external modules, exported symbols are declared as import symbols.
  #define CHUVBRAINLOCALIZATIONEXTRACTION_EXPORT ML_LIBRARY_IMPORT_ATTRIBUTE
#endif
