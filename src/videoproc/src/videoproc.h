#ifndef THEFLAME_VIDEOPROC_H
#define THEFLAME_VIDEOPROC_H
#include <string>

int vtoi(const std::string& video_filename,
        const std::string& output_dirname,
        const std::string& output_ext,
        float resize_ratio=1.
);


#endif /* THEFLAME_VIDEOPROC_H */ 
