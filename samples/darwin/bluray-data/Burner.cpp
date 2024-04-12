#include "stdafx.h"
#include "Burner.h"

Burner::Burner(void)
{
    m_pEngine = NULL;
    m_pDevice = NULL;

    setOpen(false);
    setCallback(NULL);
}

Burner::~Burner(void)
{
}

void Burner::setCallback(IBurnerCallback* value)
{
    m_pCallback = value;
}

void Burner::open()
{
    if (isOpen())
        return;

    // Enable trace log
    Library::enableTraceLog(NULL, true);

    // Put your own license xml here
    Library::setLicense("License XML");

    m_pEngine = Library::createEngine();

    if (!m_pEngine->initialize())
    {
        m_pEngine->release();
        m_pEngine = NULL;

        throw BurnerException::CreateBurnerException(ENGINE_INITIALIZATION);
    }

    setOpen(true);
}

void Burner::close()
{
    if (NULL != m_pDevice)
        m_pDevice->release();

    m_pDevice = NULL;

    if (m_pEngine)
    {
        m_pEngine->shutdown();
        m_pEngine->release();
    }

    m_pEngine = NULL;

    Library::disableTraceLog();
}

const uint64_t Burner::calculateImageSize(const std::wstring& sourceFolder, ImageTypeFlags::Enum imageType, UdfRevision::Enum udfRevision)
{
    DataDisc* pDataCD = Library::createDataDisc();
    pDataCD->setImageType(imageType);
    pDataCD->udfVolumeProps()->setRevision(udfRevision);

    uint64_t imageSize = 0;
    try
    {
        setImageLayoutFromFolder(pDataCD, sourceFolder.c_str());
        imageSize = pDataCD->imageSizeInBytes();
    }
    catch (...)
    {
        pDataCD->release();
        throw;
    }

    pDataCD->release();
    return imageSize;
}

const int32_t Burner::maxWriteSpeedKB() const
{
    if (NULL == m_pDevice)
        throw BurnerException::CreateBurnerException(NO_DEVICE);

    return m_pDevice->maxWriteSpeedKB();
}

const MediaProfile::Enum Burner::mediaProfile() const
{
    if (NULL == m_pDevice)
        throw BurnerException::CreateBurnerException(NO_DEVICE);

    return m_pDevice->mediaProfile();
}

const std::wstring Burner::mediaProfileString() const
{
    uint16_t wProfile = mediaProfile();
    switch (wProfile)
    {
        case MediaProfile::CDRom:
            return _TEXT("CD-ROM. Read only CD.");

        case MediaProfile::CDR:
            return _TEXT("CD-R. Write once CD.");

        case MediaProfile::CDRW:
            return _TEXT("CD-RW. Re-writable CD.");

        case MediaProfile::DVDRom:
            return _TEXT("DVD-ROM. Read only DVD.");

        case MediaProfile::DVDMinusRSeq:
            return _TEXT("DVD-R for Sequential Recording.");

        case MediaProfile::DVDMinusRDLSeq:
            return _TEXT("DVD-R DL Double Layer for Sequential Recording.");

        case MediaProfile::DVDMinusRDLJump:
            return _TEXT("DVD-R DL Double Layer for Layer Jump Recording.");

        case MediaProfile::DVDRam:
            return _TEXT("DVD-RAM ReWritable DVD.");

        case MediaProfile::DVDMinusRWRO:
            return _TEXT("DVD-RW for Restricted Overwrite.");

        case MediaProfile::DVDMinusRWSeq:
            return _TEXT("DVD-RW for Sequential Recording.");

        case MediaProfile::DVDPlusRW:
        {
            if (NULL == m_pDevice)
                throw BurnerException::CreateBurnerException(NO_DEVICE);

            BgFormatStatus::Enum fmt = m_pDevice->bgFormatStatus();
            switch (fmt)
            {
                case BgFormatStatus::NotFormatted:
                    return _TEXT("DVD+RW. Not formatted.");
                    break;
                case BgFormatStatus::Partial:
                    return _TEXT("DVD+RW. Partially formatted.");
                    break;
                case BgFormatStatus::Pending:
                    return _TEXT("DVD+RW. Background format is pending ...");
                    break;
                case BgFormatStatus::Completed:
                    return _TEXT("DVD+RW. Formatted.");
                    break;
            }

            return _TEXT("DVD+RW for Random Recording.");
        }

        case MediaProfile::DVDPlusR:
            return _TEXT("DVD+R for Sequential Recording.");

        case MediaProfile::DVDPlusRDL:
            return _TEXT("DVD+R DL Double Layer for Sequential Recording.");

        case MediaProfile::BDRom:
            return _TEXT("BD-ROM Read only Blu-ray Disc.");

        case MediaProfile::BDRSrm:
            return _TEXT("BD-R for Sequential Recording (SRM-POW).");

        case MediaProfile::BDRSrmPow:
            return _TEXT("BD-R for Sequential Recording with Pseudo-Overwrite (SRM+POW).");

        case MediaProfile::BDRRrm:
            return _TEXT("BD-R Random Recording Mode (RRM).");

        case MediaProfile::BDRE:
        {
            if (mediaIsFullyFormatted())
                return _TEXT("BD-RE ReWritable Blu-ray Disc. Formatted.");

            return _TEXT("BD-RE ReWritable Blu-ray Disc. Blank. Not formatted.");
        }
        default:
            return _TEXT("Unknown Profile.");
    }
}

