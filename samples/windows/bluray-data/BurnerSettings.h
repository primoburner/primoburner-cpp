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
	tstring ImageFile;
	tstring SourceFolder;
	
	tstring VolumeLabel;
	ImageTypeFlags::Enum ImageType;
	UdfRevision::Enum UdfRevision;
	bool BDVideo;

	CreateImageSettings()
	{
		ImageType = ImageTypeFlags::None;
		UdfRevision = UdfRevision::Revision102;
		BDVideo = false;
	}
};

// BurnImage Settings
struct BurnImageSettings
{
	tstring ImageFile;

	uint32_t WriteSpeedKb;
	
	bool CloseTrack;
	bool CloseSession;
	bool CloseDisc;

	bool Eject;

	BurnImageSettings()
	{
		WriteSpeedKb = 0;
		
		CloseTrack = true;
		CloseSession = true;
		CloseDisc = true;
		Eject = true;
	}
};

// Burn Settings
struct BurnSettings
{
	tstring SourceFolder;
	tstring VolumeLabel;

	ImageTypeFlags::Enum ImageType;
	UdfRevision::Enum UdfRevision;
	bool BDVideo;

	bool CacheSmallFiles;
	uint32_t SmallFilesCacheLimit;
	uint32_t SmallFileSize;

	uint32_t WriteSpeedKb;
	
	bool CloseTrack;
	bool CloseSession;
	bool CloseDisc;

	bool LoadLastTrack;

	bool Eject;

	BurnSettings()
	{
		ImageType = ImageTypeFlags::None;
		UdfRevision = UdfRevision::Revision102;
		BDVideo = false;

		CacheSmallFiles = false;
		SmallFilesCacheLimit = SMALL_FILES_CACHE_LIMIT;
		SmallFileSize = SMALL_FILE_SECTORS;

		WriteSpeedKb = 0;
		
		CloseTrack = true;
		CloseSession = true;
		CloseDisc = true;

		Eject = true;

		LoadLastTrack = false;
	}
};

// Format Settings
struct FormatSettings
{
	BDFormatType::Enum Type;
	BDFormatSubType::Enum SubType;

	// Constructor
	FormatSettings()	
	{
		Type = BDFormatType::BDFull;
		SubType = BDFormatSubType::BDREQuickReformat;
	}
};

