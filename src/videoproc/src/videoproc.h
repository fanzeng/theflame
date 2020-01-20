#ifndef THEFLAME_VIDEOPROC_H
#define THEFLAME_VIDEOPROC_H
#include <string>
#include <vector>
#include "logging.h"

int vtoi(const std::string& video_filename,
        const std::string& output_dirname,
        const std::string& output_ext,
        int m_verbosity=0,
        float resize_ratio=1.,
        int out_height=0.,
        int out_width=0.
);

int itov(const std::vector<std::string>& vec_input_filename,
        const std::string& output_dirname,
        const std::string& output_ext,
        int m_verbosity=0,
        int frames_per_image=1,
        int frames_per_second=1,
        float resize_ratio=1.,
        int out_height=0.,
        int out_width=0.,
        bool show_popup=false
);
#endif /* THEFLAME_VIDEOPROC_H */ 