const bool Burner::mediaIsBlank() const
{
    if (NULL == m_pDevice)
        throw BurnerException::CreateBurnerException(NO_DEVICE);

    return 1 == m_pDevice->isMediaBlank();
}

const bool Burner::mediaIsFullyFormatted() const
{
    if (NULL == m_pDevice)
        throw BurnerException::CreateBurnerException(NO_DEVICE);

    // Get media profile
    MediaProfile::Enum mp = m_pDevice->mediaProfile();

    // DVD+RW
    if (MediaProfile::DVDPlusRW == mp)
        return (BgFormatStatus::Completed == m_pDevice->bgFormatStatus());

    // DVD-RW for Restricted Overwrite
    if (MediaProfile::DVDMinusRWRO == mp)
        return m_pDevice->mediaCapacity() == m_pDevice->mediaFreeSpace();

    // BD-RE
    if (MediaProfile::BDRE == mp || MediaProfile::BDRSrmPow == mp || MediaProfile::BDRRrm == mp)
        return 1 == m_pDevice->isMediaFormatted();

    return false;
}

const uint32_t Burner::deviceCacheSize() const
{
    if (NULL == m_pDevice)
        throw BurnerException::CreateBurnerException(NO_DEVICE);

    return m_pDevice->internalCacheCapacity();
}

const uint32_t Burner::deviceCacheUsedSize() const
{
    if (NULL == m_pDevice)
        throw BurnerException::CreateBurnerException(NO_DEVICE);

    return m_pDevice->internalCacheUsedSpace();
}

const uint32_t Burner::writeTransferKB() const
{
    if (NULL == m_pDevice)
        throw BurnerException::CreateBurnerException(NO_DEVICE);

    return m_pDevice->writeTransferRate();
}

const uint32_t Burner::mediaFreeSpace() const
{
    if (NULL == m_pDevice)
        throw BurnerException::CreateBurnerException(NO_DEVICE);

    return m_pDevice->mediaFreeSpace();
}

const SpeedVector& Burner::enumerateWriteSpeeds()
{
    if (NULL == m_pDevice)
        throw BurnerException::CreateBurnerException(NO_DEVICE);

    m_speedVector.clear();

    SpeedEnum* pSpeeds = m_pDevice->createWriteSpeedEnumerator();

    for (int32_t i = 0; i < pSpeeds->count(); i++)
    {
        SpeedDescriptor* pSpeed = pSpeeds->at(i);
        if (NULL != pSpeed)
        {
            Speed speed;
            speed.TransferRateKB = pSpeed->transferRateKB();

            if (m_pDevice->isMediaBD())
                speed.TransferRate1xKB = (int32_t) Speed1xKB::BD;
            else if (m_pDevice->isMediaDVD())
                speed.TransferRate1xKB = (int32_t) Speed1xKB::DVD;
            else
                speed.TransferRate1xKB = (int32_t) Speed1xKB::CD;

            m_speedVector.push_back(speed);
        }
    }

    pSpeeds->release();
    return m_speedVector;
}

