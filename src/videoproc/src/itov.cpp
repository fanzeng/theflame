#include "videoproc.h"
#include <iostream>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/program_options.hpp>
#include <boost/exception/all.hpp>

namespace fs = boost::filesystem;
namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    std::string str_verbosity;
    int m_verbosity;
    std::vector<std::string> vec_input_filename;
    std::string out_path;
    std::string out_ext = "mp4"; // default output format
    float resize_ratio = 1.;
    int out_height;
    int out_width;
    int frames_per_image = 1;
    int frames_per_second = 24;
    bool show_popup = false;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "pring help messages")
        ("path,p", po::value<std::string>(), "output path")
        ("ext,e", po::value<std::string>(), "output extension")
        ("frames-per-image,d", po::value<int>(), "number of frames per image")
        ("frames-per-second,f", po::value<int>(), "number of frames per second")
        ("resize,r", po::value<float>(), "resize ratio")
        ("out_height,g", po::value<int>(), "out height")
        ("out_width,w", po::value<int>(), "out width")
        ("show-popup,s", po::value(&show_popup)->implicit_value(false),
        "show processed images in pop-up")
        ("verbose,v", po::value(&str_verbosity)->implicit_value(""), "verbosity")
        (
            "input_filenames,i",
            po::value<std::vector<std::string>> 
            (&vec_input_filename)->required(),
            "input filenames"
        )
        ;
    po::positional_options_description p;
    p.add("input_filenames", -1);
    po::variables_map vm;

    try {
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        po::notify(vm);
    } catch (const boost::exception& e) {
        if (!vm.count("help")) std::cout << "Error parsing arguments." << std::endl;
        std::cout << desc;
        exit(1);
    }

    if (vm.count("resize")) {
        resize_ratio = vm["resize"].as<float>();
    }
    

    if (vm.count("verbosity")) {
        str_verbosity += "v";
    }
    m_verbosity = str_verbosity.size();
    
    if (vm.count("out_height")) {
        out_height = vm["out_height"].as<int>();
        if ( ! vm.count("out_width")) {
            COUT_WARN(
                "out_height is set but out_width is not."
                " out_height will be ignored." << std::endl
            )
        }
    }
    
    if (vm.count("out_width")) {
        out_width = vm["out_width"].as<int>();
        if ( ! vm.count("out_height")) {
            COUT_WARN(
                "out_width is set but out_height is not."
                " out_width will be ignored." << std::endl
            )
        }
    }
    
    if (vm.count("path")) {
        out_path = vm["path"].as<std::string>();
        COUT_INFO("output path=" << out_path << std::endl)
    }

    if (vm.count("ext")) {
        out_ext = vm["ext"].as<std::string>();
        COUT_INFO("output extension=" << out_ext << std::endl)
    }
    
    if (vm.count("frames-per-image")) {
        frames_per_image = vm["frames-per-image"].as<int>();
        COUT_INFO("frames-per-image=" << frames_per_image << std::endl)
    }
    
    if (vm.count("frames-per-second")) {
        frames_per_second = vm["frames-per-second"].as<int>();
        COUT_INFO("frames-per-second=" << frames_per_second << std::endl)
    }
    if (vm.count("show-popup")) {
        show_popup = true;
        COUT_INFO("show-popup=" << (show_popup ? "true":"false") << std::endl)
    }
    itov(
        vec_input_filename,
        out_path,
        out_ext,
        frames_per_image,
        frames_per_second,
        m_verbosity,
        resize_ratio,
        out_height,
        out_width,
        show_popup
    );
    return 0;
}

