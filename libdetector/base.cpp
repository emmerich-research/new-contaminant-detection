#include "detector.hpp"

#include "base.hpp"

#include <utility>

NAMESPACE_BEGIN

namespace detector {
Detector::Detector() {}

Detector::~Detector() {}

void Detector::detect(const cv::Mat& image, cv::Mat& dest) {
  detect(std::move(image), std::move(dest));
}

cv::Mat Detector::detect(const cv::Mat& image) {
  cv::Mat dest;
  detect(std::move(image), std::move(dest));
  return dest;
}
}  // namespace detector

NAMESPACE_END