std::wstring get_DeviceTitle(Device* pDevice)
{
    const char_t *desc = pDevice->description();

    char letter = pDevice->driveLetter();

    std::wstringstream name;
    name << L"(" << letter << L":) - " << primo::ustring(desc);

    return name.str();
}

const DeviceVector& Burner::enumerateDevices()
{
    if (!isOpen())
        throw BurnerException::CreateBurnerException(BURNER_NOT_OPEN);

    m_deviceVector.clear();

    DeviceEnum* pEnumerator = m_pEngine->createDeviceEnumerator();

    int32_t nDevices = pEnumerator->count();
    if (0 == nDevices)
    {
        pEnumerator->release();
        throw BurnerException::CreateBurnerException(NO_DEVICES);
    }

    for (int32_t i = 0; i < nDevices; i++)
    {
        Device* pDevice = pEnumerator->createDevice(i);
        if (NULL != pDevice)
        {
            DeviceInfo dev;
            dev.Index = i;
            dev.Title = get_DeviceTitle(pDevice);
            dev.IsWriter = isWritePossible(pDevice);

            m_deviceVector.push_back(dev);
        }
        pDevice->release();
    }

    pEnumerator->release();
    return m_deviceVector;
}

void Burner::selectDevice(uint32_t deviceIndex, bool exclusive)
{
    if (NULL != m_pDevice)
        throw BurnerException::CreateBurnerException(DEVICE_ALREADY_SELECTED);

    DeviceEnum* pEnumerator = m_pEngine->createDeviceEnumerator();
    Device* pDevice = pEnumerator->createDevice(deviceIndex, exclusive ? 1 : 0);
    if (NULL == pDevice)
    {
        pEnumerator->release();
        throw BurnerException::CreateBurnerException(INVALID_DEVICE_INDEX);
    }

    m_pDevice = pDevice;
    pEnumerator->release();
}

void Burner::releaseDevice()
{
    if (NULL != m_pDevice)
        m_pDevice->release();

    m_pDevice = NULL;
}

bool Burner::isOpen() const
{
    return m_isOpen;
}

void Burner::setOpen(bool isOpen)
{
    m_isOpen = isOpen;
}

// DataDiscCallback

void Burner::onProgress(int64_t ddwPos, int64_t ddwAll)
{
    if (m_pCallback)
        m_pCallback->onImageProgress(ddwPos, ddwAll);
}

void Burner::onStatus(DataDiscStatus::Enum eStatus)
{
    if (m_pCallback)
    {
        std::wstring status = getDataDiscStatusString(eStatus);
        m_pCallback->onStatus(status);
    }
}

void Burner::onFileStatus(int fileIndex, const char_t* fileName, int percentComplete)
{
    if (m_pCallback)
    {
        m_pCallback->onFileProgress(fileIndex, primo::ustring(fileName).wstr(), percentComplete);
    }
}

bool_t Burner::onContinueWrite()
{
    if (m_pCallback)
        return m_pCallback->onContinue() ? 1 : 0;

    return 1;
}

// DeviceCallback

void Burner::onFormatProgress(double fPercentCompleted)
{
    if (m_pCallback)
        m_pCallback->onFormatProgress(fPercentCompleted);
}

void Burner::onEraseProgress(double fPercentCompleted)
{
    // erase is not used for Bluray Disc
}

void Burner::setImageLayoutFromFolder(DataDisc* pDataDisc, const wchar_t* sourceFolder)
{
    // Create directory structure
    if (!pDataDisc->setImageLayoutFromFolder(primo::ustring(sourceFolder)))
        throw BurnerException::CreateDataDiscException(pDataDisc, NULL);
}

