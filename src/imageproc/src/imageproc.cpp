#include "imageproc.h"
#include <iostream>

namespace fs = boost::filesystem;

cv::Mat imageproc::normalize_intensity(const cv::Mat before) {
    cv::Mat after;
    if (m_norm_factor < 0.) {
        COUT_DEBUG("no norm-factor specified. "
            "Will normalize to 0-255 using NORM_MINMAX"
            << std::endl
        )
        cv::normalize(before, after, 0, 255, cv::NORM_MINMAX);
    } else {
        COUT_DEBUG("norm-factor=" << m_norm_factor
            << ", will scale the output image intensity by it."
            << std::endl
        )        
        cv::convertScaleAbs(before, after, m_norm_factor);
    }
    return after;
}

int imageproc::convert_input_image_to_gray() {
    cv::cvtColor(m_input_image, m_input_image, cv::COLOR_BGR2GRAY);
}

int imageproc::save_output_image() {
    if (!m_yes_to_overwrite && fs::exists(m_output_filename)) {
        COUT_ERROR ("The output file already exists." << std::endl
                << "Stopped before modifying it." << std::endl
                << "Please set yes-to-overwrite flag to overwrite." << std::endl
                << std::endl
        )
        return 1;
    } else {
        COUT_INFO("output_filename=" << m_output_filename << std::endl);
        if (m_output_image.empty()) {
            COUT_WARN("The output image is empty." << std::endl)
            return 1;
        }
        if(cv::imwrite(m_output_filename, m_output_image)) {
            return 0;
        } else {
            COUT_ERROR("Failed to save output image." << std::endl)
            return 1;
        }
    }
}

// No normalization is performed.
// Result could be an image that is difficult to view
// It's up to the calling function to decide whether to normalize.

cv::Mat imageproc::cvdft(cv::Mat I) {
    cv::Mat padded;
    int m = cv::getOptimalDFTSize( I.rows );
    int n = cv::getOptimalDFTSize( I.cols );
    cv::copyMakeBorder(
        I,
        padded,
        0,
        m - I.rows,
        0,
        n - I.cols,
        cv::BORDER_CONSTANT,
        cv::Scalar::all(0)
    );

    cv::Mat planes[] = {
        cv::Mat_<float>(padded),
        cv::Mat::zeros(padded.size(),
        CV_32F)
    };
    cv::Mat complexI;
    cv::merge(planes, 2, complexI);
    cv::dft(complexI, complexI);

    cv::split(complexI, planes); // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
    cv::magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
    cv::Mat magI = planes[0];


    // crop the spectrum, if it has an odd number of rows or columns
    magI = magI(cv::Rect(0, 0, magI.cols & -2, magI.rows & -2));

    // rearrange the quadrants of Fourier image  so that the origin is at the image center
    int cx = magI.cols/2;
    int cy = magI.rows/2;

    cv::Mat q0(magI, cv::Rect(0, 0, cx, cy)); 
    cv::Mat q1(magI, cv::Rect(cx, 0, cx, cy)); 
    cv::Mat q2(magI, cv::Rect(0, cy, cx, cy));  
    cv::Mat q3(magI, cv::Rect(cx, cy, cx, cy));

    cv::Mat tmp;                           
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);                    
    q2.copyTo(q1);
    tmp.copyTo(q2);

    return magI;
}


cv::Mat imageproc::image_dft(const cv::Mat& input_image) {
    if (m_show_popup) {
        cv::namedWindow(m_input_image_filename, cv::WINDOW_NORMAL);
        cv::imshow(m_input_image_filename, input_image);
    }

    COUT_DEBUG("input_image.rows=" << input_image.rows << std::endl)
    COUT_DEBUG("input_image.cols=" << input_image.cols << std::endl)

    cv::Mat image_dft = cvdft(input_image);

    if (m_norm_factor < 0.) {
        COUT_DEBUG("no norm-factor specified. "
            "Will normalize to 0-255 using NORM_MINMAX"
            << std::endl
        )
        cv::normalize(image_dft, image_dft, 0, 255, cv::NORM_MINMAX);
    } else {
        COUT_DEBUG("norm-factor=" << m_norm_factor
            << ", will scale the output image intensity by it."
            << std::endl
        )        
        cv::convertScaleAbs(image_dft, image_dft, m_norm_factor);
    }

    if (m_show_popup) {
        cv::resizeWindow("image_proc", image_dft.cols, image_dft.rows); 
        cv::imshow("image_proc", image_dft);  
        cv::waitKey(1);
    }
    
    image_dft += cv::Scalar::all(1); // switch to logarithmic scale
    cv::log(image_dft, image_dft);
    cv::Mat output_image;
    image_dft = image_dft*255;
    image_dft.convertTo(output_image, CV_8SC1);
    return image_dft;
}


