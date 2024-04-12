#pragma once

#include "stdafx.h"

// User Errors
#define ENGINE_INITIALIZATION   ((uint32_t)-1)
#define ENGINE_INITIALIZATION_TEXT  _TEXT("PrimoBurner engine initialization error.")

#define BURNER_NOT_OPEN     ((uint32_t)-2)
#define BURNER_NOT_OPEN_TEXT   _TEXT("Burner not open.")

#define NO_DEVICES      ((uint32_t)-3)
#define NO_DEVICES_TEXT     _TEXT("No CD/DVD/BD devices are available.")

#define NO_DEVICE      ((uint32_t)-4)
#define NO_DEVICE_TEXT     _TEXT("No device selected.") 

#define DEVICE_ALREADY_SELECTED   ((uint32_t)-5)
#define DEVICE_ALREADY_SELECTED_TEXT _TEXT("Device already selected.") 

#define INVALID_DEVICE_INDEX   ((uint32_t)-6)
#define INVALID_DEVICE_INDEX_TEXT  _TEXT("Invalid device index.")

#define FORMAT_NOT_SUPPORTED   ((uint32_t)-8)
#define FORMAT_NOT_SUPPORTED_TEXT  _TEXT("Format is supported only for DVD-RW, DVD+RW media, BD-RE and BD-R for Sequential Recording.")

#define FILE_NOT_FOUND     ((uint32_t)-9)
#define FILE_NOT_FOUND_TEXT    _TEXT("File not found while processing source folder.")

#define NO_WRITER_DEVICES    ((uint32_t)-10)
#define NO_WRITER_DEVICES_TEXT   _TEXT("No CD/DVD/BD writers are available.")

class BurnerException {
protected:

    std::wstring m_message;
    uint32_t m_error;

public:

    BurnerException() {
        m_error = 0;
        m_message = _TEXT("No Error.");
    }

    BurnerException(const BurnerException& other) {
        m_message = other.m_message;
        m_error = other.m_error;
    }

    BurnerException(const uint32_t error, const tstring message)
    {
        m_error = error;
        m_message = message;
    }    

    BurnerException(const uint32_t error) {
        m_error = error;
        switch (m_error) {
            case ENGINE_INITIALIZATION:
                m_message = ENGINE_INITIALIZATION_TEXT;
                break;
            case BURNER_NOT_OPEN:
                m_message = BURNER_NOT_OPEN_TEXT;
                break;
            case NO_DEVICE:
                m_message = NO_DEVICE_TEXT;
                break;
            case NO_DEVICES:
                m_message = NO_DEVICES_TEXT;
                break;
            case DEVICE_ALREADY_SELECTED:
                m_message = DEVICE_ALREADY_SELECTED_TEXT;
                break;
            case INVALID_DEVICE_INDEX:
                m_message = INVALID_DEVICE_INDEX_TEXT;
                break;
            case FORMAT_NOT_SUPPORTED:
                m_message = FORMAT_NOT_SUPPORTED_TEXT;
                break;
            case FILE_NOT_FOUND:
                m_message = FILE_NOT_FOUND_TEXT;
                break;
            case NO_WRITER_DEVICES:
                m_message = NO_WRITER_DEVICES_TEXT;
                break;
        }
    }

    const std::wstring& message() const {
        return m_message;
    }

    const uint32_t error() const {
        return m_error;
    }

    static BurnerException CreateDataDiscException(DataDisc* dataDisc, Device* device);

    static BurnerException CreateDeviceException(Device* device);

    static BurnerException CreateSystemErrorException(int errorCode, ErrorFacility::Enum facility);

    static BurnerException CreateBurnerException(int errorCode);
};

inline std::wostream& formatHex(std::wostream& streamOut) {
    return streamOut << L"0x" << std::setw(8) << std::setfill(L'0') << std::hex;
}

class DataDiscException : public BurnerException {
public:

    DataDiscException(DataDisc* dataDisc) {
        if (NULL != dataDisc) {
            const ErrorInfo* info = dataDisc->error();
            m_error = info->code();

            std::wstringstream msg;
            msg << _TEXT("Error detected:\n\tError Facility: ") << info->facility() << L"\n\t" << formatHex << m_error << L"\n\t" << primo::ustring(info->message());

            m_message = msg.str();
        }
    }
};

class BurnerDeviceException : public BurnerException {
public:

    BurnerDeviceException(Device* device) {
        if (NULL != device) {
            const ErrorInfo* info = device->error();
            m_error = info->code();

            std::wstringstream msg;
            msg << _TEXT("Error detected:\n\tError Facility: ") << info->facility() << L"\n\t" << formatHex << m_error << L"\n\t" << primo::ustring(info->message());

            m_message = msg.str();
        }
    }
};

class BurnerSystemErrorException : public BurnerException {
public:

    BurnerSystemErrorException(int errorCode, ErrorFacility::Enum facility) : BurnerException(errorCode) {
        switch (facility) {
            case ErrorFacility::SystemPosix:
                char pMessage[1024] = {0};
                strerror_r(m_error, pMessage, sizeof (pMessage) - 1);
                m_message = primo::ustring(pMessage).wstr();
                break;
        }
    }
};
