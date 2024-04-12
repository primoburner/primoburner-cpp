/*
 *  BurnerException.cpp
 *  DVDBurner
 *
 *  Copyright 2011 PrimoSoftware Corp. All rights reserved.
 *
 */

#include "BurnerException.h"

BurnerException BurnerException::CreateDataDiscException(DataDisc* dataDisc, Device* device)
{
	if (NULL != dataDisc)
	{
        const ErrorInfo* info = dataDisc->error();
		switch(info->facility())
		{
			case ErrorFacility::Device:
				return CreateDeviceException(device);
			case ErrorFacility::SystemPosix:
				return CreateSystemErrorException(info->code(), info->facility());
			default:
				return DataDiscException(dataDisc);
		}
	}
	return BurnerException();
}

BurnerException BurnerException::CreateDeviceException(Device* device)
{
	if (NULL != device)
	{
		const ErrorInfo* info = device->error();
		switch(info->facility())
		{
			case ErrorFacility::SystemPosix:
				return CreateSystemErrorException(info->code(), info->facility());
			default:
				return BurnerDeviceException(device);
		}
	}
	return BurnerException();
}

BurnerException BurnerException::CreateSystemErrorException(int errorCode, ErrorFacility::Enum facility)
{
	return BurnerSystemErrorException(errorCode, facility);
}

BurnerException BurnerException::CreateBurnerException(int errorCode)
{
	return BurnerException(errorCode);
}