cv::Mat imageproc::image_meanstd(
    const cv::Mat& input_image,
    std::string output_option,
    int patch_size
) {
    COUT_DEBUG("input_image.rows=" << input_image.rows << std::endl)
    COUT_DEBUG("input_image.cols=" << input_image.cols << std::endl)
    COUT_DEBUG("patch_size=" << patch_size << std::endl)
    int patch_size_half = patch_size / 2;
    int step_size = 1;

    int width_result = ceil(input_image.cols/step_size);
    int height_result = ceil(input_image.rows/step_size);
    COUT_DEBUG("output image size (wxh)="
            << width_result
            << "x" 
            << height_result 
            << std::endl
    )
    cv::Mat image_mean = cv::Mat::zeros(
        cv::Size(width_result, height_result),
        CV_64FC1
    );
    cv::Mat image_stdev = cv::Mat::zeros(
        cv::Size(width_result, height_result),
        CV_64FC1
    );

    for (
            int r = patch_size_half;
            r+patch_size_half < input_image.rows;
            r+=step_size
        ) {
        for (
                int c = patch_size_half;
                c+patch_size_half < input_image.cols;
                c+=step_size
            ) {
            cv::Mat patch = input_image(
                cv::Rect(
                    c - patch_size_half, r - patch_size_half,
                    patch_size, patch_size
            ));
            cv::Scalar mean, stdev;
            cv::meanStdDev(patch, mean, stdev);
            image_mean.at<double>(r/step_size, c/step_size) = mean[0]/255;
            image_stdev.at<double>(r/step_size, c/step_size) = stdev[0]/255;
        }
    }
    cv::Mat output_image;
    std::string output_window_name = output_option;
    if (output_option == "mean") {
        output_image = image_mean;
    } else if (output_option == "stdev") {
        output_image = image_stdev;
    }

    if (m_show_popup) {
        cv::resizeWindow("image_proc", output_image.cols, output_image.rows); 
        cv::imshow("image_proc", output_image);  
        cv::waitKey(1);
    }
    
    output_image = normalize_intensity(output_image);
    return output_image;
}


cv::Mat imageproc::image_clahe(
    const cv::Mat& input_image,
    int clip_limit,
    int tile_grid_size
) {
    assert(input_image.cols > 0 && input_image.rows > 0);
    COUT_DEBUG("clip_limit=" << clip_limit << std::endl)
    COUT_DEBUG("tile_grid_size=" << tile_grid_size << std::endl)
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(
            clip_limit,
            cv::Size(tile_grid_size, tile_grid_size)
    );
    cv::Mat clahe_image;
    clahe->apply(input_image, clahe_image);
    if (m_show_popup) {
        cv::resizeWindow("image_proc", clahe_image.cols, clahe_image.rows); 
        cv::imshow("image_proc", clahe_image);  
        cv::waitKey(1);
    }
    return clahe_image;
}

cv::Mat imageproc::image_canny(
    const cv::Mat& input_image,
    int threshold1,
    int threshold2
) {
    cv::Mat output_image;
    COUT_DEBUG("threshold1=" << threshold1 << std::endl)
    COUT_DEBUG("threshold2=" << threshold2 << std::endl)
    cv::Canny(input_image, output_image, threshold1, threshold2);
    if (m_show_popup) {
        cv::resizeWindow("image_proc", output_image.cols, output_image.rows); 
        cv::imshow("image_proc", output_image);  
        cv::waitKey(1);
    }
    return output_image;
}

