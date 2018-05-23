//----------------------------------------------------------------------------------
//! Dynamic library and runtime type system initialization.
/*!
// \file    
// \author  FetalMRI
// \date    2018-05-22
*/
//----------------------------------------------------------------------------------


#pragma once


ML_START_NAMESPACE

//! Calls init functions of all modules to add their types to the runtime type
//! system of the ML.
int CHUVIntensityStandardizationInit();

ML_END_NAMESPACE
