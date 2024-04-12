#include "stdafx.h"

#include "BurnerApp.h"
#include "ConsoleUtils.h"

BurnerApp::BurnerApp()
{
    _burner.open();
    _burner.setCallback(this);

    _format_progress_bar = new ProgressBar{
        option::BarWidth{50},
        option::Start{"["},
        option::End{"]"},
        option::ShowPercentage{true},
        option::ForegroundColor{Color::yellow},
        option::FontStyles{std::vector<FontStyle>{FontStyle::bold}}};
}

BurnerApp::~BurnerApp()
{
    _burner.close();

    if (_format_progress_bar)
        delete _format_progress_bar;
}

void BurnerApp::listDevices()
{
    try
    {
        tabulate::Table table;
        table.add_row({"Index", "Title", "IsWriter"});

        const DeviceVector &devices = _burner.enumerateDevices();

        int deviceCount = 0;
        for (size_t i = 0; i < devices.size(); i++)
        {
            const DeviceInfo &dev = devices[i];

            table.add_row({std::to_string(dev.Index), _N(dev.Title), dev.IsWriter ? "Yes" : "No"});

            if (dev.IsWriter)
                deviceCount++;
        }

        if (0 == deviceCount)
            throw BurnerException(NO_WRITER_DEVICES, NO_WRITER_DEVICES_TEXT);

        std::cout << table << std::endl;
    }
    catch (BurnerException &bme)
    {
        showErrorMessage(bme);
    }
}

void BurnerApp::eject(int device_index)
{
    try
    {
        _burner.selectDevice(device_index, false);
        _burner.eject();
        _burner.releaseDevice();
    }
    catch (BurnerException &bme)
    {
        _burner.releaseDevice();
        showErrorMessage(bme);
    }
}

void BurnerApp::closeTray(int device_index)
{
    try
    {
        _burner.selectDevice(device_index, false);
        _burner.closeTray();
        _burner.releaseDevice();
    }
    catch (BurnerException &bme)
    {
        _burner.releaseDevice();
        showErrorMessage(bme);
    }
}

void BurnerApp::discInfo(int device_index) {
    try
    {
        _burner.selectDevice(device_index, false);

        // Get and display the media profile
        uint64_t freeSpace = (uint64_t)_burner.mediaFreeSpace() * (int)BlockSize::BD;

        // Media profile
        auto labelMediaType = _burner.mediaProfileString();
        auto mediaProfile = fmt::format("Media Profile: {}", _N(labelMediaType));
        std::cout << mediaProfile << std::endl;

        // Capacity
        auto labelFreeSpace = fmt::format("Media Free Space: {:.2f}GB", (double)freeSpace / 1e9);
        std::cout << labelFreeSpace << std::endl;

        _burner.releaseDevice();
    }
    catch (BurnerException &bme)
    {
        _burner.releaseDevice();
        showErrorMessage(bme);
    }
}

void BurnerApp::speedInfo(int device_index) {
    try
    {
        _burner.selectDevice(device_index, false);

        tabulate::Table table;
        table.add_row({"Index", "Name", "Transfer Rate KB/s"});

        const SpeedVector& speeds = _burner.enumerateWriteSpeeds();
        for (size_t i = 0; i < speeds.size(); i++)
        {
            const Speed &speed = speeds[i];
            table.add_row({
                std::to_string(i), 
                fmt::format("{:.1f}x", (double)speed.TransferRateKB / speed.TransferRate1xKB), 
                std::to_string(speed.TransferRateKB)
            });
        }

        std::cout << table << std::endl;

        _burner.releaseDevice();
    }
    catch (BurnerException &bme)
    {
        _burner.releaseDevice();
        showErrorMessage(bme);
    }
}

void BurnerApp::format(int device_index, bool force)
{
    show_console_cursor(false);

    _format_progress_bar->set_option(option::PrefixText{"Formatting "});
    _format_progress_bar->set_progress(0.0f);

    try
    {
        _burner.selectDevice(device_index, true);

        if (!force)
        {
            if (_burner.mediaIsFullyFormatted())
            {
                if (!console_utils::confirm("Media is already formatted. Do you want to format it again?"))
                {
                    _burner.releaseDevice();
                    return;
                }
            }

            if (!console_utils::confirm("Formatting will destroy all the information on the disc. Do you want to continue?"))
            {
                _burner.releaseDevice();
                return;
            }
        }

        auto mediaProfile = _burner.mediaProfile();
        FormatSettings settings;
        {
            settings.Type = BDFormatType::BDFull;
            if (MediaProfile::BDRE == mediaProfile)
                settings.SubType = BDFormatSubType::BDREQuickReformat;

            if (MediaProfile::BDRSrm == mediaProfile)
                settings.SubType = BDFormatSubType::BDRSrmPow;
        }

        _burner.format(settings);
        _burner.releaseDevice();
    }
    catch (BurnerException &bme)
    {
        _burner.releaseDevice();
        showErrorMessage(bme);
    }

    show_console_cursor(true);
}

