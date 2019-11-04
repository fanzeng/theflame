#include "videoproc.h"
#include <iostream>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <opencv2/opencv.hpp>

namespace fs = boost::filesystem;

int vtoi(const std::string& video_filename,
        const std::string& output_dirname,
        const std::string& output_ext,
        float resize_ratio
    ) {

    fs::path output_path = output_dirname;
    std::cout << "video_filename = " << video_filename << std::endl;
    std::cout << "resize_ratio = " << resize_ratio << std::endl;
    cv::VideoCapture capture(video_filename);
    cv::Mat frame;  
    long frame_count = 0;
    while(1)
    {
        capture >> frame;
        cv::resize(frame, frame, cv::Size(), resize_ratio, resize_ratio, cv::INTER_LANCZOS4);
        if (frame.empty())
            break;
        cv::namedWindow(video_filename, cv::WINDOW_AUTOSIZE);
        cv::imshow(video_filename, frame);
        char c = cv::waitKey(1) & 255;
        if (c == 'q' || c == 'Q') {
            std::cout << "q is pressed on the keyboard. Exit." << std::endl;
            break;
        }
        std::string output_file_basename = std::to_string(frame_count) + "." + output_ext;
        fs::path output_filename = output_dirname / fs::path(output_file_basename);
        
        cv::imwrite(output_filename.string(), frame);
        frame_count++;
    }
    cv::destroyAllWindows();
    return 0;
}