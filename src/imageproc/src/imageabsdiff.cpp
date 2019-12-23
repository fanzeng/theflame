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
    std::string input_dirname_0, input_dirname_1;
    std::string output_path;
    std::string out_ext;

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
            "input_dir_0,i",
            po::value<std::string>(&input_dirname_0)->required(),
            "input directory 0"
        )
        (
            "input_dir_1,j",
            po::value<std::string>(&input_dirname_1)->required(),
            "input directory 1"
        );

    po::positional_options_description p;
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
    COUT_INFO("verbosity level=" << m_verbosity << std::endl)
    
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
    
    auto input_path_0 = fs::path(input_dirname_0);
    auto input_path_1 = fs::path(input_dirname_1);

    fs::directory_iterator it_end;
    std::vector<std::string> vec_input_filename_0, vec_input_filename_1;
    for (fs::directory_iterator it(input_path_0); it != it_end; it++) {
        if(! fs::is_regular_file((it->status()))) continue;
        vec_input_filename_0.push_back(it->path().string());
    }
    for (fs::directory_iterator it(input_path_1); it != it_end; it++) {
        if(! fs::is_regular_file((it->status()))) continue;
        vec_input_filename_1.push_back(it->path().string());
    }
    std::sort(vec_input_filename_0.begin(), vec_input_filename_0.end());
    std::sort(vec_input_filename_1.begin(), vec_input_filename_1.end());

    COUT_PROG("total number of images in input_dir_0="
            << vec_input_filename_0.size()
            << std::endl << std::endl
    )
    COUT_PROG("total number of images in input_dir_1="
            << vec_input_filename_1.size()
            << std::endl << std::endl
    )
    assert(vec_input_filename_0.size() <= vec_input_filename_1.size());
    int success_count = 0;
    std::vector<std::string>::iterator it_0 = vec_input_filename_0.begin();
    std::vector<std::string>::iterator it_1 = vec_input_filename_1.begin();
    for (;
            it_0 != vec_input_filename_0.end()
                && it_1 != vec_input_filename_1.end();
            it_0++, it_1++
        ) {
        
        std::vector<std::string> vec_input_filename_pair;
        std::string input_filename_0 = *it_0;
        std::string input_filename_1 = *it_1;

        COUT_PROG(
            std::endl << std::endl
            << "start to process image "
            << std::distance(vec_input_filename_0.begin(), it_0)
            << ":" << std::endl << std::endl
        )
        COUT_INFO("inputfilename_0=" << input_filename_0 << std::endl)
        COUT_INFO("inputfilename_1=" << input_filename_1 << std::endl)              
        vec_input_filename_pair.push_back(input_filename_0);
        vec_input_filename_pair.push_back(input_filename_1);

        if (true) {
            imageproc ip = imageproc(
                vec_input_filename_pair,
                output_path,
                out_ext,
                m_verbosity,
                norm_factor,
                show_popup,
                yes_to_overwrite,
                std::vector<int>()
            );

            int ret;
            ret = ip.image_absdiff();
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

