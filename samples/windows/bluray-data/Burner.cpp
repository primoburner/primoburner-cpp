#include "stdafx.h"
#include "Burner.h"

Burner::Burner(void)
{
    m_pEngine = NULL;
    m_pDevice = NULL;

    setIsOpen(false);
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
    Library::enableTraceLog(NULL, TRUE);

    m_pEngine = Library::createEngine();
    if (!m_pEngine->initialize()) 
    {
        m_pEngine->release();
        m_pEngine = NULL;

        throw BurnerException(ENGINE_INITIALIZATION, ENGINE_INITIALIZATION_TEXT);
    }

    setIsOpen(true);
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

const uint64_t Burner::calculateImageSize(const tstring& sourceFolder, ImageTypeFlags::Enum imageType, UdfRevision::Enum udfRevision)
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
    catch(...)
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
        throw BurnerException(NO_DEVICE, NO_DEVICE_TEXT);

    return m_pDevice->maxWriteSpeedKB();
}

const MediaProfile::Enum Burner::mediaProfile() const
{
    if (NULL == m_pDevice)
        throw BurnerException(NO_DEVICE, NO_DEVICE_TEXT);

    return m_pDevice->mediaProfile();
}

const tstring Burner::mediaProfileString() const
{
    WORD wProfile = mediaProfile();
    switch(wProfile)
    {
        case MediaProfile::CDRom:
            return _T("CD-ROM. Read only CD."); 

        case MediaProfile::CDR:
            return _T("CD-R. Write once CD."); 

        case MediaProfile::CDRW:
            return _T("CD-RW. Re-writable CD.");

        case MediaProfile::DVDRom:
            return _T("DVD-ROM. Read only DVD.");

        case MediaProfile::DVDMinusRSeq:
            return _T("DVD-R for Sequential Recording.");

        case MediaProfile::DVDMinusRDLSeq:	
            return _T("DVD-R DL Double Layer for Sequential Recording.");

        case MediaProfile::DVDMinusRDLJump:
            return _T("DVD-R DL Double Layer for Layer Jump Recording.");

        case MediaProfile::DVDRam:
            return _T("DVD-RAM ReWritable DVD.");

        case MediaProfile::DVDMinusRWRO:
            return _T("DVD-RW for Restricted Overwrite.");

        case MediaProfile::DVDMinusRWSeq:
            return _T("DVD-RW for Sequential Recording.");

        case MediaProfile::DVDPlusRW :
        {
            if (NULL == m_pDevice)
                throw BurnerException(NO_DEVICE, NO_DEVICE_TEXT);

            BgFormatStatus::Enum fmt = m_pDevice->bgFormatStatus();
            switch(fmt)
            {
                case BgFormatStatus::NotFormatted:
                    return _T("DVD+RW. Not formatted.");
                break;
                case BgFormatStatus::Partial:
                    return _T("DVD+RW. Partially formatted.");
                break;
                case BgFormatStatus::Pending:
                    return _T("DVD+RW. Background format is pending ...");
                break;
                case BgFormatStatus::Completed:
                    return _T("DVD+RW. Formatted.");
                break;
            }

            return _T("DVD+RW for Random Recording.");
        }

        case MediaProfile::DVDPlusR:
            return _T("DVD+R for Sequential Recording.");
        
        case MediaProfile::DVDPlusRDL:
            return _T("DVD+R DL Double Layer for Sequential Recording.");

        case MediaProfile::BDRom:
            return _T("BD-ROM Read only Blu-ray Disc.");

        case MediaProfile::BDRSrm:
        {
            if (mediaIsFullyFormatted())
                return _T("BD-R in Sequential Recording Mode with Spare Areas (SRM-POW).");

            return _T("BD-R in Sequential Recording Mode.");
        }

        case MediaProfile::BDRSrmPow:
            return _T("BD-R in Sequential Recording Mode with Pseudo-Overwrite (SRM+POW).");

        case MediaProfile::BDRRrm:
            return _T("BD-R in Random Recording Mode (RRM).");

        case MediaProfile::BDRE:
        {
            if (mediaIsFullyFormatted())
                return _T("BD-RE ReWritable Blu-ray Disc. Formatted.");
            
            return _T("BD-RE ReWritable Blu-ray Disc. Blank. Not formatted."); 
        }
        default:
            return _T("Unknown Profile.");
    }
}

