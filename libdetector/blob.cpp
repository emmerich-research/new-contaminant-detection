#include "detector.hpp"

#include "blob.hpp"

#include <vector>

#include "util.hpp"

NAMESPACE_BEGIN

namespace detector {
BlobDetector::BlobDetector() {}

BlobDetector::~BlobDetector() {}

void BlobDetector::detect(const cv::Mat&&            image,
                          [[maybe_unused]] cv::Mat&& dest) {
  if (auto opt_source_points = util::quadrilateral<std::vector>(image)) {
    auto    source_points = *opt_source_points;
    auto    dim = util::image_dimension(source_points);
    cv::Mat warped_image = util::warp(image, source_points, dim);

    cv::Mat grayscale_warped;
    cv::cvtColor(warped_image, grayscale_warped, cv::COLOR_BGR2GRAY);

    cv::Mat shadowless_image = util::remove_shadows(grayscale_warped);
    cv::Mat thresh_binary;
    cv::threshold(shadowless_image, thresh_binary, 0, 255,
                  cv::THRESH_BINARY | cv::THRESH_OTSU);

    cv::SimpleBlobDetector::Params blob_params;
    blob_params.minArea = 5;
    blob_params.minCircularity = 0.5f;
    blob_params.minInertiaRatio = 0.1f;
    blob_params.minConvexity = 0.5;

    dest = std::move(grayscale_warped);

    cv::Ptr<cv::SimpleBlobDetector> detector =
        cv::SimpleBlobDetector::create(blob_params);
    std::vector<cv::KeyPoint> keypoints;
    detector->detect(thresh_binary, keypoints);
    cv::drawKeypoints(grayscale_warped, keypoints, grayscale_warped,
                      cv::Scalar{0, 0, 255},
                      cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
  }
}
}  // namespace detector

NAMESPACE_END
