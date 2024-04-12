#pragma once

#include "stdafx.h"

#include "BurnerSettings.h"
#include "BurnerException.h"
#include "BurnerCallback.h"

struct DeviceInfo
{
    int32_t Index;
    tstring Title;
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
    void setIsOpen(bool isOpen);

    const int32_t maxWriteSpeedKB() const;
    const uint32_t mediaFreeSpace() const;

    const MediaProfile::Enum mediaProfile() const;
    const tstring mediaProfileString() const;

    const bool mediaIsBlank() const;
    const bool mediaIsFullyFormatted() const;

    const uint32_t deviceCacheSize() const;
    const uint32_t deviceCacheUsedSize() const;
    const uint32_t writeTransferKB() const;

    void open();
    void close();

    void selectDevice(uint32_t deviceIndex, bool exclusive);
    void releaseDevice();

    const uint64_t calculateImageSize(const tstring& sourceFolder, ImageTypeFlags::Enum imageType, UdfRevision::Enum udfRevision);

    const DeviceVector& enumerateDevices();
    const SpeedVector& enumerateWriteSpeeds();

    void closeTray();
    void eject();

    void createImage(const CreateImageSettings& settings);
    void burnImage(const BurnImageSettings& settings);
    
    void burn(const BurnSettings& settings);
    
    void format(const FormatSettings& settings);

    void dismountMedia();

// DeviceCallback
public:
    void onFormatProgress(double progress);
    void onEraseProgress(double progress);

// DataDiscCallback
public:
    void onProgress(int64_t bytesWritten, int64_t all);
    void onStatus(DataDiscStatus::Enum eStatus);
    void onFileStatus(int32_t fileNumber, const char_t* filename, int32_t percentWritten);
    bool_t onContinueWrite();

protected:
    BOOL formatMedia(Device * pDevice);
    int getLastCompleteTrack(Device * pDevice);

    void processInputTree(DataFile * pCurrentFile, tstring& currentPath);
    void setImageLayoutFromFolder(DataDisc* pDataCD, LPCTSTR sourceFolder);

    void setVolumeProperties(DataDisc* pDataDisc, const tstring& volumeLabel, primo::burner::ImageTypeFlags::Enum imageType);
    tstring getDataDiscStatusString(DataDiscStatus::Enum eStatus);

    bool isWritePossible(Device *device) const;

private:
    bool m_isOpen;

    DeviceVector m_deviceVector;
    SpeedVector m_speedVector;

    Engine* m_pEngine;
    Device* m_pDevice;

    IBurnerCallback* m_pCallback;
};
