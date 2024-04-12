#pragma once

#include "stdafx.h"

enum
{
	SMALL_FILES_CACHE_LIMIT	= 20000,
	SMALL_FILE_SECTORS		= 10, 
	MAX_SMALL_FILE_SECTORS	= 1000
};

// CreateImage Settings
struct CreateImageSettings
{
	std::wstring ImageFile;
	std::wstring SourceFolder;
	
	std::wstring VolumeLabel;
    ImageTypeFlags::Enum ImageType;
    UdfRevision::Enum UdfRevision;
	bool BDVideo;

	CreateImageSettings()
	{
		ImageType = ImageTypeFlags::None;
		UdfRevision = UdfRevision::Revision102;;
		BDVideo = false;
	}
};

// BurnImage Settings
struct BurnImageSettings
{
    std::wstring ImageFile;

	uint32_t WriteSpeedKB;
	
	bool CloseDisc;
	bool Eject;

	BurnImageSettings()
	{
		WriteSpeedKB = 0;
		
		CloseDisc = true;
		Eject = true;
	}
};

// Burn Settings
struct BurnSettings
{
    std::wstring SourceFolder;
	std::wstring VolumeLabel;

    ImageTypeFlags::Enum ImageType;
    UdfRevision::Enum UdfRevision;
	bool BDVideo;

	bool CacheSmallFiles;
	uint32_t SmallFilesCacheLimit;
	uint32_t SmallFileSize;

	uint32_t WriteSpeedKB;
	
	bool LoadLastTrack;
	bool CloseDisc;
	bool Eject;

	BurnSettings()
	{
		ImageType = ImageTypeFlags::None;
		UdfRevision = UdfRevision::Revision102;
		BDVideo = false;

		CacheSmallFiles = false;
		SmallFilesCacheLimit = SMALL_FILES_CACHE_LIMIT;
		SmallFileSize = SMALL_FILE_SECTORS;

		WriteSpeedKB = 0;

		LoadLastTrack = false;
		CloseDisc = true;
		Eject = true;
	}
};

// Format Settings
struct FormatSettings
{
	bool Quick; 		// Quick format
	bool Force;			// Format even if disc is already formatted

	BDFormatType::Enum Type;
	BDFormatSubType::Enum SubType;

	// Constructor
	FormatSettings()	
	{
		Quick = true;
		Force = false;

		Type = BDFormatType::BDFull;
		SubType = BDFormatSubType::BDREQuickReformat;
	}
};