const bool Burner::mediaIsBlank() const
{
    if (NULL == m_pDevice)
        throw BurnerException(NO_DEVICE, NO_DEVICE_TEXT);

    return 1 == m_pDevice->isMediaBlank();
}

const bool Burner::mediaIsFullyFormatted() const
{
    if (NULL == m_pDevice)
        throw BurnerException(NO_DEVICE, NO_DEVICE_TEXT);

    // Get media profile
    MediaProfile::Enum mp = m_pDevice->mediaProfile();

    // DVD+RW
    if (MediaProfile::DVDPlusRW == mp)
        return (BgFormatStatus::Completed == m_pDevice->bgFormatStatus());

    // DVD-RW for Restricted Overwrite
    if (MediaProfile::DVDMinusRWRO == mp)
        return m_pDevice->mediaCapacity() == m_pDevice->mediaFreeSpace();

    // BD-RE
    if (MediaProfile::BDRE == mp || MediaProfile::BDRSrm == mp || MediaProfile::BDRSrmPow == mp || MediaProfile::BDRRrm == mp)
        return 1 == m_pDevice->isMediaFormatted();

    return false;
}

const uint32_t Burner::deviceCacheSize() const
{
    if (NULL == m_pDevice)
        throw BurnerException(NO_DEVICE, NO_DEVICE_TEXT);

    return m_pDevice->internalCacheCapacity();
}

const uint32_t Burner::deviceCacheUsedSize() const
{
    if (NULL == m_pDevice)
        throw BurnerException(NO_DEVICE, NO_DEVICE_TEXT);

    return m_pDevice->internalCacheUsedSpace();
}

const uint32_t Burner::writeTransferKB() const
{
    if (NULL == m_pDevice)
        throw BurnerException(NO_DEVICE, NO_DEVICE_TEXT);

    return m_pDevice->writeTransferRate();
}


const uint32_t Burner::mediaFreeSpace() const
{
    if (NULL == m_pDevice)
        throw BurnerException(NO_DEVICE, NO_DEVICE_TEXT);

    return m_pDevice->mediaFreeSpace();
}

const SpeedVector& Burner::enumerateWriteSpeeds()
{
    if (NULL == m_pDevice)
        throw BurnerException(NO_DEVICE, NO_DEVICE_TEXT);

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
                    speed.TransferRate1xKB = (int32_t)Speed1xKB::BD;
                else if (m_pDevice->isMediaDVD())
                    speed.TransferRate1xKB = (int32_t)Speed1xKB::DVD;
                else
                    speed.TransferRate1xKB = (int32_t)Speed1xKB::CD;

                m_speedVector.push_back(speed);
            }
        }

    pSpeeds->release();
    return m_speedVector;
}

tstring get_DeviceTitle(Device* pDevice)
{
    char chLetter = pDevice->driveLetter();

    TCHAR tcsName[200] = {0};
    _stprintf(tcsName, _T("(%c:) - %s"), chLetter, pDevice->description());

    return tcsName;
}

const DeviceVector& Burner::enumerateDevices()
{
    if (!isOpen())
        throw BurnerException(BURNER_NOT_OPEN, BURNER_NOT_OPEN_TEXT);

    m_deviceVector.clear();

    DeviceEnum* pEnumerator = m_pEngine->createDeviceEnumerator();

    int32_t nDevices = pEnumerator->count();
    if (0 == nDevices) 
    {
        pEnumerator->release();
        throw BurnerException(NO_DEVICES, NO_DEVICES_TEXT);
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

            pDevice->release();
        }
    }

    pEnumerator->release();
    return m_deviceVector;
}