void Burner::setVolumeProperties(DataDisc* pDataDisc, const std::wstring& volumeLabel)
{
    // Volume label
    pDataDisc->udfVolumeProps()->setVolumeLabel(primo::ustring(volumeLabel));

    // set volume times
    filetime_t timeUtc = time(NULL);
    pDataDisc->udfVolumeProps()->setVolumeCreationTime(timeUtc);
}

void Burner::closeTray()
{
    if (NULL == m_pDevice)
        throw BurnerException::CreateBurnerException(NO_DEVICE);

    m_pDevice->eject(0);
}

void Burner::eject()
{
    if (NULL == m_pDevice)
        throw BurnerException::CreateBurnerException(NO_DEVICE);

    m_pDevice->eject(1);
}

void Burner::createImage(const CreateImageSettings& settings)
{
    DataDisc* pDataCD = Library::createDataDisc();

    try
    {
        setVolumeProperties(pDataCD, settings.VolumeLabel);
        pDataCD->setImageType(settings.ImageType);
        pDataCD->udfVolumeProps()->setRevision(settings.UdfRevision);

        if (settings.BDVideo)
            pDataCD->udfVolumeProps()->setTextEncoding(UdfTextEncoding::Ansi);

        pDataCD->setCallback(this);

        setImageLayoutFromFolder(pDataCD, settings.SourceFolder.c_str());

        // Create the image file
        if (!pDataCD->writeToImageFile(primo::ustring(settings.ImageFile)))
        {
            throw BurnerException::CreateDataDiscException(pDataCD, NULL);
        }

        pDataCD->release();
    }
    catch (...)
    {
        pDataCD->release();
        throw;
    }
}

void Burner::burnImage(const BurnImageSettings& settings)
{
    if (NULL == m_pDevice)
        throw BurnerException::CreateBurnerException(NO_DEVICE);

    DataDisc * pDataCD = Library::createDataDisc();

    try
    {
        m_pDevice->setWriteSpeedKB(settings.WriteSpeedKB);
        formatMedia(m_pDevice);

        pDataCD->setCallback(this);
        pDataCD->setDevice(m_pDevice);
        pDataCD->setSimulateBurn(0);
        pDataCD->setWriteMethod(WriteMethod::BluRay);
        pDataCD->setCloseDisc(settings.CloseDisc);

        // Write the image to the DVD
        if (!pDataCD->writeImageToDisc(primo::ustring(settings.ImageFile)))
        {
            throw BurnerException::CreateDataDiscException(pDataCD, m_pDevice);
        }

        if (settings.Eject)
            m_pDevice->eject(1);

        pDataCD->release();
    }
    catch (...)
    {
        pDataCD->release();
        throw;
    }
}

