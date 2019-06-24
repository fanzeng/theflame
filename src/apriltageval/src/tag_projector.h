#ifndef TAG_PROJECTOR_H
#define TAG_PROJECTOR_H
#include<boost/filesystem.hpp>
#include<opencv2/opencv.hpp>
#include <ctime>

class TagProjector {
public:
    TagProjector(std::string tag_path_, std::string out_path_, bool do_distort_tag_ = false, bool do_add_dust_ = false) {
        tag_path = tag_path_;
        out_path = out_path_;
        path_tag_path = boost::filesystem::path(tag_path);
        do_distort_tag = do_distort_tag_;
        do_add_dust = do_add_dust_;
        time_t timer;
        srand(time(&timer));
    };
    
    std::string project_tag(std::string filename_image_orignal, std::string tag_type, 
            std::string tag_label, std::string out_path);
    std::vector<std::string> project_tag_for_dir(std::string dirname_image_orignal, 
            std::string string_regex, std::string tag_type, std::vector<std::string> vec_tag_label, std::string out_path);

    
private:
    std::string out_path;
    std::string tag_path;
    boost::filesystem::path path_tag_path;
    float resize_ratio;
    bool do_distort_tag;
    bool do_add_dust;
    cv::Mat image_tag_original;
    cv::Mat image_original;
    cv::Mat image_projected;
    cv::Mat resize_tag(float resize_ratio);
    cv::Mat distort_tag(const cv::Mat& image, const cv::Mat& image_original_roi);
    cv::Mat add_dust(const cv::Mat& image_in, size_t num_dust, int radius_min, int radius_max);
};


#endif // TAG_PROJECTOR_H