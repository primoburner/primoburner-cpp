#include "stdafx.h"

#include "BurnerApp.h"
#include <primo/platform/ustring.h>

// For agrparse usage see:
// https://github.com/p-ranav/argparse

int main(int argc, char *argv[])
{
    argparse::ArgumentParser program("bluray-data");

    // List devices
    argparse::ArgumentParser list_devices("list-devices");
    list_devices.add_description("List the CD / DVD / BD devices attached to the system.");
    program.add_subparser(list_devices);

    // Eject
    argparse::ArgumentParser eject("eject");
    eject.add_description("Open device tray");
    eject.add_argument("--device-index").help("Index of the device to use.").required().scan<'i', int>();
    program.add_subparser(eject);

    // Close tray
    argparse::ArgumentParser closeTray("close-tray");
    closeTray.add_description("Close device tray");
    closeTray.add_argument("--device-index").help("Index of the device to use.").required().scan<'i', int>();
    program.add_subparser(closeTray);

    // Disc Info
    argparse::ArgumentParser discInfo("disc-info");
    discInfo.add_description("Show media profile and free space.");
    discInfo.add_argument("--device-index").help("Index of the device to use.").required().scan<'i', int>();
    program.add_subparser(discInfo);

    // Disc Info
    argparse::ArgumentParser speedInfo("speed-info");
    speedInfo.add_description("Show available write speeds. This depends on the media in the device.");
    speedInfo.add_argument("--device-index").help("Index of the device to use.").required().scan<'i', int>();
    program.add_subparser(speedInfo);

    // // Speed Info
    // var speedInfoSubCommand = new Command("speed-info", "Show available write speeds. This depends on the media in the device");
    // rootCommand.Add(speedInfoSubCommand);
    
    // speedInfoSubCommand.AddOption(deviceIndexOpt);
    
    // speedInfoSubCommand.SetHandler((deviceIndex) =>
    // {
    //     var app = new BurnerApp();
    //     app.SpeedInfo(deviceIndex);
    //     app.Dispose();
    // }, deviceIndexOpt);

    // Format
    argparse::ArgumentParser format("format");
    format.add_description("Format disc in device specified by --device-index");
    format.add_argument("-d", "--device-index").help("Index of the device to use.").required().scan<'i', int>();
    format.add_argument("-f", "--force").help("Index of the device to use.").flag();
    program.add_subparser(format);

    // Burn
    argparse::ArgumentParser burn("burn");
    burn.add_description("Burn a data folder");
    burn.add_argument("-d", "--device-index").help("Index of the device to use.").required().scan<'i', int>();
    burn.add_argument("-l", "--volume-label").help("Volume label.").required().default_value(std::string("DATADISC"));
    burn.add_argument("-o", "--overwrite").help("Overwrite existing data instead of appending to disc.").flag();
    burn.add_argument("-e", "--eject").help("Eject disc after writing.").flag();
    burn.add_argument("-s", "--source-folder").help("Folder to burn. The contents of the folder will be added to the disc.").required(); 

    program.add_subparser(burn);

    // Parse command line arguments
    try
    {
        program.parse_args(argc, argv);
    }
    catch (const std::exception &err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    // List devices
    if (program.is_subcommand_used("list-devices"))
    {
        BurnerApp app;
        app.listDevices();
    }
    // Eject
    else if (program.is_subcommand_used("eject"))
    {
        auto device_index = eject.get<int>("--device-index");

        BurnerApp app;
        app.eject(device_index);
    }
    // Close tray
    else if (program.is_subcommand_used("close-tray"))
    {
        auto device_index = closeTray.get<int>("--device-index");

        BurnerApp app;
        app.closeTray(device_index);
    }
    // Disc Info
    else if (program.is_subcommand_used("disc-info"))
    {
        auto device_index = discInfo.get<int>("--device-index");

        BurnerApp app;
        app.discInfo(device_index);
    }
    // Speed Info
    else if (program.is_subcommand_used("speed-info"))
    {
        auto device_index = speedInfo.get<int>("--device-index");

        BurnerApp app;
        app.speedInfo(device_index);
    }
    // Format
    else if (program.is_subcommand_used("format"))
    {
        auto device_index = format.get<int>("--device-index");
        auto force = format.get<bool>("--force");

        BurnerApp app;
        app.format(device_index, force);
    }
    // Burn
    else if (program.is_subcommand_used("burn"))
    {
        auto device_index = burn.get<int>("--device-index");
        auto volume_label = burn.get<std::string>("--volume-label");
        auto overwrite = burn.get<bool>("--overwrite");
        auto eject = burn.get<bool>("--eject");
        auto source_folder = burn.get<std::string>("--source-folder");

        if (!std::filesystem::exists(source_folder))
        {
            std::cout << source_folder << " does not exist" << std::endl;
            return 1;
        }

        bool bd_video = false;
        bool load_last_track = !overwrite;
        bool close_disc = false;

        BurnerApp app;
        app.burn(device_index,
                 primo::ustring(source_folder).wstr(),
                 UdfRevision::Revision102,
                 bd_video, 
                 primo::ustring(volume_label).wstr(),
                 load_last_track, 
                 close_disc, 
                 eject);
    }

    return 0;
}
