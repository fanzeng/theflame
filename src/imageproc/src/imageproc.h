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
    m_output_ext(output_ext),
    m_norm_factor(norm_factor),
    m_verbosity(verbosity),
    m_vec_additional_args(vec_additional_args) {
        m_input_image_filename = m_vec_filename.at(0);
        if (output_ext.length() <= 0) {
            m_output_dirname = fs::path(m_input_image_filename).parent_path().string();
        } else {
            m_output_dirname = output_dirname;
        }
        fs::path output_path = m_output_dirname;
        COUT_INFO("image_filename=" << m_input_image_filename << std::endl)
        cv::VideoCapture capture(m_input_image_filename);
        m_input_image = cv::imread(m_input_image_filename, cv::IMREAD_COLOR);
        m_show_popup = show_popup;
        std::string str_out_filename = fs::path(m_input_image_filename).stem().string() + "." + m_output_ext;
        COUT_INFO("out file basename=" << str_out_filename << std::endl)
        m_output_filename = (output_path / fs::path(str_out_filename)).string();
        COUT_INFO("out file name=" << m_output_filename << std::endl)
        COUT_INFO("vec_additional_args.size()=" << m_vec_additional_args.size() << std::endl)

    }

    int nop() {
        m_output_image = normalize_intensity(m_input_image);
        return 0;
    }

    int imagethreshold() {
        convert_input_image_to_gray();
        if (m_vec_additional_args.size() > 2) {
           m_output_image = threshold(
                m_input_image,
                m_vec_additional_args.at(0),
                m_vec_additional_args.at(1),
                m_vec_additional_args.at(2)
            );
        } else if (m_vec_additional_args.size() > 1) {
           m_output_image = threshold(
                m_input_image,
                m_vec_additional_args.at(0),
                m_vec_additional_args.at(1)
            );
        } else if (m_vec_additional_args.size() > 0) {
           m_output_image = threshold(
                m_input_image,
                m_vec_additional_args.at(0)
            );
        } else {
            m_output_image = threshold(m_input_image);
        }
        return 0;
    }
    
    cv::Mat threshold(
        const cv::Mat input_image,
        int thresh = 127,
        int maxval = 255,
        int type = 0
    );
    
    int imageresize() {
        if (m_vec_additional_args.size() > 1) {
            m_output_image = imageresize(
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
    
    cv::Mat imageresize(const cv::Mat input_image, int width, int height);

    int imagegaussianblur() {
        convert_input_image_to_gray();
        if (m_vec_additional_args.size() > 0) {
            m_output_image = imagegaussianblur(
                m_input_image,
                m_vec_additional_args.at(0)
            );
        } else {
            m_output_image = imagegaussianblur(m_input_image);
        }
          return 0;
    }

    cv::Mat imagegaussianblur(const cv::Mat input_image, int kernel_size=3);
    
    int imagedft() {
        convert_input_image_to_gray();
        m_output_image = imagedft(m_input_image);
        return 0;
    }

    cv::Mat imagedft(const cv::Mat& input_image);

    int imagemean() {
        convert_input_image_to_gray();
        m_output_image = imagemean(m_input_image);
        return 0;
    }

    cv::Mat imagemean(const cv::Mat& input_image) {
        if (m_vec_additional_args.size() > 0) {
            return imagemeanstd(
                input_image,
                "mean",
                m_vec_additional_args.at(0)
            );
        } else {
            return imagemeanstd(input_image, "mean");
        }
    }

    int imagestdev() {
        convert_input_image_to_gray();
        m_output_image = imagestdev(m_input_image);
        return 0;
    }

    cv::Mat imagestdev(const cv::Mat& input_image) {
        if (m_vec_additional_args.size() > 0) {
            return imagemeanstd(
                input_image,
                "stdev",
                m_vec_additional_args.at(0)
            );
        } else {
            return imagemeanstd(input_image, "stdev");
        }    }

    int imageclahe() {
        convert_input_image_to_gray();
        if (m_vec_additional_args.size() > 1) {
           m_output_image = clahe(
                m_input_image,
                m_vec_additional_args.at(0),
                m_vec_additional_args.at(1)
            );
        } else if (m_vec_additional_args.size() > 0) {
           m_output_image = clahe(
                m_input_image,
                m_vec_additional_args.at(0)
            );
        } else {
            m_output_image = clahe(m_input_image);
        }
        return 0;
    }
    
    cv::Mat clahe(const cv::Mat& input_image, int clip_limit=40, int tile_grid_size=8);

    int imagecanny() {
        convert_input_image_to_gray();
        m_output_image = canny(m_input_image);
        return 0;
    }

    cv::Mat canny(const cv::Mat& input_image, int threshold1=10, int threshold2=350);
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

    int convert_input_image_to_gray();
    cv::Mat normalize_intensity(const cv::Mat before);
    cv::Mat dft(cv::Mat I);
    cv::Mat imagemeanstd(const cv::Mat& input_image, std::string output_option, int patch_size=17);

};


#endif /* THEFLAME_IMAGEPROC_H */ 
