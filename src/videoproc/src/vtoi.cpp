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
    std::string input_filename;
    std::string out_path;
    std::string out_ext;
    float resize_ratio = 1.;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "pring help messages")
        ("path,p", po::value<std::string>(), "output path")
        ("ext,e", po::value<std::string>(), "output extension")
        ("resize,r", po::value<float>(), "resize ratio")
        ("verbose,v", po::value(&str_verbosity)->implicit_value(""), "verbosity")
        ("input_filename", po::value<std::string>(&input_filename)->required(), "input_filename")
        ;
    po::positional_options_description p;
    p.add("input_filename", 1);
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

    if (vm.count("path")) {
        out_path = vm["path"].as<std::string>();
        std::cout << "output path=" << out_path << std::endl;
    }

    if (vm.count("ext")) {
        out_ext = vm["ext"].as<std::string>();
        std::cout << "output extension=" << out_ext << std::endl;
    }
    std::cout << "inputfilename: " << input_filename << std::endl;
    if (fs::is_regular_file(input_filename)) {
        if (resize_ratio != 1.) {
            vtoi(input_filename, out_path, out_ext, resize_ratio);
        } else {
            vtoi(input_filename, out_path, out_ext);
        }
        return 0;
    } else {
        std::cout
            << "Input video file is not a regular file. Exit without processing."
            << std::endl;
    }
}

