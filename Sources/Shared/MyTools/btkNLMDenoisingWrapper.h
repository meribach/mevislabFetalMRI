/*==========================================================================

© Université de Strasbourg - Centre National de la Recherche Scientifique

Date: 16/08/2011
Author(s): Marc Schweitzer (marc.schweitzer(at)unistra.fr)

This software is governed by the CeCILL-B license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL-B
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL-B license and that you accept its terms.

==========================================================================*/

#ifndef btkNLMDenoisingWrapper_H
#define btkNLMDenoisingWrapper_H

#include <string>

#if defined(_MSC_VER)
    //  Microsoft 
    #define btkNLMDenoisingWrapper_EXPORT __declspec(dllexport)

#elif defined(__GNUC__)
    //  GCC
    #define btkNLMDenoisingWrapper_EXPORT __attribute__((visibility("default")))
#endif


class btkNLMDenoise;

class btkNLMDenoisingWrapper_EXPORT btkNLMDenoisingWrapper {

public:

	//! default constructor
	btkNLMDenoisingWrapper(const char* const _inputFile, const char* const _outputFile, const char* const _maskFile, const char* const _refFile,
		float _padding, int _hwn, int _hwvs, float _beta, int _block, int _center, int _optimized, float _lowerMeanThreshold,
		float _lowerVarianceThreshold, const char* const _differenceFile, int _localSmoothing);

    //second constructor with only inputfile outputfile and maskfile as input. other parameters will be default values
	btkNLMDenoisingWrapper(const char* const _inputFile, const char* const _outputFile, const char* const _maskFile);

	btkNLMDenoisingWrapper(const char* const _inputFile, const char* const _outputFile);

	//! default destructor
	~btkNLMDenoisingWrapper();

	const char* inputFile;
	const char* outputFile;
	const char* maskFile;
	const char* refFile;
	float padding;
	int hwn;
	int hwvs;
	float beta;
	int block;
	int center;
	int optimized;
	float lowerMeanThreshold;
	float lowerVarianceThreshold;
	const char* differenceFile;
	int localSmoothing;
	
	bool runDenoise();
	bool boolExit;

private:
	btkNLMDenoise* wrap_btkNLMDenoise;

};


#endif btkNLMDenoisingWrapper_H
