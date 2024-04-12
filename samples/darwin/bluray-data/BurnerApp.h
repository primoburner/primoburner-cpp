#pragma once

#include "Burner.h"
#include "BurnerCallback.h"

#include <indicators/progress_bar.hpp>
using namespace indicators;

class BurnerApp : public IBurnerCallback
{
public:
    BurnerApp();
    ~BurnerApp();

    void listDevices();
    
    void eject(int device_index);
    void closeTray(int device_index);

    void discInfo(int device_index);
    void speedInfo(int device_index);

    void format(int device_index, bool force);

    void burn(int device_index, const tstring &source_folder,
              UdfRevision::Enum udf_revision, bool bd_video, const tstring &volume_label,
              bool load_last_track, bool close_disc, bool eject);

    // IBurnerCallback
protected:
    void onStatus(const tstring &message);

    void onImageProgress(uint64_t pos, uint64_t all);
    void onFileProgress(int file, const tstring &fileName, int percentCompleted);

    void onFormatProgress(double percentCompleted);

    bool onContinue();

private:
    void showErrorMessage(BurnerException &burnerException);

    void createBurnProgressBars();
    void deleteBurnProgressBars();

private:
    Burner _burner;
    
    // Format progress bar
    ProgressBar* _format_progress_bar;

    // Burn progress bars
    ProgressBar* _device_progress_bar;
    ProgressBar* _image_progress_bar;
    ProgressBar* _file_progress_bar;
    MultiProgress<ProgressBar, 3>* _burn_multi_progress;
};
