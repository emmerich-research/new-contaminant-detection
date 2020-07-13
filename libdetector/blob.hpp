#ifndef LIB_DETECTOR_BLOB_HPP_
#define LIB_DETECTOR_BLOB_HPP_

/** @file base.hpp
 *  @brief BlobDetector implementation
 *
 * BlobDetector implementation
 */

#include <opencv4/opencv2/opencv.hpp>

#include "base.hpp"

NAMESPACE_BEGIN

namespace detector {
/**
 * @brief BlobDetector
 *
 * BlobDetector implementation class
 *
 * @author Ray Andrew
 * @date   July 2020
 */
class BlobDetector : public Detector {
 public:
  /**
   * Detect specific thing
   *
   * @param image  original image
   * @param dest   destination image buffer
   */
  virtual void detect(const cv::Mat&& image, cv::Mat&& dest) override;

 public:
  /**
   * BlobDetector constructor
   */
  BlobDetector();
  /**
   * BlobDetector destructor
   *
   * Destroy allocated resources
   */
  virtual ~BlobDetector() override;
};
}  // namespace detector

NAMESPACE_END

#endif  // LIB_DETECTOR_BLOB_HPP_
