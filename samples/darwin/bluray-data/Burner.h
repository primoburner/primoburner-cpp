#pragma once

#include "stdafx.h"

#include "BurnerSettings.h"
#include "BurnerException.h"
#include "BurnerCallback.h"

struct DeviceInfo
{
	int32_t Index;
    std::wstring Title;
	bool IsWriter;
};
typedef std::vector<DeviceInfo> DeviceVector;

struct Speed
{
	int32_t TransferRateKB;
	int32_t TransferRate1xKB;
};
typedef std::vector<Speed> SpeedVector;

class Burner : public DataDiscCallback, public DeviceCallback
{
public:
	
	Burner(void);
	virtual ~Burner(void);

public:
	void setCallback(IBurnerCallback* value);

	bool isOpen() const;
	void setOpen(bool isOpen);

	const int32_t maxWriteSpeedKB() const;
	const uint32_t mediaFreeSpace() const;

	const primo::burner::MediaProfile::Enum mediaProfile() const;
	const std::wstring mediaProfileString() const;

	const bool mediaIsBlank() const;
	const bool mediaIsFullyFormatted() const;

	const uint32_t deviceCacheSize() const;
	const uint32_t deviceCacheUsedSize() const;
	const uint32_t writeTransferKB() const;

	void open();
	void close();

	void selectDevice(uint32_t deviceIndex, bool exclusive);
	void releaseDevice();

	const uint64_t calculateImageSize(const std::wstring& sourceFolder, ImageTypeFlags::Enum imageType, UdfRevision::Enum udfRevision);

	const DeviceVector& enumerateDevices();
	const SpeedVector& enumerateWriteSpeeds();

	void closeTray();
	void eject();

	void createImage(const CreateImageSettings& settings);
	void burnImage(const BurnImageSettings& settings);
	
	void burn(const BurnSettings& settings);
	
	void format(const FormatSettings& settings);

	void dismountMedia();

// DeviceCallback13
public:
	void onFormatProgress(double fPercentCompleted);
	void onEraseProgress(double fPercentCompleted);

// DataDiscCallback
public:
	void onProgress(int64_t ddwPos, int64_t ddwAll);
	void onStatus(DataDiscStatus::Enum eStatus);
	void onFileStatus(int nFile, const char_t* tcsFileName, int nPercent);
	bool_t onContinueWrite();

protected:
	bool_t formatMedia(Device * pDevice);
	int getLastCompleteTrack(Device * pDevice);

	void setImageLayoutFromFolder(DataDisc* pDataCD, const wchar_t* sourceFolder);

	void setVolumeProperties(DataDisc* pDataDisc, const std::wstring& volumeLabel);
	std::wstring getDataDiscStatusString(DataDiscStatus::Enum eStatus);
    
    bool isWritePossible(Device *pDevice) const;

private:
	bool m_isOpen;

	DeviceVector m_deviceVector;
	SpeedVector m_speedVector;

	Engine* m_pEngine;
	Device * m_pDevice;

	IBurnerCallback* m_pCallback;
};
