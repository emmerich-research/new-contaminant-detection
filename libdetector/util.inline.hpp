#ifndef LIB_DETECTOR_UTIL_INLINE_HPP_
#define LIB_DETECTOR_UTIL_INLINE_HPP_

/** @file util.inline.hpp
 *  @brief Util Inline implementation
 *
 * Util inline implementation
 */

#include "util.hpp"

#include <algorithm>
#include <functional>
#include <iterator>
#include <numeric>

NAMESPACE_BEGIN

namespace detector {
namespace util {
template <template <typename... Args> class Container>
std::optional<Container<cv::Point2f>> quadrilateral(const cv::Mat& image,
                                                    int            threshold1,
                                                    int            threshold2,
                                                    int blur_radius) {
  cv::Mat grayscale;
  cv::cvtColor(image, grayscale, cv::COLOR_BGR2GRAY);
  cv::GaussianBlur(grayscale, grayscale, cv::Size{blur_radius, blur_radius}, 0);

  cv::Mat binary;
  cv::Canny(grayscale, binary, threshold1, threshold2);

  Container<Container<cv::Point>> contours;
  cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

  Container<int> indices{contours.size()};
  std::iota(indices.begin(), indices.end(), 0);

  Container<Container<cv::Point>> hulls{contours.size()};
  for (size_t i = 0; i < contours.size(); ++i) {
    cv::convexHull(cv::Mat{contours[i]}, hulls[i], false);
  }

  Container<float> hull_areas{contours.size()};
  std::transform(hulls.begin(), hulls.end(), std::back_inserter(hull_areas),
                 [](const auto& hull) { return contourArea(hull); });

  // Sort in Descending Order
  std::sort(indices.begin(), indices.end(), [&hull_areas](int lhs, int rhs) {
    return hull_areas[lhs] > hull_areas[rhs];
  });

  // Find the convex hull object for each contour
  Container<Container<cv::Point>> hull(1);
  cv::convexHull(cv::Mat{contours[indices[0]]}, hull[0], false);

  cv::Mat convex_hull_mask(grayscale.rows, grayscale.cols, CV_8UC1);
  convex_hull_mask = cv::Scalar(0);
  Container<cv::Vec4i> lines;

  cv::drawContours(convex_hull_mask, hull, 0, cv::Scalar(255));
  // cv::imshow("convex_hull_mask", convex_hull_mask);
  cv::HoughLinesP(convex_hull_mask, lines, 1, CV_PI / 180, 50, 10, 20);

  if (lines.size() < 4) {
    // LOG_ERROR();
    // throw length_error("Not enough lines to perform corner detection");
    return {};
  }

  for (size_t i = 0; i < lines.size(); ++i) {
    cv::line(grayscale, cv::Point{lines[i][0], lines[i][1]},
             cv::Point{lines[i][2], lines[i][3]}, cv::Scalar(255), 1, 8);
  }

  Container<cv::Vec3f> params(lines.size());
  std::transform(lines.begin(), lines.end(), std::back_inserter(params),
                 [](const auto& line) {
                   return calculate_params(cv::Point2f{line[0], line[1]},
                                           cv::Point2f{line[2], line[3]});
                 });

  Container<cv::Point2f> points;
  for (size_t i = 0; i < params.size(); i++) {
    // j starts at i so we don't have duplicated points
    for (size_t j = i; j < params.size(); j++) {
      cv::Point2f intersec = find_intersection(params[i], params[j]);
      // if not parallel
      if ((intersec.x > 0) && (intersec.y > 0) &&
          (intersec.x < static_cast<float>(grayscale.cols)) &&
          (intersec.y < static_cast<float>(grayscale.rows))) {
        // cout << "point: " << intersec << endl;
        points.push_back(intersec);
      }
    }
  }

  if (points.size() < 4) {
    return {};
    // throw length_error("Not enough points to perform K-means clustering");
  }

  for (size_t i = 0; i < points.size(); ++i) {
    cv::circle(grayscale, points[i], 3, cv::Scalar{255}, -1);
  }

  cv::Mat                labels;
  Container<cv::Point2f> centers;

  // 4 corners
  [[maybe_unused]] double compactness =
      kmeans(points, 4, labels,
             cv::TermCriteria{cv::TermCriteria::EPS + cv::TermCriteria::COUNT,
                              20, 0.1},
             10, cv::KMEANS_PP_CENTERS, centers);

  for (size_t i = 0; i < centers.size(); ++i) {
    cv::circle(grayscale, centers[i], 3, cv::Scalar{0}, -1);
  }

  // namedWindow("canny", WINDOW_AUTOSIZE);
  // imshow("canny", binary);
  // namedWindow("grayscale", WINDOW_AUTOSIZE);
  // imshow("grayscale", grayscale);

  // namedWindow("canny", WINDOW_AUTOSIZE);
  // imshow("canny", binary);
  // namedWindow("grayscale", WINDOW_AUTOSIZE);
  // imshow("grayscale", grayscale);

  // cout << "Center Size:" << centers.size() << endl;
  // vector<Point2f> sorted_centers = ;

  return order_points(centers);
}

template <template <typename... Args> class Container>
Container<cv::Point2f> order_points(const Container<cv::Point2f>& points) {
  int sums_min;
  int sums_max;
  int diffs_min;
  int diffs_max;

  Container<float> sums(points.size());

  for (int i = 0; i < points.size(); i++) {
    sums[i] = points[i].x + points[i].y;
  }

  const auto [s_min, s_max] =
      std::minmax_element(std::begin(sums), std::end(sums));

  sums_min = std::distance(sums.begin(), s_min);
  sums_max = std::distance(sums.begin(), s_max);

  Container<float> diffs(points.size());
  for (int i = 0; i < points.size(); i++) {
    diffs[i] = points[i].y - points[i].x;
  }

  const auto [d_min, d_max] =
      std::minmax_element(std::begin(diffs), std::end(diffs));

  diffs_min = std::distance(diffs.begin(), d_min);
  diffs_max = std::distance(diffs.begin(), d_max);

  return {points[sums_min], points[diffs_min], points[sums_max],
          points[diffs_max]};
}

template <template <typename... Args> class Container>
dimension image_dimension(const Container<cv::Point2f>& points,
                          int                           longer_length,
                          int                           shorter_length) {
  // Top Left, Top Right, Bottom Right, Bottom Left
  const float width_a = std::pow(points[2].x - points[3].x, 2) +
                        std::pow(points[2].y - points[3].y, 2);
  const float width_b = std::pow(points[1].x - points[0].x, 2) +
                        std::pow(points[1].y - points[0].y, 2);
  const float max_width = std::max(width_a, width_b);

  const float height_a = std::pow(points[1].x - points[2].x, 2) +
                         std::pow(points[1].y - points[2].y, 2);
  const float height_b = std::pow(points[0].x - points[3].x, 2) +
                         std::pow(points[0].y - points[3].y, 2);
  const float max_height = std::max(height_a, height_b);

  const bool is_landscape = max_height < max_width;

  const int height = is_landscape ? shorter_length : longer_length;
  const int width = is_landscape ? longer_length : shorter_length;

  return {height, width};
}

template <template <typename... Args> class Container>
cv::Mat warp(const cv::Mat&                image,
             const Container<cv::Point2f>& points,
             dimension&                    image_dimensions) {
  auto [height, width] = image_dimensions;

  cv::Mat homography = cv::findHomography(
      points, Container<cv::Point2f>{cv::Point2f{0, 0}, cv::Point2f{width, 0},
                                     cv::Point2f{width, height},
                                     cv::Point2f{0, height}});

  cv::Mat warped_image;
  cv::warpPerspective(image, warped_image, homography, cv::Size{width, height});

  return warped_image;
}  // namespace util

template <template <typename... Args> class Container>
Container<cv::Point> spray_points(const Container<cv::Point2f>& points,
                                  const dimension& image_dimensions,
                                  int              spray_radius) {
  auto [height, width] = image_dimensions;

  cv::Mat gaussian_image{height, width, CV_32FC1};
  gaussian_image = cv::Scalar{0};
  for (cv::Point2f point : points) {
    // arbitrary positive value
    gaussian_image.at<float>(point) = 1.0f;
  }

  // std deviation=spray_radius, convolution size=3*stdev
  cv::GaussianBlur(gaussian_image, gaussian_image,
                   cv::Size{spray_radius * 6 + 1, spray_radius * 6 + 1},
                   spray_radius, spray_radius);

  // https://stackoverflow.com/questions/5550290/find-local-maxima-in-grayscale-image-using-opencv
  cv::Mat1b element(cv::Size{3, 3}, 1u);
  element.at<uchar>(1, 1) = 0u;

  cv::Mat dilated_image;
  dilate(gaussian_image, dilated_image, element);

  cv::Mat1b local_maxima = (gaussian_image > dilated_image);
  // only for visualization
  cv::normalize(gaussian_image, gaussian_image, 0, 255, cv::NORM_MINMAX);
  // namedWindow("gaussian_image", WINDOW_AUTOSIZE);
  // imshow("gaussian_image", gaussian_image);
  // namedWindow("spray_locations", WINDOW_AUTOSIZE);
  // imshow("spray_locations", local_maxima);

  Container<cv::Point> spray_locations;
  cv::findNonZero(local_maxima, spray_locations);

  return spray_locations;
}
}  // namespace util
}  // namespace detector

NAMESPACE_END

#endif  // LIB_DETECTOR_UTIL_INLINE_HPP_
