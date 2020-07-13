#ifndef LIB_DETECTOR_UTIL_HPP_
#define LIB_DETECTOR_UTIL_HPP_

/** @file base.hpp
 *  @brief Detector implementation
 *
 * BlobDetector implementation
 */

#include <optional>
#include <tuple>

#include <opencv4/opencv2/opencv.hpp>

#include <libcore/core.hpp>

NAMESPACE_BEGIN

namespace detector {
typedef std::tuple<int, int> dimension;

namespace util {
/**
 * CLAHE (Contrast Limited Adaptive Histogram Equalization)
 *
 * https://stackoverflow.com/questions/24341114/simple-illumination-correction-in-images-opencv-c
 * https://docs.opencv.org/master/d6/dc7/group__imgproc__hist.html#gad689d2607b7b3889453804f414ab1018
 *
 * @param image          image to apply CLAHE
 * @param clip_limit     clip limit
 * @param tile_grid_size tile grid size
 *
 * @return new image after applying CLAHE
 */
cv::Mat apply_CLAHE(const cv::Mat&  image,
                    double          clip_limit = 4.0,
                    const cv::Size& tile_grid_size = cv::Size(8, 8));

/** Calculate params
 *
 * Taken from :
 * https://stackoverflow.com/questions/44020751/how-to-detect-rectangle-from-houghlines-transform-in-opencv-java
 * https://stackoverflow.com/questions/44127342/detect-card-minarea-quadrilateral-from-contour-opencv
 *
 * @param p1  first point
 * @param p2  second point
 *
 * @return params
 */
cv::Vec3f calculate_params(const cv::Point2f& p1, const cv::Point2f& p2);

/** Get quadrilateral
 *
 * @param image        image to get
 * @param threashold1  first threshold
 * @param threashold2  second threshold
 * @param blur_radius  blur radius
 *
 * @return quadrilateral or none at all
 */
template <template <typename... Args> class Container>
std::optional<Container<cv::Point2f>> quadrilateral(const cv::Mat& image,
                                                    int threshold1 = 30,
                                                    int threshold2 = 100,
                                                    int blur_radius = 5);
/** Order points
 *
 * @param points   points to order
 *
 * @return ordered points
 */
template <template <typename... Args> class Container>
Container<cv::Point2f> order_points(const Container<cv::Point2f>& points);

/** Find intersections between two points param
 *
 * @param params1   first point params
 * @param params2   second point params
 *
 * @return intersection point
 */
cv::Point2f find_intersection(const cv::Vec3f& params1,
                              const cv::Vec3f& params2);

/** Get image dimension
 *
 * @param points          points to get
 * @param longer_length   longer length
 * @param shorter_length  shorter length
 *
 * @return image dimension
 */
template <template <typename... Args> class Container>
dimension image_dimension(const Container<cv::Point2f>& points,
                          int                           longer_length = 900 / 3,
                          int shorter_length = 600 / 3);
/** Warp image by points
 *
 * @param image   image to warp
 * @param points  points to warp
 *
 * @return warped image
 */
template <template <typename... Args> class Container>
cv::Mat warp(const cv::Mat&                image,
             const Container<cv::Point2f>& points,
             dimension&                    image_dimensions);

/** Remove shadows from image
 *
 * @param image          image to remove shadows
 * @param dilation_size  dilation size
 * @param blur_size      blur size
 *
 * @return shadows-free image
 */
cv::Mat remove_shadows(const cv::Mat& image,
                       int            dilation_size = 3,
                       int            blur_size = 21);

/** Get spray points
 *
 * @param points            known blobs
 * @param image_dimensions  image dimension
 * @param spray_radius      spray radius
 *
 * @return spray points
 */
template <template <typename... Args> class Container>
Container<cv::Point> spray_points(const Container<cv::Point2f>& points,
                                  const dimension& image_dimensions,
                                  int              spray_radius = 15 / 3);

}  // namespace util
}  // namespace detector

NAMESPACE_END

#endif  // LIB_DETECTOR_UTIL_HPP_
