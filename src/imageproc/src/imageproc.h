#ifndef THEFLAME_IMAGEPROC_H
#define THEFLAME_IMAGEPROC_H

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include "logging.h"

namespace fs = boost::filesystem;

class imageproc {
public:
    // vec_image_filename:
    // a vector of filenames of images to be processed
    // the 0th element of the vector is the "main" image to be processed
    // the rest of the vector is used to store
    // other files necessary for the processing.

    imageproc(const std::vector<std::string>& vec_filename,
            const std::string& output_dirname,
            const std::string& output_ext,
            int verbosity,
            float norm_factor,
            bool show_popup,
            bool yes_to_overwrite,
            std::vector<int> vec_additional_args
            ) :
    m_yes_to_overwrite(yes_to_overwrite),
    m_vec_filename(vec_filename),
    m_norm_factor(norm_factor),
    m_verbosity(verbosity),
    m_vec_additional_args(vec_additional_args) {
        m_input_image_filename = m_vec_filename.at(0);
        if (output_dirname.length() <= 0) {
            m_output_dirname = fs::path(m_input_image_filename).parent_path().string();
        } else {
            m_output_dirname = output_dirname;
        }
        fs::path output_path = m_output_dirname;
        COUT_INFO("image_filename=" << m_input_image_filename << std::endl)
        cv::VideoCapture capture(m_input_image_filename);
        m_input_image = cv::imread(m_input_image_filename, cv::IMREAD_COLOR);
        m_show_popup = show_popup;
        if (output_ext.length() > 0) {
            m_output_ext = "." + output_ext;
            COUT_DEBUG(
                    "setting m_output_ext to output_ext="
                    << m_output_ext << std::endl
                    );
        } else {
            m_output_ext = fs::path(m_input_image_filename).extension().string();
            COUT_INFO(
                    "no output extension specified, using input extension="
                    << m_output_ext << std::endl
                    );
        }
        std::string str_out_filename = fs::path(m_input_image_filename).stem().string() + m_output_ext;
        COUT_INFO("out file basename=" << str_out_filename << std::endl)
        m_output_filename = (output_path / fs::path(str_out_filename)).string();
        COUT_INFO("out file name=" << m_output_filename << std::endl)
        COUT_INFO("vec_additional_args.size()=" << m_vec_additional_args.size() << std::endl)

    }
    
    inline int image_absdiff() {
        cv::Mat input_image_0 = cv::imread(m_vec_filename.at(0), CV_LOAD_IMAGE_GRAYSCALE);
        cv::Mat input_image_1 = cv::imread(m_vec_filename.at(1), CV_LOAD_IMAGE_GRAYSCALE);
        m_output_image = image_absdiff(input_image_0, input_image_1);
        return 0;
    }
        
    inline int image_canny() {
        convert_input_image_to_gray();
        m_output_image = image_canny(m_input_image);
        return 0;
    }

    inline int image_clahe() {
        convert_input_image_to_gray();
        if (m_vec_additional_args.size() > 1) {
            m_output_image = image_clahe(
                    m_input_image,
                    m_vec_additional_args.at(0),
                    m_vec_additional_args.at(1)
                    );
        } else if (m_vec_additional_args.size() > 0) {
            m_output_image = image_clahe(
                    m_input_image,
                    m_vec_additional_args.at(0)
                    );
        } else {
            m_output_image = image_clahe(m_input_image);
        }
        return 0;
    }

    inline int image_dilate() {
        convert_input_image_to_gray();
        if (m_vec_additional_args.size() > 1) {
            m_output_image = image_dilate(
                    m_input_image,
                    m_vec_additional_args.at(0),
                    m_vec_additional_args.at(1)
                    );
        } else if (m_vec_additional_args.size() > 0) {
            m_output_image = image_dilate(
                    m_input_image,
                    m_vec_additional_args.at(0)
                    );
        } else {
            m_output_image = image_dilate(m_input_image);
        }
        return 0;
    }
        
    inline int image_dft() {
        convert_input_image_to_gray();
        m_output_image = image_dft(m_input_image);
        return 0;
    }
    
    inline int image_erode() {
        convert_input_image_to_gray();
        if (m_vec_additional_args.size() > 1) {
            m_output_image = image_erode(
                    m_input_image,
                    m_vec_additional_args.at(0),
                    m_vec_additional_args.at(1)
                    );
        } else if (m_vec_additional_args.size() > 0) {
            m_output_image = image_erode(
                    m_input_image,
                    m_vec_additional_args.at(0)
                    );
        } else {
            m_output_image = image_erode(m_input_image);
        }
        return 0;
    }
    
