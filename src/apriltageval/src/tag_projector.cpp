#include "tag_projector.h"
#include "tag_detector.h"
#include <boost/regex.hpp>

cv::Mat TagProjector::resize_tag(float resize_ratio) {
    cv::Size new_size((int)image_tag_original.cols*resize_ratio, (int)image_tag_original.rows*resize_ratio);
    cv::Mat image_tag_resized = cv::Mat::zeros(new_size, image_tag_original.type());
    for (int row = 0; row < image_tag_original.rows; row++) {
        for (int col = 0; col < image_tag_original.cols; col++) {
            cv::Point center((int)(resize_ratio/2 + col*resize_ratio), (int)(resize_ratio/2 + row*resize_ratio));
            cv::Rect roi((int)(center.x-resize_ratio/2), (int)(center.y- resize_ratio/2), (int)resize_ratio, (int)resize_ratio);
            cv::Scalar val = image_tag_original.at<cv::Vec3b>(cv::Point(col, row))[0];
            image_tag_resized(roi).setTo(val);
        }
    }
    return image_tag_resized;
}


cv::Mat TagProjector::add_dust(const cv::Mat& image_in, size_t num_dust, int radius_min, int radius_max) {
    cv::Mat image_out;
    image_in.copyTo(image_out);
//    time_t timer;
//    srand(time(&timer));
    for (int i = 0; i < num_dust; i++) {
        cv::Scalar dust_color;
        if (image_in.channels() == 1) {
            dust_color = 127;
        } else if (image_in.channels() == 3) {
            dust_color = cv::Scalar(127, 127, 127);
        }
        int rnd_x = rand() % image_in.cols;
        int rnd_y = rand() % image_in.rows;
        cv::Point rnd_point(rnd_x, rnd_y);
        int rnd_radius = radius_min;
        if (radius_max > radius_min) rnd_radius += rand() % (radius_max-radius_min);
        
        cv::circle(image_out, rnd_point, rnd_radius, dust_color, -1);
    }
    return image_out;

}

cv::Mat TagProjector::distort_tag(const cv::Mat& image, const cv::Mat& image_original_roi) {
    cv::Mat M = cv::getRotationMatrix2D(cv::Point2f(image.cols/2, image.rows/2),3, 1);
    cv::Point2f src[4] = { cv::Point2f(0,0), cv::Point2f(0,100), cv::Point2f(100,0), cv::Point2f(100,100)};
    int distortion_rate = 10;
    cv::Point2f dst[4] = { cv::Point2f(-distortion_rate/2+(rand()%distortion_rate),-distortion_rate/2+(rand()%distortion_rate)),
                           cv::Point2f(-distortion_rate/2+(rand()%distortion_rate),-distortion_rate/2+(rand()%distortion_rate)+100),
                           cv::Point2f(-distortion_rate/2+(rand()%distortion_rate)+100,-distortion_rate/2+(rand()%distortion_rate)), cv::Point2f(-distortion_rate/2+(rand()%distortion_rate)+100,-distortion_rate/2+(rand()%distortion_rate)+100) };

    M = cv::getPerspectiveTransform(src, dst);
    cv::Mat image_tag_distorted;
    image_original_roi.copyTo(image_tag_distorted);
    cv::warpPerspective(image, image_tag_distorted, M, image.size(), cv::INTER_LINEAR, cv::BORDER_TRANSPARENT);
    return image_tag_distorted;
}

std::string TagProjector::project_tag(std::string filename_image_orignal, std::string tag_type, std::string tag_label, std::string out_path) {
    resize_ratio = 10;
    boost::filesystem::path path_tag_type = boost::filesystem::path(tag_type);
    boost::filesystem::path path_tag_label = boost::filesystem::path(tag_label);
    boost::filesystem::path path_image_tag_original = path_tag_path / path_tag_type / path_tag_label;
    image_tag_original = cv::imread(path_image_tag_original.string() + ".png");
    assert(image_tag_original.cols > 0 && image_tag_original.rows > 0);
    cv::Mat image_tag_resized = resize_tag(resize_ratio);
    image_original = cv::imread(filename_image_orignal);

    cv::Rect roi = cv::Rect(image_original.cols/2, image_original.rows/2, image_tag_resized.cols, image_tag_resized.rows);
    if (do_distort_tag) {
        image_tag_resized = distort_tag(image_tag_resized, image_original(roi));
    }   

    if (do_add_dust) {
        int radius_min = 1;
        int radius_max_size_factor = 40;
        int radius_max = (int)((image_tag_resized.rows + image_tag_resized.cols)/2/radius_max_size_factor);
        size_t num_dust = 2;
        image_tag_resized = add_dust(image_tag_resized, num_dust, radius_min, radius_max);
    }
    cv::Mat image_tag_to_project = image_tag_resized;
    cv::Mat image_projected = image_original;
    image_tag_to_project.copyTo(image_projected(roi));
    boost::filesystem::path path_out_path = boost::filesystem::path(out_path);
    boost::filesystem::path basename_image_out = boost::filesystem::path(filename_image_orignal).filename().stem();
    boost::filesystem::path path_filename_no_ext = path_out_path / basename_image_out;
    std::string filename_image_out = path_filename_no_ext.string() + ".pnm";
    bool success_imwrite = cv::imwrite(filename_image_out, image_projected);
    if (success_imwrite) {
        std::cout << "imwrite success." << std::endl;
    } else {
        std::cout << "imwrite failed." << std::endl;
    }
    return filename_image_out;
}

std::vector<std::string> TagProjector::project_tag_for_dir(std::string dirname_image_orignal, 
        std::string string_regex, std::string tag_type, std::vector<std::string> vec_tag_label, std::string out_path) {
    const boost::regex mosaic_test_image_filter(string_regex, boost::regex::extended);
    boost::filesystem::path path_dirname_image_original(dirname_image_orignal);
    boost::filesystem::directory_iterator it_end;
    std::vector<std::string> vec_filename_image_out;
    size_t i = 0;
    for( boost::filesystem::directory_iterator it(path_dirname_image_original); it != it_end; it++ )
    {
        if( !boost::filesystem::is_regular_file( it->status() ) ) continue;
        boost::smatch what;
        std::string filename = it->path().filename().string();
        if( !boost::regex_match(filename, what, mosaic_test_image_filter ) ) continue;
        if (i >= vec_tag_label.size()) {
            break;
        } else {
            std::cout << "i = " << i << ", " << it->path().string() << "," << vec_tag_label.at(i) << std::endl;
        }
        std::string tag_label = vec_tag_label.at(i);
        std::string filename_image_out = project_tag(it->path().string(), tag_type, tag_label, out_path);
        vec_filename_image_out.emplace_back(filename_image_out);
        i++;
    }
    return vec_filename_image_out;
}