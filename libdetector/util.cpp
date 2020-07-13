#include "detector.hpp"

#include "util.hpp"

#include <vector>

NAMESPACE_BEGIN

namespace detector {
namespace util {
cv::Mat apply_CLAHE(const cv::Mat&  image,
                    double          clip_limit,
                    const cv::Size& tile_grid_size) {
  // clip_limit and tile_grid_size are hyperparameters
  cv::Mat lab_image;
  cv::cvtColor(image, lab_image, cv::COLOR_BGR2Lab);

  // Extract the L channel
  std::vector<cv::Mat> lab_planes(3);
  cv::split(lab_image, lab_planes);  // now we have the L image in lab_planes[0]

  // apply the CLAHE algorithm to the L channel
  auto&&  clahe = createCLAHE(clip_limit, tile_grid_size);
  cv::Mat dst;
  clahe->apply(lab_planes[0], dst);

  // Merge the the color planes back into an Lab image
  dst.copyTo(lab_planes[0]);
  cv::merge(lab_planes, lab_image);

  // convert back to RGB
  cv::Mat result;
  cv::cvtColor(lab_image, result, cv::COLOR_Lab2BGR);

  return result;
}

cv::Vec3f calculate_params(const cv::Point2f& p1, const cv::Point2f& p2) {
  float a, b, c;

  // If horizontal, A=0
  if ((p2.y - p1.y) == 0.0f) {
    a = 0.0f;
    b = -1.0f;
    // If vertical, B=0
  } else if ((p2.x - p1.x) == 0.0f) {
    a = -1.0f;
    b = 0.0f;
  } else {
    // A=gradient -> y=mx+c
    a = (p2.y - p1.y) / (p2.x - p1.x);
    b = -1.0f;
  }

  c = (-a * p1.x) - b * p1.y;

  return cv::Vec3f{a, b, c};
}

// https://stackoverflow.com/questions/44047819/increase-image-brightness-without-overflow/44054699#44054699
cv::Mat remove_shadows(const cv::Mat& image, int dilation_size, int blur_size) {
  cv::Mat element = cv::getStructuringElement(
      cv::MORPH_ELLIPSE,
      cv::Size{2 * dilation_size + 1, 2 * dilation_size + 1});

  cv::Mat dilated_image;
  cv::dilate(image, dilated_image, element);

  // namedWindow("dilated", WINDOW_AUTOSIZE);
  // imshow("dilated", dilated_image);

  cv::Mat blurred_image;
  medianBlur(dilated_image, blurred_image, blur_size);

  // namedWindow("blurred", WINDOW_AUTOSIZE);
  // imshow("blurred", blurred_image);

  cv::Mat diff_image;
  absdiff(blurred_image, image, diff_image);
  // Invert colors
  diff_image = cv::Scalar::all(255) - diff_image;

  // namedWindow("diff", cv::WINDOW_AUTOSIZE);
  // imshow("diff", diff_image);

  cv::Mat normalized_image;
  cv::normalize(diff_image, normalized_image, 0, 255, cv::NORM_MINMAX);
  // namedWindow("normalized_image", WINDOW_AUTOSIZE);
  // imshow("normalized_image", normalized_image);

  return normalized_image;
}

cv::Point2f find_intersection(const cv::Vec3f& params1,
                              const cv::Vec3f& params2) {
  float x = -1.0f, y = -1.0f;

  // Matrix determinant
  float det = params1[0] * params2[1] - params2[0] * params1[1];

  if (det < 0.1f && det > -0.1f) {
    // lines are approximately parallel
    return cv::Point2f{-1.0f, -1.0f};
  }

  x = (params2[1] * (-params1[2]) - params1[1] * (-params2[2])) / det;
  y = (params1[0] * (-params2[2]) - params2[0] * (-params1[2])) / det;

  return cv::Point2f{x, y};
}
}  // namespace util
}  // namespace detector

NAMESPACE_END
