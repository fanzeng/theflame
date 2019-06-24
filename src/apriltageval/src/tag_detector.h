#ifndef TAG_DETECTOR_H
#define TAG_DETECTOR_H

#include<opencv2/opencv.hpp>
#include"apriltag_all.h"

class TagDetector {
public:
    TagDetector();
    int detect_tag(const std::string filename_image_with_tag);
    std::vector<size_t> detect_tag_for_dir(const std::vector<std::string> vec_filename_image_with_tag);
};


#endif /* TAG_DETECTOR_H */

