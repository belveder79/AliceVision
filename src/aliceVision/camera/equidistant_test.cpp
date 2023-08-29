// This file is part of the AliceVision project.
// Copyright (c) 2020 AliceVision contributors.
// This Source Code Form is subject to the terms of the Mozilla Public License,
// v. 2.0. If a copy of the MPL was not distributed with this file,
// You can obtain one at https://mozilla.org/MPL/2.0/.

#include <aliceVision/camera/camera.hpp>

#define BOOST_TEST_MODULE equidistant

#include <boost/test/unit_test.hpp>
#include <boost/test/tools/floating_point_comparison.hpp>
#include <aliceVision/unitTest.hpp>

using namespace aliceVision;
using namespace aliceVision::camera;

//-----------------
// Test summary:
//-----------------
// - Create an Equidistant camera
// - Generate random point inside the image domain
// - Add and remove distortion and assert we found back the generated point
// - Check the last point in the camera & image domain
// - Assert that the tested distortion is not null (in order to ensure validity of the test)
//-----------------
BOOST_AUTO_TEST_CASE(cameraEquidistant_disto_undisto_Radial)
{
  makeRandomOperationsReproducible();

  const int w = 1000;
  const int h = 800;
  const double focal = 800.0;
  const double offsetX = 0.0;
  const double offsetY = 0.0;
  const double radius = 0.0; // defined automatically
  const double k1 = 0.3;
  const double k2 = 0.2;
  const double k3 = 0.1;

  std::shared_ptr<Distortion> distortion = std::make_shared<DistortionRadialK3PT>(k1, k2, k3);

  std::shared_ptr<Equidistant> cam = std::make_shared<Equidistant>(w, h, focal, offsetX, offsetY, radius, distortion);

  const double epsilon = 1e-4;
  for (int i = 0; i < 10; ++i)
  {
    // generate random point inside the image domain (last random to avoid 0,0)
    const Vec2 ptImage_gt = (Vec2::Random() * 800./2.) + Vec2(500,500) + Vec2::Random();
    const Vec2 ptCamera = cam->ima2cam(ptImage_gt);

    // Check that adding and removing distortion allow to recover the provided point
    EXPECT_MATRIX_NEAR(ptCamera, cam->removeDistortion(cam->addDistortion(ptCamera)), epsilon);
    EXPECT_MATRIX_NEAR(ptImage_gt, cam->cam2ima(cam->removeDistortion(cam->addDistortion(ptCamera))), epsilon);

    // Assert that distortion field is not null and it has moved the initial provided point
    BOOST_CHECK(! (cam->addDistortion(ptCamera) == cam->removeDistortion(cam->addDistortion(ptCamera))) ) ;

    // Check projection / back-projection
    const double depth_gt = std::abs(Vec2::Random()(0)) * 100.0;
    const geometry::Pose3 pose(geometry::randomPose());

    const Vec3 pt3d = cam->backproject(ptImage_gt, true, pose, depth_gt);
    const Vec2 pt2d_proj = cam->project(pose, pt3d.homogeneous(), true);

    EXPECT_MATRIX_NEAR(ptImage_gt, pt2d_proj, epsilon);
  }
}