void Burner::burn(const BurnSettings& settings)
{
    if (NULL == m_pDevice)
        throw BurnerException::CreateBurnerException(NO_DEVICE);

    DataDisc* pDataDisc = Library::createDataDisc();

    try
    {
        formatMedia(m_pDevice);

        m_pDevice->setWriteSpeedKB(settings.WriteSpeedKB);

        pDataDisc->setCallback(this);
        pDataDisc->setDevice(m_pDevice);
        pDataDisc->setSimulateBurn(0);
        pDataDisc->setWriteMethod(WriteMethod::BluRay);
        pDataDisc->setCloseDisc(settings.CloseDisc);

        // Set the session start address. This must be done before intializing the file system.
        pDataDisc->setSessionStartAddress(m_pDevice->newSessionStartAddress());

        // Multi-session. Find the last complete track number from the last session.
        if (settings.LoadLastTrack)
        {
            int nPrevTrackNumber = getLastCompleteTrack(m_pDevice);
            if (nPrevTrackNumber > 0)
            {
                // Set the track number here. DataDisc will load it later.
                pDataDisc->setLayoutLoadTrack(nPrevTrackNumber);
                pDataDisc->setDataOverwrite(DataOverwrite::Overwrite);
            }
        }

        // Set burning parameters
        pDataDisc->setImageType(settings.ImageType);
        pDataDisc->udfVolumeProps()->setRevision(settings.UdfRevision);

        if (settings.BDVideo)
            pDataDisc->udfVolumeProps()->setTextEncoding(UdfTextEncoding::Ansi);

        setVolumeProperties(pDataDisc, settings.VolumeLabel);

        // Set image layout
        setImageLayoutFromFolder(pDataDisc, settings.SourceFolder.c_str());

        // Burn 
        bool_t bRes = 0;
        while (true)
        {
            // Try to write the image
            bRes = pDataDisc->writeToDisc();
            if (!bRes)
            {
                // When error is: Cannot load image layout, most likely it is an empty formatted DVD+RW 
                // or empty formatted DVD-RW RO with one track.
                if (DataDiscError::CannotLoadImageLayout == pDataDisc->error()->code())
                {
                    // Set to 0 to disable loading filesystem from previous track
                    pDataDisc->setLayoutLoadTrack(0);

                    // retry writing
                    continue;
                }
            }

            break;
        }

        // Check result and show error message
        if (!bRes)
        {
            throw BurnerException::CreateDataDiscException(pDataDisc, m_pDevice);
        }

        if (settings.Eject)
            m_pDevice->eject(1);

        pDataDisc->release();
    }
    catch (...)
    {
        pDataDisc->release();
        throw;
    }
}

void Burner::format(const FormatSettings& settings)
{
    if (NULL == m_pDevice)
        throw BurnerException::CreateBurnerException(NO_DEVICE);

    MediaProfile::Enum mp = m_pDevice->mediaProfile();

    if (MediaProfile::DVDMinusRWSeq != mp && MediaProfile::DVDMinusRWRO != mp && MediaProfile::DVDPlusRW != mp && MediaProfile::BDRE != mp && MediaProfile::BDRSrm != mp)
        throw BurnerException::CreateBurnerException(FORMAT_NOT_SUPPORTED);

    m_pDevice->setCallback(this);

    bool_t bRes = 1;
    switch (mp)
    {
        case MediaProfile::DVDMinusRWRO:
            bRes = m_pDevice->format(settings.Quick ? FormatType::DVDMinusRWQuick : FormatType::DVDMinusRWFull);
            break;
        case MediaProfile::DVDMinusRWSeq:
            bRes = m_pDevice->format(settings.Quick ? FormatType::DVDMinusRWQuick : FormatType::DVDMinusRWFull);
            break;

        case MediaProfile::DVDPlusRW:
        {
            BgFormatStatus::Enum fmt = m_pDevice->bgFormatStatus();
            switch (fmt)
            {
                case BgFormatStatus::Completed:
                    if (settings.Force)
                        bRes = m_pDevice->format(FormatType::DVDPlusRWFull, 0, !settings.Quick);
                    break;
                case BgFormatStatus::NotFormatted:
                    bRes = m_pDevice->format(FormatType::DVDPlusRWFull, 0, !settings.Quick);
                    break;
                case BgFormatStatus::Partial:
                    bRes = m_pDevice->format(FormatType::DVDPlusRWRestart, 0, !settings.Quick);
                    break;
                case BgFormatStatus::Pending:
                    break;
            }
        }
            break;

            // BD-RE
        case MediaProfile::BDRE:
            bRes = m_pDevice->formatBD(BDFormatType::BDFull, BDFormatSubType::BDREQuickReformat);
            break;

            // Format for Pseudo-Overwrite (POW)
        case MediaProfile::BDRSrm:
            bRes = m_pDevice->formatBD(BDFormatType::BDFull, BDFormatSubType::BDRSrmPow);
            break;
    }

    if (!bRes)
    {
        throw BurnerException::CreateDeviceException(m_pDevice);
    }
}

void Burner::dismountMedia()
{
    if (NULL == m_pDevice)
        throw BurnerException::CreateBurnerException(NO_DEVICE);

    // Dismount the system volume. 
    // Device::Dismount works only on Windows NT, 2000 and XP.
    m_pDevice->dismount();
}

