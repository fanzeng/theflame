#include "videoproc.h"
#include <iostream>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <opencv2/opencv.hpp>
#include <iomanip>
#include <sstream>

namespace fs = boost::filesystem;

int vtoi(const std::string& video_filename,
        const std::string& output_dirname,
        const std::string& output_ext,
        int m_verbosity,
        float resize_ratio,
        int out_height,
        int out_width
    ) {

    fs::path output_path = output_dirname;
    COUT_INFO("video_filename=" << video_filename << std::endl)
    COUT_INFO("resize_ratio=" << resize_ratio << std::endl)
    COUT_INFO("out_height=" << out_height << std::endl)
    COUT_INFO("out_width=" << out_width << std::endl)

    cv::VideoCapture capture(video_filename);
    cv::Mat frame;  
    int frame_count = 0;
    while(1)
    {
        capture >> frame;
        
        // if customized resize ratio provided,
        if (resize_ratio != 1. && resize_ratio > 0) {
            cv::resize(frame, frame, cv::Size(0,0), resize_ratio, resize_ratio, cv::INTER_LANCZOS4);
        } else if (out_height > 0 && out_width > 0) {
            // customized out width and out height provided
            cv::resize(frame, frame, cv::Size(out_width ,out_height), 0, 0, cv::INTER_LANCZOS4);
        }
        
        if (frame.empty())
            break;
        cv::namedWindow(video_filename, cv::WINDOW_AUTOSIZE);
        cv::imshow(video_filename, frame);
        char c = cv::waitKey(1) & 255;
        if (c == 'q' || c == 'Q') {
            COUT_INFO("q is pressed on the keyboard. Exit." << std::endl)
            break;
        }
        std::string video_file_basename = fs::path(video_filename).stem().string();
        std::stringstream ss;
        ss << std::setw(5) << std::setfill('0') << std::fixed << frame_count;
        std::string frame_count_string = ss.str();
        std::string output_file_basename = 
                video_file_basename
                + "_" 
                + frame_count_string
                + "." + output_ext;
        fs::path output_filename = output_dirname / fs::path(output_file_basename);
        
        cv::imwrite(output_filename.string(), frame);
        frame_count++;
    }
    cv::destroyAllWindows();
    capture.release();
    return 0;
}


int itov(const std::vector<std::string>& vec_input_filename,
        const std::string& output_dirname,
        const std::string& output_ext,
        int frames_per_image,
        int frames_per_second,
        int m_verbosity,
        float resize_ratio,
        int out_height,
        int out_width,
        bool show_popup
    ) {

    fs::path output_path = output_dirname;
    COUT_INFO("resize_ratio=" << resize_ratio << std::endl)
    COUT_INFO("out_height=" << out_height << std::endl)
    COUT_INFO("out_width=" << out_width << std::endl)
    int first_regular_file_index = 0;
    while (
            first_regular_file_index < vec_input_filename.size()
            && !fs::is_regular_file(
                vec_input_filename.at(first_regular_file_index)
            )
        ) {
        first_regular_file_index++;
    }
    if (first_regular_file_index >= vec_input_filename.size()) {
        COUT_ERROR("cannot open any of the input files."
                " Please double check." << std::endl)
    }

    cv::Mat first_frame = cv::imread(
            vec_input_filename.at(first_regular_file_index)
        );
    
        
    size_t video_height = (size_t)first_frame.rows;
    size_t video_width = (size_t)first_frame.cols;

    // if customized resize ratio provided,
    if (resize_ratio != 1. && resize_ratio > 0) {
        video_height *= resize_ratio;
        video_width *= resize_ratio;
    } else if (out_height > 0 && out_width > 0) {
        // customized out width and out height provided
        video_height = out_height;
        video_width = out_width;
    }
    
    COUT_INFO("video_height=" << video_height << std::endl)
    COUT_INFO("video_width=" << video_width << std::endl)
            
    long input_frame_count = 0;
    long output_frame_count = 0;
    std::string output_file_basename = "itov_generated_video." + output_ext;
    fs::path output_filename = output_dirname / fs::path(output_file_basename);
    std::string output_filename_str = output_filename.string();
    COUT_INFO("output filename=" << output_filename_str << std::endl);
    cv::VideoWriter video(
        output_filename_str,
        cv::VideoWriter::fourcc('m','p','4','v'),
        frames_per_second,
        cv::Size(video_width, video_height)
    );
                
    for (
            std::vector<std::string>::const_iterator it = vec_input_filename.begin();
            it != vec_input_filename.end();
            it++
    ) {
        COUT_DEBUG("input file=" << *it << std::endl)
        if (!fs::is_regular_file(*it)) {
            COUT_ERROR("cannot read " << *it << ", skipping. " << std::endl)
            continue;
        }
        cv::Mat frame = cv::imread(*it);
        cv::resize(frame, frame, cv::Size(video_width,video_height), 0, 0, cv::INTER_LANCZOS4);
        if (frame.empty())
            break;

        if (show_popup) {
            cv::namedWindow("input frame", cv::WINDOW_AUTOSIZE);
            cv::imshow("input frame", frame);
            char c = cv::waitKey(1) & 255;
            if (c == 'q' || c == 'Q') {
                std::cout << "q is pressed on the keyboard. Exit." << std::endl;
                break;
            }
        }

        for(int i = 0; i < frames_per_image; i++) {
            video.write(frame);
            output_frame_count++;
        }
        input_frame_count++;
    }
    video.release();
    cv::destroyAllWindows();
    COUT_PROG("total input frames=" << input_frame_count << std::endl)
    COUT_PROG("total output frames=" << output_frame_count << std::endl)
    return 0;
}