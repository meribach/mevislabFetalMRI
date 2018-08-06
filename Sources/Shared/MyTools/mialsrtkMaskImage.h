#ifndef mialsrtkMaskImage_H
#define mialsrtkMaskImage_H

class mialsrtkMaskImage
{
public:
	mialsrtkMaskImage(const char* const _inputFile, const char* const _maskFile ,const char* const _outputFile);

	~mialsrtkMaskImage();

	const char* inputFile;
	const char* maskFile;
	const char* outputFile;
	bool runMaskImage();

};

#endif mialsrtkMaskImage_H