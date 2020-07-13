#ifndef LIB_DETECTOR_BASE_HPP_
#define LIB_DETECTOR_BASE_HPP_

/** @file base.hpp
 *  @brief Detector base
 *
 * Detector base
 */

#include <opencv4/opencv2/opencv.hpp>

#include <libcore/core.hpp>

NAMESPACE_BEGIN

namespace detector {
/**
 * @brief Detector base
 *
 * Detector base class
 *
 * @author Ray Andrew
 * @date   July 2020
 */
class Detector : public StackObj {
 public:
  /**
   * Detect specific thing
   *
   * @param image  original image
   * @param dest   destination image buffer
   */
  virtual void detect(const cv::Mat&& image, cv::Mat&& dest) = 0;
  /**
   * Detect specific thing
   *
   * @param image  original image
   * @param dest   destination image buffer
   */
  void detect(const cv::Mat& image, cv::Mat& dest);
  /**
   * Detect specific things
   *
   * @param image  original image
   *
   * @return modified image buffer
   */
  cv::Mat detect(const cv::Mat& image);

 protected:
  /**
   * Detector constructor
   */
  Detector();
  /**
   * Detector destructor
   *
   * Destroy allocated resources
   */
  virtual ~Detector();
};
}  // namespace detector

NAMESPACE_END

#endif  // LIB_DETECTOR_BASE_HPP_
