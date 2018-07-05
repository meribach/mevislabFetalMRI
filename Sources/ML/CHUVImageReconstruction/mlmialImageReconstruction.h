//----------------------------------------------------------------------------------
//! The ML module class mialImageReconstruction.
/*!
// \file    
// \author  FetalMRI
// \date    2018-06-11
//
// 
*/
//----------------------------------------------------------------------------------


#pragma once


#include "CHUVImageReconstructionSystem.h"

#include <mlModuleIncludes.h>
#include "mialsrtkImageReconstructionWrapper.h"

class mialImageReconstructionWrapper;
   

ML_START_NAMESPACE


class mialImageReconstructionBackgroundTask; 
//! 
class CHUVIMAGERECONSTRUCTION_EXPORT mialImageReconstruction : public Module
{

	friend class mialImageReconstructionBackgroundTask;

public:

  //! Constructor.
  mialImageReconstruction();

  virtual ~mialImageReconstruction();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

private:

  // ----------------------------------------------------------
  //! \name Module field declarations
  //@{
  // ----------------------------------------------------------
  void clear();
  //! input file name, multiple files: use -- as delimeter, ex: C:/file1.nii--C:/file2.nii
  StringField* _inputFilesFld;
  //! input mask file name, multiple files: use -- as delimeter, ex: C:/filemask1.nii--C:/filemask2.nii
  StringField* _maskFilesFld;
  //! output file name
  StringField* _outputFileFld;
  //! Reference Image
  StringField* _refFileFld;
  //! transform output files, multiple files: use -- as delimeter, ex: C:/file1transform.nii--C:/file2transform.nii"
  StringField* _transformoutFilesFld;
  //! Resampled image file name with initial transform (this is an output to check initial transform consistency), multiple files: use -- as delimeter, ex: C:/fileresampled1.nii--C:/fileresampled2.nii
  StringField* _resampledFilesFld;
  //! All image masks combined in a single one, output filename
  StringField* _combinedMasksFld;
  //! Maximum number of iterations
  IntField* _itMaxFld;
  //! Minimal percent change between two iterations considered as convergence. (default 0.0001)
  DoubleField* _espilonFld;
  //! 
  DoubleField* _marginFld;
  //! Use of 3D rigid transforms, Recommended for adult subjects.
  BoolField* _rigid3DFld;
  //! No registration is performed, the image is reconstructed with the identity transform. This option is important to have a reference for performance assessment.
  BoolField* _noregFld;
  //! Flag to set deblurring kernel for SDI (double the neighborhood) (by default it is disable.).
  BoolField* _debluringFld;
  //! box: Use intersections for roi calculation ; mask:Use masks for roi calculation ; all:Use the whole image FOV, default is mask
  EnumField* _methodRoiCalculationFld;
  //! targetImage of the lowToHighResFilter (should be the one with the less movement), default is 0
  IntField* _ImageBaseOfReconFld;
  //! Metric of the Coregistration, NC = Normalized Correlation, MI = Mutual Information, default is NC
  EnumField* _MetricToUseFld;
  //! 
  IntField* _nbIterationsRegistrationFld;
  //! 
  DoubleField* _GradientMagnitudeToleranceRegistrationFld;
  //! 
  DoubleField* _MinStepLengthRegistrationFld;
  //! 
  DoubleField* _MaxStepLengthRegistrationFld;
  //! 
  DoubleField* _RelaxationFactorRegistrationFld;
  //@}

  FloatField* _resampleKernelMultiplicatorFld;

  StringField* _statusFld;
  NotifyField* _startTaskFld;
  NotifyField* _startTaskModalFld;
  BoolField* _inProgressFld;
  BoolField* _outputSucceedFld;

  std::vector< std::string > splitinputFile;
  std::vector< std::string > splitmaskFile;
  std::vector< std::string > splittransformout;
  std::vector< std::string > splitresampled;

  bool maskBool;
  bool boxBool;
  bool allBool;

protected:
	void postComputation();
	void ReconstructHRImage();
	mialImageReconstructionBackgroundTask * m_pBGImageReconstructionWorker;
	boost::thread *m_pBGImageReconstructionWorkerThread;

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(mialImageReconstruction)
};


ML_END_NAMESPACE