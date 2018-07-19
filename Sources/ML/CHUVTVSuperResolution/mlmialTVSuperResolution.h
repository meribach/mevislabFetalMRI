//----------------------------------------------------------------------------------
//! The ML module class mialTVSuperResolution.
/*!
// \file    
// \author  FetalMRI
// \date    2018-06-26
//
// 
*/
//----------------------------------------------------------------------------------


#pragma once


#include "CHUVTVSuperResolutionSystem.h"

#include <mlModuleIncludes.h>
#include "mialsrtkTVSuperResolutionWrapper.h"

class mialsrtkTVSuperResolutionWrapper;

ML_START_NAMESPACE


class mialSuperResolutionBackgroundTask;
//! 
class CHUVTVSUPERRESOLUTION_EXPORT mialTVSuperResolution : public Module
{

	friend class mialSuperResolutionBackgroundTask;
public:

  //! Constructor.
  mialTVSuperResolution();

  ~mialTVSuperResolution();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

private:

  // ----------------------------------------------------------
  //! \name Module field declarations
  //@{
  // ----------------------------------------------------------
  void clear();
  //! Super resolution output image
  StringField* _outputFileFld;
  //! low-resolution image mask file, use -- as delimeter, ex: C:/file1mask.nii--C:/file2mask.nii
  StringField* _maskFilesFld;
  //! Reconstructed image for initialization. Typically the output of btkImageReconstruction is used.
  StringField* _refFileFld;
  //! Low-resolution image file, use -- as delimeter, ex: C:/file1.nii--C:/file2.nii
  StringField* _inputFilesFld;
  //! in-transform file, use -- as delimeter, ex: C:/file1transform.txt--C:/file2transform.txt
  StringField* _transformFilesInputFld;
  //! Number of inner iterations (default = 50)
  IntField* _nbIterFld;
  //! Regularization factor (default = 0.1)
  FloatField* _LambdaFld;
  //! Parameter deltat (default = 1.0)
  FloatField* _DeltaTFld;
  //! Parameter gamma (default = 1.0)
  FloatField* _GammaFld;
  //! Parameter step scale (default = 1.0)
  FloatField* _stepScaleFld;
  //! Inner loop convergence threshold (default = 1e-4)
  DoubleField* _innerConvThresholdFld;
  //! Outer loop convergence threshold (default = 1e-4)
  DoubleField* _outerConvThresholdFld;
  //! Number of Bregman loops (default = 10)
  IntField* _numberOfBregmanLoopsFld;
  //! Number of loops (SR/denoising) (default = 5)
  IntField* _numberOfLoopsFld;
  //! A boxcar-shaped PSF is assumed as imaging model (by default a Gaussian-shaped PSF is employed.)
  BoolField* _boxcarFld;
  //! Flag to enable the update of motion parameters during SR (by default it is disable.).
  BoolField* _updateMotionFld;
  //! Mask of the reconstructed image for initialization - Only used when motion estimation is updated during SR
  StringField* _refMaskFld;
  //! Low-resolution pre-processed image file - Only used when motion estimation is updated during SR
  StringField* _pre_inputFld;
  //! output transform file - Only used when motion estimation is updated during SR
  StringField* _transformFilesOutputFld;
  //! Directory where  SR reconstructed image at each outer loop of the reconstruction optimization is saved
  StringField* _debugDirFld;
  //! Flag to set deblurring PSF during SR (double the neighborhood)  (by default it is disable.).
  BoolField* _debluringFld;

  FloatField* _kernelRadiusMultiplicatorFld;
  //! Metric of the Coregistration, NC = Normalized Correlation, MI = Mutual Information, default is NC
  EnumField* _CoregistrationMethodFld;
  //! 
  IntField* _NbIterationRegistrationFld;
  //! 
  DoubleField* _GradientMagnitudeToleranceRegistrationFld;
  //! 
  DoubleField* _MinStepLengthRegistrationFld;
  //! 
  DoubleField* _RelaxationFactorRegistrationFld;
  //! 
  DoubleField* _MaxStepLengthRegistrationFld;


  StringField* _statusFld;
  NotifyField* _startTaskFld;
  NotifyField* _startTaskModalFld;
  BoolField* _inProgressFld;
  BoolField* _outputSucceedFld;

  std::vector< std::string > splitinputFile;
  std::vector< std::string > splitmaskFile;
  std::vector< std::string > splittransformin;
  std::vector< std::string > splittransformout;
  std::vector< std::string > splitpreinput;

  //@}
protected:
	void postComputation();
	void runTVSuperResolution();
	mialSuperResolutionBackgroundTask * m_pBGSuperResolutionWorker;
	boost::thread *m_pBGSuperResolutionWorkerThread;
  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(mialTVSuperResolution)
};


ML_END_NAMESPACE