cv::Mat imageproc::image_gaussianblur(const cv::Mat input_image, int kernel_size) {
    cv::Mat output_image;

    COUT_DEBUG("kernel_size=" << kernel_size << std::endl)
    if (kernel_size <= 0 || kernel_size %2 != 1) {
        COUT_ERROR("invalid kernel size." << std::endl)
    }
    cv::GaussianBlur(
        input_image,
        output_image,
        cv::Size(kernel_size, kernel_size),
        0
    );
    
    if (m_show_popup) {
        cv::resizeWindow("image_proc", output_image.cols, output_image.rows); 
        cv::imshow("image_proc", output_image);  
        cv::waitKey(1);
    }
    return output_image;
}

cv::Mat imageproc::image_resize(const cv::Mat input_image, int width, int height) {
    cv::Mat output_image;
    
    COUT_DEBUG("width=" << width << std::endl)
    COUT_DEBUG("height=" << height << std::endl)
            
    cv::resize(
        input_image,
        output_image,
        cv::Size(width, height)
    );
    
    if (m_show_popup) {
        cv::resizeWindow("image_proc", output_image.cols, output_image.rows); 
        cv::imshow("image_proc", output_image);  
        cv::waitKey(1);
    }
    
    return output_image;
}

cv::Mat imageproc::image_threshold(
    const cv::Mat input_image,
    int thresh,
    int maxval,
    int type
) {
    cv::Mat output_image;

    COUT_DEBUG("thresh=" << thresh << std::endl)
    COUT_DEBUG("maxval=" << maxval << std::endl)
    COUT_DEBUG("type=" << type << std::endl)

    cv::threshold(input_image, output_image, thresh, maxval, type);
    if (m_show_popup) {
        cv::resizeWindow("image_proc", output_image.cols, output_image.rows); 
        cv::imshow("image_proc", output_image);  
        cv::waitKey(1);
    }
    return output_image;
}

cv::Mat imageproc::image_erode(
    const cv::Mat input_image,
    int erosion_size,
    int erosion_type
) {
    cv::Mat output_image;

    COUT_DEBUG("erosion_size=" << erosion_size << std::endl)
    COUT_DEBUG("erosion_type=" << erosion_type << std::endl)

    cv::Mat kernel = cv::getStructuringElement(
                erosion_type,
                cv::Size(2*erosion_size + 1, 2*erosion_size + 1)
            );
    cv::erode(input_image, output_image, kernel);
    if (m_show_popup) {
        cv::resizeWindow("image_proc", output_image.cols, output_image.rows); 
        cv::imshow("image_proc", output_image);  
        cv::waitKey(1);
    }
    return output_image;
}

cv::Mat imageproc::image_dilate(
    const cv::Mat input_image,
    int dilation_size,
    int dilation_type
) {
    cv::Mat output_image;

    COUT_DEBUG("dilation_size=" << dilation_size << std::endl)
    COUT_DEBUG("dilation_type=" << dilation_type << std::endl)

    cv::Mat kernel = cv::getStructuringElement(
                dilation_type,
                cv::Size(2*dilation_size + 1, 2*dilation_size + 1)
            );
    cv::dilate(input_image, output_image, kernel);
    if (m_show_popup) {
        cv::resizeWindow("image_proc", output_image.cols, output_image.rows); 
        cv::imshow("image_proc", output_image);  
        cv::waitKey(1);
    }
    return output_image;
}

cv::Mat imageproc::image_absdiff(const cv::Mat& input_image_0, const cv::Mat input_image_1) {
    cv::Mat input_image_0_gray, input_image_1_gray, output_image;
    if (input_image_0.channels() == 1) {
        input_image_0_gray = input_image_0;
    } else {
        cv::cvtColor(input_image_0, input_image_0_gray, cv::COLOR_BGR2GRAY);
    }
    if (input_image_1.channels() == 1) {
        input_image_1_gray = input_image_1;
    } else {
        cv::cvtColor(input_image_1, input_image_1_gray, cv::COLOR_BGR2GRAY);
    }    
    cv::absdiff(input_image_0, input_image_1, output_image);
    if (m_show_popup) {
        cv::resizeWindow("image_proc", output_image.cols, output_image.rows); 
        cv::imshow("image_proc", output_image);  
        cv::waitKey(1);
    } 
    return output_image;
}
