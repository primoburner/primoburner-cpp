#pragma once

class IBurnerCallback
{
public:
	virtual void onStatus(const tstring& message) = 0;

	virtual void onImageProgress(uint64_t ddwPos, uint64_t ddwAll) = 0; 
	virtual void onFileProgress(int file, const tstring& fileName, int percentCompleted) = 0;

	virtual void onFormatProgress(double percentCompleted) = 0;

	virtual bool onContinue() = 0;
};