int Burner::getLastCompleteTrack(Device * pDevice)
{
    // Get the last track number from the last session if multisession option was specified
    int nLastTrack = 0;

    // Check for DVD+RW and DVD-RW RO random writable media. 
    MediaProfile::Enum mp = pDevice->mediaProfile();
    if (MediaProfile::DVDPlusRW == mp || MediaProfile::DVDMinusRWRO == mp ||
        MediaProfile::BDRE == mp || MediaProfile::BDRSrmPow == mp)
    {
        // DVD+RW and DVD-RW RO has only one session with one track
        if (pDevice->mediaFreeSpace() > 0)
            nLastTrack = 1;
    }
    else
    {
        // All other media is recorded using tracks and sessions and multi-session is no different 
        // than with the CD. 

        // Use the ReadDiskInfo method to get the last track number
        DiscInfo* di = pDevice->readDiscInfo();
        if (NULL != di)
        {
            nLastTrack = di->lastTrack();

            // ReadDiskInfo reports the empty space as a track too
            // That's why we need to go back one track to get the last completed track
            if (DiscStatus::Open == di->discStatus() || DiscStatus::Empty == di->discStatus())
                nLastTrack--;
            di->release();
        }
    }

    return nLastTrack;
}

bool_t Burner::formatMedia(Device * pDevice)
{
    if (pDevice->isMediaFormatted())
        return 1;

    pDevice->setCallback(this);

    if (m_pCallback)
        m_pCallback->onStatus(_TEXT("Formatting..."));

    MediaProfile::Enum mp = pDevice->mediaProfile();
    switch (mp)
    {
            // MP_BD_RE (needs to be formatted before the disc can be used)
        case MediaProfile::BDRE:
            pDevice->formatBD(BDFormatType::BDFull, BDFormatSubType::BDREQuickReformat);
            break;
    }

    // Must be DVD-R, DVD+R
    return 1;
}

std::wstring Burner::getDataDiscStatusString(DataDiscStatus::Enum eStatus)
{
    switch (eStatus)
    {
        case DataDiscStatus::BuildingFileSystem:
            return _TEXT("Building file system...");

        case DataDiscStatus::WritingFileSystem:
            return _TEXT("Writing file system...");

        case DataDiscStatus::WritingImage:
            return _TEXT("Writing image...");

        case DataDiscStatus::CachingSmallFiles:
            return _TEXT("Caching small files ...");

        case DataDiscStatus::CachingNetworkFiles:
            return _TEXT("Caching network files ...");

        case DataDiscStatus::CachingCDRomFiles:
            return _TEXT("Caching CD-ROM files ...");

        case DataDiscStatus::Initializing:
            return _TEXT("Initializing...");

        case DataDiscStatus::Writing:
            return _TEXT("Writing...");

        case DataDiscStatus::WritingLeadOut:
            return _TEXT("Flushing device cache and writing lead-out...");

        case DataDiscStatus::LoadingImageLayout:
            return _TEXT("Loading image layout from last track...");
    }

    return _TEXT("Unknown status...");
}

bool Burner::isWritePossible(Device *pDevice) const
{
    CDFeatures *cdFeatures = pDevice->cdFeatures();
    DVDFeatures *dvdFeatures = pDevice->dvdFeatures();
    BDFeatures *bdFeatures = pDevice->bdFeatures();

    bool cdWritePossible = cdFeatures->canWriteCDR() || cdFeatures->canWriteCDRW();
    bool dvdWritePossible = dvdFeatures->canWriteDVDMinusR() || dvdFeatures->canWriteDVDPlusR() ||
        dvdFeatures->canWriteDVDMinusRDL() || dvdFeatures->canWriteDVDPlusRDL() ||
        dvdFeatures->canWriteDVDMinusRW() || dvdFeatures->canWriteDVDPlusRW() ||
        dvdFeatures->canWriteDVDRam();
    bool bdWritePossible = bdFeatures->canWriteBDR() || bdFeatures->canWriteBDRE();

    return cdWritePossible || dvdWritePossible || bdWritePossible;
}
