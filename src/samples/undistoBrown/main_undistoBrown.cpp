// This file is part of the AliceVision project.
// Copyright (c) 2017 AliceVision contributors.
// Copyright (c) 2013 openMVG contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include <aliceVision/cmdline/cmdline.hpp>
#include <aliceVision/image/all.hpp>
#include <aliceVision/camera/camera.hpp>
#include <aliceVision/system/ProgressDisplay.hpp>

#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <string>
#include <iostream>

// These constants define the current software version.
// They must be updated when the command line is changed.
#define ALICEVISION_SOFTWARE_VERSION_MAJOR 1
#define ALICEVISION_SOFTWARE_VERSION_MINOR 0

using namespace aliceVision;
using namespace aliceVision::camera;
using namespace aliceVision::image;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char **argv)
{
    std::string inputImagePath;
    std::string outputImagePath;
    // Temp storage for the Brown's distortion model
    Vec2 c; // distortion center
    Vec3 k; // distortion factors
    double f; // Focal
    std::string suffix = "jpg";

    po::options_description requiredParams("Required parameters");
    requiredParams.add_options()
        ("input,i", po::value<std::string>(&inputImagePath)->required(),
         "An image.")
        ("output,o", po::value<std::string>(&outputImagePath)->required(),
         "An image.")
        ("cx", po::value<double>(&c(0))->required(),
         "Distortion center (x).")
        ("cy", po::value<double>(&c(1))->required(),
         "Distortion center (y).")
        ("k1", po::value<double>(&k(0))->required(),
         "Distortion factors (1).")
        ("k2", po::value<double>(&k(1))->required(),
         "Distortion factors (2).")
        ("k3", po::value<double>(&k(2))->required(),
         "Distortion factors (3).")
        ("focal", po::value<double>(&f)->required(),
         "Focal length.");
        
    po::options_description optionalParams("Optional parameters");
    optionalParams.add_options()
        ("suffix", po::value<std::string>(&suffix)->default_value(suffix),
         "Suffix of the input files.");

    aliceVision::CmdLine cmdline("AliceVision Sample undistoBrown");
    cmdline.add(requiredParams);
    cmdline.add(optionalParams);

    if(!cmdline.execute(argc, argv))
    {
        return EXIT_FAILURE;
    }

    if (outputImagePath == inputImagePath)
    {
        std::cerr << "Input and Ouput path are set to the same value" << std::endl;
        return EXIT_FAILURE;
    }

    if (!fs::exists(outputImagePath))
        fs::create_directory(outputImagePath);

    std::cout << "Used Brown's distortion model values: \n"
              << "  Distortion center: " << c.transpose() << "\n"
              << "  Distortion coefficients (K1,K2,K3): "
              << k.transpose() << "\n"
              << "  Distortion focal: " << f << std::endl;

    const boost::regex filter(".*."+suffix);

    std::vector<std::string> vec_fileNames;

    boost::filesystem::directory_iterator endItr;
    for(boost::filesystem::directory_iterator i(inputImagePath); i != endItr; ++i)
    {
        if(!boost::filesystem::is_regular_file(i->status()))
            continue;

        boost::smatch what;

        if(!boost::regex_match(i->path().filename().string(), what, filter))
            continue;

        vec_fileNames.push_back(i->path().filename().string());
    }

    std::cout << "\nLocated " << vec_fileNames.size() << " files in " << inputImagePath
              << " with suffix " << suffix;

    auto progressDisplay = system::createConsoleProgressDisplay(vec_fileNames.size(), std::cout);
    for (size_t j = 0; j < vec_fileNames.size(); ++j, ++progressDisplay)
    {
        const std::string inFileName = (fs::path(inputImagePath) / fs::path(vec_fileNames[j]).filename()).string();
        const std::string outFileName = (fs::path(outputImagePath) / fs::path(vec_fileNames[j]).filename()).string();

        Image<RGBColor> image, imageUd;
        readImage(inFileName, image, image::EImageColorSpace::NO_CONVERSION);

        std::shared_ptr<Distortion> distortion = std::make_shared<DistortionRadialK3>(k(0), k(1), k(2));

        std::shared_ptr<Pinhole> cam = std::make_shared<Pinhole>(image.Width(), image.Height(), f, f, c(0), c(1), distortion);

        UndistortImage(image, cam.get(), imageUd, BLACK);
        writeImage(outFileName, imageUd,
                   image::ImageWriteOptions().toColorSpace(image::EImageColorSpace::NO_CONVERSION));

    } //end loop for each file
    return EXIT_SUCCESS;
}

