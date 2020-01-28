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
    std::string input_filename;
    std::string out_path;
    std::string out_ext = "png";
    float resize_ratio = 1.;
    int out_height;
    int out_width;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "print help messages")
        ("path,p", po::value<std::string>(), "output path")
        ("ext,e", po::value<std::string>(), "output extension")
        ("resize,r", po::value<float>(), "resize ratio, overwrites g&w")
        ("out_height,g", po::value<int>(), "out height")
        ("out_width,w", po::value<int>(), "out width")
        ("verbose,v", po::value(&str_verbosity)->implicit_value(""), "verbosity")
        ("input-filename,i", po::value<std::string>(&input_filename)->required(), "input filename")
        ;
    po::positional_options_description p;
    p.add("input-filename", 1);
    po::variables_map vm;

    try {
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
        po::notify(vm);
    } catch (const boost::exception& e) {
        if (!vm.count("help")) std::cout << "Error parsing arguments." << std::endl;
        std::cout << desc;
        exit(1);
    }

    if (vm.count("verbosity")) {
        str_verbosity += "v";
    }

    m_verbosity = str_verbosity.size();
    
    if (vm.count("resize")) {
        resize_ratio = vm["resize"].as<float>();
    }

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
    }
    COUT_INFO("output path=" << out_path << std::endl)

    if (vm.count("ext")) {
        out_ext = vm["ext"].as<std::string>();
    }
    COUT_INFO("output extension=" << out_ext << std::endl)

    std::cout << "input filename=" << input_filename << std::endl;
    if (fs::is_regular_file(input_filename)) {
        vtoi(
            input_filename,
            out_path,
            out_ext,
            m_verbosity,
            resize_ratio,
            out_height,
            out_width
        );
        return 0;
    } else {
        std::cout
            << "Input video file is not a regular file. Exit without processing."
            << std::endl;
    }
}