void BurnerApp::burn(int device_index, const tstring &source_folder,
                     UdfRevision::Enum udf_revision, bool bd_video, const tstring &volume_label,
                     bool load_last_track, bool close_disc, bool eject)
{
    show_console_cursor(false);

    createBurnProgressBars();

    try
    {
        _burner.selectDevice(device_index, true);

        BurnSettings settings;
        {
            settings.SourceFolder = source_folder;
            settings.ImageType = ImageTypeFlags::Udf;
            settings.UdfRevision = udf_revision;
            settings.BDVideo = bd_video;
            settings.VolumeLabel = volume_label;

            // TODO:
            // settings.WriteSpeedKb = write_speed;

            settings.CloseDisc = close_disc;
            settings.LoadLastTrack = load_last_track;
            settings.Eject = eject;
        }

        _burner.burn(settings);
        _burner.releaseDevice();
    }
    catch (BurnerException &bme)
    {
        _burner.releaseDevice();
        showErrorMessage(bme);
    }

    deleteBurnProgressBars();

    show_console_cursor(true);
}

// Event handlers
void BurnerApp::onImageProgress(int64_t pos, int64_t all)
{
    try
    {
        // get device internal buffer
        double dd = (double)_burner.deviceCacheUsedSize();
        dd = dd * 100.0 / (double)_burner.deviceCacheSize();
        float usedCachePercent = (float)dd;

        dd = (double)pos * 100.0 / (double)all;
        float percentCompleted = (float)dd;

        // get actual write speed (KB/s)
        // int actualWriteSpeed = _burner.writeTransferKB();

        _burn_multi_progress->set_progress<0>(usedCachePercent);
        _burn_multi_progress->set_progress<2>(percentCompleted);
    }
    catch (...)
    {
    }
}

void BurnerApp::onFileProgress(int file, const tstring &fileName, int percentCompleted)
{
    _file_progress_bar->set_option(option::PostfixText{primo::ustring(fileName).str()});
    _burn_multi_progress->set_progress<1>((float)percentCompleted);

    // Another way to show progress
    // fmt::print("File: {} {:.2f}% ...\n", _N(fileName), percentCompleted);
}

bool BurnerApp::onContinue()
{
    // Do nothing
    return true;
}

void BurnerApp::onFormatProgress(double percentCompleted)
{
    _format_progress_bar->set_progress(percentCompleted);

    // Another way to show progress
    // fmt::print("Formatting {:.2f}% ...\n", percentCompleted);
}

void BurnerApp::onStatus(const tstring &message)
{
    _image_progress_bar->set_option(option::PostfixText{primo::ustring(message).str()});

    // Another way to show progress
    // fmt::print("{}\n", _N(message));
}

void BurnerApp::showErrorMessage(BurnerException &burnerException)
{
    fmt::print("{} (0x{:x})\n", _N(burnerException.message()), burnerException.error());
}

void BurnerApp::createBurnProgressBars()
{
    _device_progress_bar = new ProgressBar{
        option::PrefixText{"Device Cache "},
        option::BarWidth{50},
        option::Start{"["},
        option::End{"]"},
        option::ShowPercentage{true},
        option::ForegroundColor{Color::cyan},
        option::FontStyles{std::vector<FontStyle>{FontStyle::bold}}};

    _file_progress_bar = new ProgressBar{
        option::PrefixText{"File         "},
        option::BarWidth{50},
        option::Start{"["},
        option::End{"]"},
        option::ShowPercentage{true},
        option::ForegroundColor{Color::yellow},
        option::FontStyles{std::vector<FontStyle>{FontStyle::bold}}};

    _image_progress_bar = new ProgressBar{
        option::PrefixText{"Image        "},
        option::BarWidth{50},
        option::Start{"["},
        option::End{"]"},
        option::ShowPercentage{true},
        option::ForegroundColor{Color::green},
        option::FontStyles{std::vector<FontStyle>{FontStyle::bold}}};

    _burn_multi_progress = new MultiProgress<indicators::ProgressBar, 3>(*_device_progress_bar, *_file_progress_bar, *_image_progress_bar);
}

void BurnerApp::deleteBurnProgressBars()
{
    if (_burn_multi_progress)
        delete _burn_multi_progress;
    _burn_multi_progress = nullptr;

    if (_image_progress_bar)
        delete _image_progress_bar;
    _image_progress_bar = nullptr;

    if (_file_progress_bar)
        delete _file_progress_bar;
    _file_progress_bar = nullptr;

    if (_device_progress_bar)
        delete _device_progress_bar;
    _device_progress_bar = nullptr;
}