    inline int image_gaussianblur() {
        convert_input_image_to_gray();
        if (m_vec_additional_args.size() > 0) {
            m_output_image = image_gaussianblur(
                    m_input_image,
                    m_vec_additional_args.at(0)
                    );
        } else {
            m_output_image = image_gaussianblur(m_input_image);
        }
        return 0;
    }
        
    inline int nop() {
        m_output_image = normalize_intensity(m_input_image);
        return 0;
    }

    

    inline int image_mean() {
        convert_input_image_to_gray();
        m_output_image = image_mean(m_input_image);
        return 0;
    }

    inline int image_resize() {
        if (m_vec_additional_args.size() > 1) {
            m_output_image = image_resize(
                    m_input_image,
                    m_vec_additional_args.at(0),
                    m_vec_additional_args.at(1)
                    );
        } else {
            COUT_WARN("invalid new size. No resizing will be performed." << std::endl)
            m_output_image = m_input_image;
        }
        return 0;
    }
        inline int image_stdev() {
        convert_input_image_to_gray();
        m_output_image = image_stdev(m_input_image);
        return 0;
    }

    inline int image_threshold() {
        convert_input_image_to_gray();
        if (m_vec_additional_args.size() > 2) {
            m_output_image = image_threshold(
                    m_input_image,
                    m_vec_additional_args.at(0),
                    m_vec_additional_args.at(1),
                    m_vec_additional_args.at(2)
                    );
        } else if (m_vec_additional_args.size() > 1) {
            m_output_image = image_threshold(
                    m_input_image,
                    m_vec_additional_args.at(0),
                    m_vec_additional_args.at(1)
                    );
        } else if (m_vec_additional_args.size() > 0) {
            m_output_image = image_threshold(
                    m_input_image,
                    m_vec_additional_args.at(0)
                    );
        } else {
            m_output_image = image_threshold(m_input_image);
        }
        return 0;
    }

    int save_output_image();

private:
    std::vector<std::string> m_vec_filename;
    std::string m_output_dirname;
    std::string m_output_filename;
    std::string m_output_ext;
    float m_norm_factor = -1.;
    bool m_show_popup = false;
    bool m_yes_to_overwrite = false;
    int m_verbosity = 1;
    std::vector<int> m_vec_additional_args;
    std::string m_input_image_filename;
    cv::Mat m_input_image;
    cv::Mat m_output_image;

    cv::Mat cvdft(cv::Mat I);
    int convert_input_image_to_gray();
    cv::Mat normalize_intensity(const cv::Mat before);

    cv::Mat image_absdiff(const cv::Mat& input_image_0, const cv::Mat input_image_1);
    cv::Mat image_clahe(const cv::Mat& input_image, int clip_limit = 40, int tile_grid_size = 8);
    cv::Mat image_canny(const cv::Mat& input_image, int threshold1 = 10, int threshold2 = 350);
    cv::Mat image_dft(const cv::Mat& input_image);
    cv::Mat image_dilate(
            const cv::Mat input_image,
            int dilation_size = 1,
            int dilation_type = 0
            );
    cv::Mat image_erode(
            const cv::Mat input_image,
            int erosion_size = 1,
            int erosion_type = 0
            );
    cv::Mat image_gaussianblur(const cv::Mat input_image, int kernel_size = 3);
    inline cv::Mat image_mean(const cv::Mat& input_image) {
        if (m_vec_additional_args.size() > 0) {
            return image_meanstd(
                    input_image,
                    "mean",
                    m_vec_additional_args.at(0)
                    );
        } else {
            return image_meanstd(input_image, "mean");
        }
    }
    cv::Mat image_meanstd(const cv::Mat& input_image, std::string output_option, int patch_size = 17);
    cv::Mat image_resize(const cv::Mat input_image, int width, int height);
        inline cv::Mat image_stdev(const cv::Mat& input_image) {
        if (m_vec_additional_args.size() > 0) {
            return image_meanstd(
                    input_image,
                    "stdev",
                    m_vec_additional_args.at(0)
                    );
        } else {
            return image_meanstd(input_image, "stdev");
        }
    }
    cv::Mat image_threshold(
            const cv::Mat input_image,
            int thresh = 127,
            int maxval = 255,
            int type = 0
            );


};


#endif /* THEFLAME_IMAGEPROC_H */ 
