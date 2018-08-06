#ifndef mialsrtkN4BiasFieldCorrection_H
#define mialsrtkN4BiasFieldCorrection_H

class mialsrtkN4BiasFieldCorrection
{
   public:
	   mialsrtkN4BiasFieldCorrection(const char* const _inputFile, const char* const _maskFile, const char* const _outputFile, const char* const _outputBiasFile);

	   ~mialsrtkN4BiasFieldCorrection();

	   const char* inputFile;
	   const char* maskFile;
	   const char* outputFile;
	   const char* outputBiasFile;

	   unsigned int ShrinkFactor;
	   bool runN4BiasFieldCorrection();
};




#endif mialsrtkN4BiasFieldCorrection_H