void Burner::selectDevice(uint32_t deviceIndex, bool exclusive)
{
    if (NULL != m_pDevice)
        throw BurnerException(DEVICE_ALREADY_SELECTED, DEVICE_ALREADY_SELECTED_TEXT);

    DeviceEnum* pEnumerator = m_pEngine->createDeviceEnumerator();
    Device* pDevice = pEnumerator->createDevice(deviceIndex, exclusive ? 1 : 0);
    if (NULL == pDevice)
    {
        pEnumerator->release();
        throw BurnerException(INVALID_DEVICE_INDEX, INVALID_DEVICE_INDEX_TEXT);
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

void Burner::setIsOpen(bool isOpen)
{
    m_isOpen = isOpen;
}

// IDataDiscCallback14
void Burner::onProgress(int64_t bytesWritten, int64_t all)
{
    if (m_pCallback)
        m_pCallback->onImageProgress(bytesWritten, all);
}

void Burner::onStatus(DataDiscStatus::Enum eStatus)
{
    if (m_pCallback)
    {
        tstring status = getDataDiscStatusString(eStatus);
        m_pCallback->onStatus(status);
    }
}

void Burner::onFileStatus(int32_t fileNumber, const char_t* filename, int32_t percentWritten) 
{
    if (m_pCallback)
    {
        tstring fileName1 = filename;
        m_pCallback->onFileProgress(fileNumber, fileName1, percentWritten);
    }
}

BOOL Burner::onContinueWrite() 
{
    if (m_pCallback)
        return m_pCallback->onContinue() ? TRUE : FALSE;

    return TRUE;
}

// IDeviceCallback13
void Burner::onFormatProgress(double progress)
{
    if (m_pCallback)
        m_pCallback->onFormatProgress(progress);
}

void Burner::onEraseProgress(double progress)
{
    // Not used for Blu-ray Media
    // if (m_pCallback)
    //     m_pCallback->onEraseProgress(progress);
}

// Uses FindFile and WIN32_FIND_DATA to get the file list
void Burner::processInputTree(DataFile * pCurrentFile, tstring& sCurrentPath)
{
    const primo::burner::ImageTypeFlags::Enum allImageTypes = (primo::burner::ImageTypeFlags::Enum)
                                                (primo::burner::ImageTypeFlags::Udf | 
                                                primo::burner::ImageTypeFlags::Iso9660 | 
                                                primo::burner::ImageTypeFlags::Joliet);


    tstring sSearchFor = sCurrentPath + _T("/*") ;

    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = FindFirstFile(sSearchFor.c_str(), &FindFileData);

    if (hFind == INVALID_HANDLE_VALUE)
        throw BurnerException(FILE_NOT_FOUND, FILE_NOT_FOUND_TEXT);

    do 
    {
        // Keep the original file name
        tstring sFileName = FindFileData.cFileName; 
        
        // Get the full path
        tstring sFullPath = sCurrentPath + tstring(_T("/") + sFileName);

        if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // Skip the curent folder . and the parent folder .. 
            if (sFileName == _T(".")  || sFileName == _T(".."))
                continue;

            // Create a folder entry and scan it for the files
            DataFile * pDataFile = Library::createDataFile(); 

            pDataFile->setDirectory(TRUE);
            pDataFile->setPath(sFileName.c_str());			
            pDataFile->setLongFilename(sFileName.c_str());

            if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
                pDataFile->setHiddenMask(allImageTypes);

            pDataFile->setCreationTime(FindFileData.ftCreationTime);

            // Search for all files
            processInputTree(pDataFile, sFullPath);

            // Add this folder to the tree
            pCurrentFile->children()->add(pDataFile);

            pDataFile->release();
        } 
        else
        {
            // File
            DataFile * pDataFile = Library::createDataFile(); 

            pDataFile->setDirectory(FALSE);
            pDataFile->setPath(sFullPath.c_str());			
            pDataFile->setLongFilename(sFileName.c_str());

            if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
                pDataFile->setHiddenMask(allImageTypes);

            pDataFile->setCreationTime(FindFileData.ftCreationTime);

            // Add to the tree
            pCurrentFile->children()->add(pDataFile);

            pDataFile->release();
        }
    } 
    while (FindNextFile (hFind, &FindFileData));

    FindClose(hFind);
}

void Burner::setImageLayoutFromFolder(DataDisc* pDataDisc, LPCTSTR sourceFolder)
{
    // Create directory structure
    DataFile * pDataFile = Library::createDataFile(); 

    try
    {
        // Full path to the source folder
        tstring sFullPath = sourceFolder;

        // Set up the root of the image file system
        pDataFile->setDirectory(TRUE);
        pDataFile->setPath(_T("\\"));			
        pDataFile->setLongFilename(_T("\\"));

        processInputTree(pDataFile, sFullPath);

        // Set image layout
        if (!pDataDisc->setImageLayout(pDataFile))
        {
            throw BurnerException(pDataDisc);
        }

        pDataFile->release();
    }
    catch(...)
    {
        pDataFile->release();
        throw;
    }
}

void Burner::setVolumeProperties(DataDisc* pDataDisc, const tstring& volumeLabel, primo::burner::ImageTypeFlags::Enum imageType)
{
    // set volume times
    SYSTEMTIME st;
    GetSystemTime(&st);

    FILETIME ft;
    SystemTimeToFileTime(&st, &ft);
    
    if((ImageTypeFlags::Iso9660 & imageType) ||
        (ImageTypeFlags::Joliet & imageType))
    {
        IsoVolumeProps *iso = pDataDisc->isoVolumeProps();
        
        iso->setVolumeLabel(volumeLabel.c_str());

        // Sample settings. Replace with your own data or leave empty
        iso->setSystemID(_T("WINDOWS"));
        iso->setVolumeSet(_T("SET"));
        iso->setPublisher(_T("PUBLISHER"));
        iso->setDataPreparer(_T("PREPARER"));
        iso->setApplication(_T("DVDBURNER"));
        iso->setCopyrightFile(_T("COPYRIGHT.TXT"));
        iso->setAbstractFile(_T("ABSTRACT.TXT"));
        iso->setBibliographicFile(_T("BIBLIO.TXT"));

        iso->setVolumeCreationTime(ft);
    }

    if(ImageTypeFlags::Joliet & imageType)
    {
        JolietVolumeProps *joliet = pDataDisc->jolietVolumeProps();
        
        joliet->setVolumeLabel(volumeLabel.c_str());

        // Sample settings. Replace with your own data or leave empty
        joliet->setSystemID(_T("WINDOWS"));
        joliet->setVolumeSet(_T("SET"));
        joliet->setPublisher(_T("PUBLISHER"));
        joliet->setDataPreparer(_T("PREPARER"));
        joliet->setApplication(_T("DVDBURNER"));
        joliet->setCopyrightFile(_T("COPYRIGHT.TXT"));
        joliet->setAbstractFile(_T("ABSTRACT.TXT"));
        joliet->setBibliographicFile(_T("BIBLIO.TXT"));

        joliet->setVolumeCreationTime(ft);
    }

    if(ImageTypeFlags::Udf & imageType)
    {
        UdfVolumeProps *udf = pDataDisc->udfVolumeProps();
        
        udf->setVolumeLabel(volumeLabel.c_str());

        // Sample settings. Replace with your own data or leave empty
        udf->setVolumeSet(_T("SET"));
        udf->setCopyrightFile(_T("COPYRIGHT.TXT"));
        udf->setAbstractFile(_T("ABSTRACT.TXT"));

        udf->setVolumeCreationTime(ft);
    }
}

void Burner::closeTray()
{
    if (NULL == m_pDevice)
        throw BurnerException(NO_DEVICE, NO_DEVICE_TEXT);

    m_pDevice->eject(0);
}

void Burner::eject()
{
    if (NULL == m_pDevice)
        throw BurnerException(NO_DEVICE, NO_DEVICE_TEXT);

    m_pDevice->eject(1);
}

void Burner::createImage(const CreateImageSettings& settings)
{
    DataDisc* pDataCD = Library::createDataDisc();

    try
    {
        setVolumeProperties(pDataCD, settings.VolumeLabel, settings.ImageType);
        pDataCD->setImageType(settings.ImageType);
        pDataCD->udfVolumeProps()->setRevision(settings.UdfRevision);

        if(settings.BDVideo)
            pDataCD->udfVolumeProps()->setTextEncoding(UdfTextEncoding::Ansi);

        pDataCD->setCallback(this);

        setImageLayoutFromFolder(pDataCD, settings.SourceFolder.c_str());

        // Create the image file
        if (!pDataCD->writeToImageFile(settings.ImageFile.c_str())) 
        {
            throw BurnerException(pDataCD);
        }

        pDataCD->release();
    }
    catch(...)
    {
        pDataCD->release();
        throw;
    }
}

void Burner::burnImage(const BurnImageSettings& settings) 
{
    if (NULL == m_pDevice)
        throw BurnerException(NO_DEVICE, NO_DEVICE_TEXT);

    DataDisc * pDataDisc = Library::createDataDisc();

    try
    {
        m_pDevice->setWriteSpeedKB(settings.WriteSpeedKb);
        formatMedia(m_pDevice);

        pDataDisc->setCallback(this);
        pDataDisc->setDevice(m_pDevice);
        pDataDisc->setSimulateBurn(FALSE);
        pDataDisc->setWriteMethod(WriteMethod::BluRay);
        pDataDisc->setCloseTrack(settings.CloseTrack);
        pDataDisc->setCloseSession(settings.CloseSession);
        pDataDisc->setCloseDisc(settings.CloseDisc);

        // Write the image to the DVD
        if (!pDataDisc->writeImageToDisc(settings.ImageFile.c_str())) 
        {
            throw BurnerException(pDataDisc);
        }

        if (settings.Eject)
            m_pDevice->eject(TRUE);

        pDataDisc->release();
    }
    catch(...)
    {
        pDataDisc->release();
        throw;
    }
}

void Burner::burn(const BurnSettings& settings) 
{
    if (NULL == m_pDevice)
        throw BurnerException(NO_DEVICE, NO_DEVICE_TEXT);

    DataDisc* pDataDisc = Library::createDataDisc();

    try
    {
        formatMedia(m_pDevice);

        m_pDevice->setWriteSpeedKB(settings.WriteSpeedKb);

        pDataDisc->setCallback(this);
        pDataDisc->setDevice(m_pDevice);
        pDataDisc->setSimulateBurn(FALSE);
        pDataDisc->setWriteMethod(WriteMethod::BluRay);
        pDataDisc->setCloseTrack(settings.CloseTrack);
        pDataDisc->setCloseSession(settings.CloseSession);
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

        if(settings.BDVideo)
            pDataDisc->udfVolumeProps()->setTextEncoding(UdfTextEncoding::Ansi);

        setVolumeProperties(pDataDisc, settings.VolumeLabel, settings.ImageType);

        // Set image layout
        setImageLayoutFromFolder(pDataDisc, settings.SourceFolder.c_str());

        // Burn 
        BOOL bRes = FALSE;
        while (true)
        {
            // Try to write the image
            bRes = pDataDisc->writeToDisc();
            if (!bRes)
            {
                // When error is: Cannot load image layout, most likely it is an empty formatted DVD+RW 
                // or empty formatted DVD-RW RO with one track.
                if((pDataDisc->error()->facility() == ErrorFacility::DataDisc) &&
                    (pDataDisc->error()->code() == DataDiscError::CannotLoadImageLayout))
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
            throw BurnerException(pDataDisc);
        }

        if (settings.Eject)
            m_pDevice->eject(TRUE);

        pDataDisc->release();
    }
    catch(...)
    {
        pDataDisc->release();
        throw;
    }
}

void Burner::format(const FormatSettings& settings) 
{
    if (NULL == m_pDevice)
        throw BurnerException(NO_DEVICE, NO_DEVICE_TEXT);

    MediaProfile::Enum mp = m_pDevice->mediaProfile();

    if (MediaProfile::BDRE != mp &&
        MediaProfile::BDRSrm != mp)
    {
        throw BurnerException(FORMAT_NOT_SUPPORTED, FORMAT_NOT_SUPPORTED_TEXT);
    }

    m_pDevice->setCallback(this);

    BOOL bRes = TRUE;
    switch(mp)
    {
        // BD-R
        case MediaProfile::BDRSrm:
            bRes = m_pDevice->formatBD(BDFormatType::BDFull, settings.SubType);
        break;

        // BD-RE
        case MediaProfile::BDRE:
            bRes = m_pDevice->formatBD(BDFormatType::BDFull, settings.SubType);
        break;
    }

    if (!bRes)
    {
        throw BurnerException(m_pDevice);
    }
}

void Burner::dismountMedia()
{
    if (NULL == m_pDevice)
        throw BurnerException(NO_DEVICE, NO_DEVICE_TEXT);

    // Dismount the system volume. 
    // IDevice::Dismount works only on Windows 10 and Windows Server 2016 and later.
    m_pDevice->dismount();
}

int Burner::getLastCompleteTrack(Device * pDevice)
{
    // Get the last track number from the last session if multisession option was specified
    int nLastTrack = 0;

    // Check for DVD+RW and DVD-RW RO random writable media. 
    MediaProfile::Enum mp = pDevice->mediaProfile();

    if ((MediaProfile::DVDPlusRW == mp) || (MediaProfile::DVDMinusRWRO == mp) || 
        (MediaProfile::BDRE == mp)  || (MediaProfile::BDRSrmPow == mp) || (MediaProfile::DVDRam == mp))
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
        DiscInfo *pDI = pDevice->readDiscInfo();
        if(NULL != pDI)
        {
            nLastTrack = pDI->lastTrack();
            
            // readDiskInfo reports the empty space as a track too
            // That's why we need to go back one track to get the last completed track
            if ((DiscStatus::Open == pDI->discStatus()) || (DiscStatus::Empty == pDI->discStatus()))
                nLastTrack--;

            pDI->release();
        }
    }

    return nLastTrack;
}

BOOL Burner::formatMedia(Device * pDevice)
{
    if (pDevice->isMediaFormatted())
        return TRUE;

    pDevice->setCallback(this);

    if (m_pCallback)
        m_pCallback->onStatus(_T("Formatting..."));

    MediaProfile::Enum mp = pDevice->mediaProfile();
    switch(mp)
    {
        // MP_BD_RE (needs to be formatted before the disc can be used)
    case MediaProfile::BDRE:
            pDevice->formatBD(BDFormatType::BDFull, BDFormatSubType::BDREQuickReformat);
        break;
    }

    // Must be DVD-R, DVD+R
    return TRUE;
}

tstring Burner::getDataDiscStatusString(DataDiscStatus::Enum eStatus)
{
    switch(eStatus)
    {
        case DataDiscStatus::BuildingFileSystem:
            return tstring(_T("Building filesystem..."));
        
        case DataDiscStatus::WritingFileSystem:
            return tstring(_T("Writing filesystem..."));

        case DataDiscStatus::WritingImage:
            return tstring(_T("Writing image..."));

        case DataDiscStatus::CachingSmallFiles:
            return tstring(_T("Caching small files ..."));

        case DataDiscStatus::CachingNetworkFiles:
            return tstring(_T("Caching network files ..."));

        case DataDiscStatus::CachingCDRomFiles:
            return tstring(_T("Caching CD-ROM files ..."));

        case DataDiscStatus::Initializing:
            return tstring(_T("Initializing..."));

        case DataDiscStatus::Writing:
            return tstring(_T("Writing..."));

        case DataDiscStatus::WritingLeadOut:
            return tstring(_T("Flushing device cache and writing lead-out..."));

        case DataDiscStatus::LoadingImageLayout:
            return tstring(_T("Loading image layout from last track..."));
    }

    return tstring(_T("Unknown status..."));
}

bool Burner::isWritePossible(Device *device) const
{
    CDFeatures *cdfeatures = device->cdFeatures();
    DVDFeatures *dvdfeatures = device->dvdFeatures();
    BDFeatures *bdfeatures = device->bdFeatures();

    bool cdWritePossible = cdfeatures->canWriteCDR() || cdfeatures->canWriteCDRW();
    bool dvdWritePossible = dvdfeatures->canWriteDVDMinusR() || dvdfeatures->canWriteDVDMinusRDL() ||
        dvdfeatures->canWriteDVDPlusR() || dvdfeatures->canWriteDVDPlusRDL() ||
        dvdfeatures->canWriteDVDMinusRW() || dvdfeatures->canWriteDVDPlusRW() ||
        dvdfeatures->canWriteDVDRam();
    bool bdWritePossible = bdfeatures->canWriteBDR() || bdfeatures->canWriteBDRE();
    return cdWritePossible || dvdWritePossible || bdWritePossible;
}
