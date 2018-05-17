//----------------------------------------------------------------------------------
//! The ML module class mevisbtkDenoising.
/*!
// \file    
// \author  FetalMRI
// \date    2018-05-09
//
// 
*/
//----------------------------------------------------------------------------------


#pragma once


#include "CHUVToolsSystem.h"

#include <mlModuleIncludes.h>
#include "btkNLMDenoisingWrapper.h"


class btkNLMDenoisingWrapper;

ML_START_NAMESPACE

class BTKDenoiseBackgroundTask; //define in the cpp

//! 
class CHUVTOOLS_EXPORT mevisbtkDenoising : public Module
{

	friend class BTKDenoiseBackgroundTask;

public:

  //! Constructor.
  mevisbtkDenoising();

  virtual ~mevisbtkDenoising();

  //! Handles field changes of the field \p field.
  virtual void handleNotification (Field* field);

private:

  // ----------------------------------------------------------
  //! \name Module field declarations
  //@{
  // ----------------------------------------------------------
  void clear();
  //! 
  StringField* _inputFileNameFld;
  //! 
  StringField* _outputFileNameFld;
  //! 
  StringField* _maskFileNameFld;
  //! filename of the reference image
  StringField* _referenceFileNameFld;
  //! padding value (used if no mask image is provided, default is 0)
  FloatField* _paddingValueFld;
  //! patch half size (default is 1)
  IntField* _patchHalfSizeFld;
  //! half size of the volume search area, i.e. the spatial bandwidth (default is 5)
  IntField* _halfsizeVolumeSearchAreaFld;
  //! beta: smoothing parameter (high beta produces smoother result, default is 1)
  FloatField* _smoothingParameterBetaFld;
  //! 0: pointwise, 1: blockwise, 2: fast blockwise (default is 1)
  EnumField* _blockFld;
  //! optimized mode (use mean and standard deviation of patches) (0: no, 1: yes) (default is 1)
  BoolField* _optFld;
  //! lower mean threshold (0.95 by default) -- for optimized mode only
  FloatField* _lowerMeanThresholdFld;
  //! lower variance threshold (0.5 by default) -- for optimized mode only
  FloatField* _lowerVarianceThresholdFld;
  //! difference_file", "filename of the difference image
  StringField* _differenceFileFld;
  //! Estimation of the smoothing parameter. 0: global, 1: local (default is 0)
  BoolField* _smoothingParameterEstimationFld;
  //! weight of the central patch (possible value: 0, 1, -1 (max)) (default is -1)
  EnumField* _centerFld;


  StringField* _statusFld;
  NotifyField* _startTaskFld;
  BoolField* _inProgressFld;
  BoolField* _ouputSucceedFld;

  std::vector<std::string> splitInputs;
  std::vector<std::string> splitOutputs;
  std::vector<std::string> splitMasks;
  std::vector<std::string> splitRefs;
  std::vector<std::string> splitDiffs;


protected:
	void postComputation();
	void DenoiseAllInput();
	BTKDenoiseBackgroundTask * m_pBGBTKDenoiseWorker;
	boost::thread *m_pBTKDenoiseWorkerThread;

  // Implements interface for the runtime type system of the ML.
  ML_MODULE_CLASS_HEADER(mevisbtkDenoising)
};


ML_END_NAMESPACE