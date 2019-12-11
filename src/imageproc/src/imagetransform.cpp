#include "imageproc.h"
#include <iostream>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include "logging.h"

namespace fs = boost::filesystem;
namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    std::string str_verbosity = "v";
    int m_verbosity;
    std::vector<std::string> vec_input_filename;
    std::string output_path;
    std::string out_ext;
    std::string transform_type;
    std::vector<int> vec_additional_args;
    bool show_popup = false;
    float norm_factor = -1.;
    bool yes_to_overwrite = false;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "print help messages")
        ("output-path,o", po::value<std::string>(), "output path")
        ("ext,e", po::value<std::string>(), "output extension")
        (
            "verbosity,v",
            po::value(&str_verbosity)->implicit_value(""),
            "verbosity"
        )
        ("norm-factor,n", po::value<float>(&norm_factor),
            "scale intensity of result image by this factor for visualization,"
            " if negative, use cv::NORM_MINMAX to scale to 0-255 "
        )
        ("show-popup,s", po::value(&show_popup)->implicit_value(false),
        "show processed images in pop-up")
        ("yes-to-overwrite,y", po::value(&yes_to_overwrite)->implicit_value(""),
        "overwrite output image if exists")
        (
            "transform-type,t", po::value(&transform_type)->implicit_value(""),
            "specify which transform to perform:"
            "clahe, canny, dft, gaussianblur, mean, resize, stdev, threshold."
            "if not specified, only norm-factor will be applied."
        )
        (
            "additional-args,a",
            po::value<std::vector<int>>(&vec_additional_args)->multitoken(),
            "specify additional arguments used by the transform:\n"
            "clahe=>clip_limit,tile_grid_size\n"
            "canny=>threshold1,threshold2\n"
            "dft=>None\n"
            "gaussianblur=>kernel_size\n"
            "mean=>patch_size\n"
            "resize=>width,height\n"
            "stdev=>patch_size\n"
            "threshold=>thresh,maxval,type\n"
        )            
        (
            "input_filenames,i",
            po::value<std::vector < std::string >> 
            (&vec_input_filename)->required(),
            "input filenames"
        )
        ;
    po::positional_options_description p;
    p.add("input_filenames", -1);
    po::variables_map vm;

    try {
        po::store(
            po::command_line_parser(argc, argv).options(desc)
                .positional(p).run(),
            vm
        );
        po::notify(vm);
    } catch (const boost::exception& e) {
        if (!vm.count("help")) {
            std::cout << "Error parsing arguments." << std::endl;
        }
        std::cout << desc;
        exit(1);
    }

    if (vm.count("verbosity")) {
        str_verbosity += "v";
    }

    m_verbosity = str_verbosity.size();
    COUT_INFO("verbosity level = " << m_verbosity << std::endl)
    
    if (vm.count("output-path")) {
        output_path = vm["output-path"].as<std::string>();
    }

    if (vm.count("ext")) {
        out_ext = vm["ext"].as<std::string>();
    }

    if (vm.count("norm-factor")) {
        norm_factor = vm["norm-factor"].as<float>();
    }


    if (vm.count("show-popup")) {
        show_popup = true;
    }
    
    if (vm.count("yes-to-overwrite")) {
        yes_to_overwrite = true;
    }

    if (vm.count("transform-type")) {
        transform_type = vm["transform-type"].as<std::string>();
    }

    if (vm.count("additional-args")) {
        vec_additional_args = vm["additional-args"].as<std::vector<int>>();
    }
    
    COUT_PROG("total number of input images = "
            << vec_input_filename.size()
            << std::endl << std::endl
            )
            int success_count = 0;
    for (std::vector<std::string>::iterator it = vec_input_filename.begin();
            it != vec_input_filename.end();
            it++
            ) {
        std::string input_filename = *it;
        COUT_PROG("start to process image "
                << std::distance(vec_input_filename.begin(), it)
                << ":" << std::endl << std::endl)
                COUT_INFO("inputfilename=" << input_filename << std::endl)
        if (fs::is_regular_file(input_filename)) {
            imageproc ip = imageproc(
                std::vector<std::string>(1, input_filename),
                output_path,
                out_ext,
                m_verbosity,
                norm_factor,
                show_popup,
                yes_to_overwrite,
                vec_additional_args
            );

            int ret;
            if (transform_type.size() <= 0) transform_type = "nop";
            COUT_INFO("transform type=" << transform_type << std::endl);
            if (transform_type == "clahe") {
                ret = ip.imageclahe();
            } else if (transform_type == "canny") {
                ret = ip.imagecanny();
            } else if (transform_type == "dft") {
                ret = ip.imagedft();
            } else if (transform_type == "gaussianblur") {
                ret = ip.imagegaussianblur();
            } else if (transform_type == "mean") {
                ret = ip.imagemean();
            } else if (transform_type == "resize") {
                ret = ip.imageresize();
            } else if (transform_type == "stdev") {
                ret = ip.imagestdev();
            } else if (transform_type == "threshold") {
                ret = ip.imagethreshold();
            } else if (transform_type == "nop") {
                ret = ip.nop();
            } else {
                COUT_WARN(
                    "unrecognized transform_type="
                    << transform_type << std::endl
                );
            }
            
            if (ret == 0) {
                ret = ip.save_output_image();
                if (ret != 0) {
                    COUT_ERROR("Something went wrong. Stopped." << std::endl)
                    return ret;
                } else {
                    success_count++;
                }
            }
        } else {
            COUT_ERROR(
                "Input image file is not a regular file. Exit without processing."
                << std::endl
                )
        }
    }
    COUT_PROG(success_count << " images processed. " << std::endl << std::endl)
    return 0;
}

