#include <iostream>
#include <numeric>
#include <opencv2/opencv.hpp>
#include <stdexcept>

using namespace std;
using namespace cv;

struct img_dims {
  int height;
  int width;
};

Mat applyCLAHE(const Mat &image, double clip_limit = 4.0,
               const Size& tile_grid_size = Size(8, 8));
Vec3f calcParams(const Point2f &p1, const Point2f &p2);
Point2f findIntersection(const Vec3f &params1, const Vec3f &params2);
vector<Point2f> getQuadrilateral(const Mat &image, int threshold1 = 30,
                                 int threshold2 = 100, int blur_radius = 5);
vector<Point2f> orderPoints(const vector<Point2f> &points);
img_dims getImageDimensions(const vector<Point2f> &points,
                            int longer_length = 900 / 3,
                            int shorter_length = 600 / 3);
Mat warpImage(const Mat &image, const vector<Point2f> &points,
              img_dims image_dimensions);
Mat removeShadows(const Mat &image, int dilation_size = 3, int blur_size = 21);
vector<Point> getSprayPoints(const vector<Point2f> &points,
                             const img_dims &image_dimensions,
                             int spray_radius = 15 / 3);

int main(int argc, char **argv) {
  if (argc != 2) {
    cout << "No input image given" << endl;
    return -1;
  }
  // BGR
  Mat original_image = imread(argv[1], IMREAD_COLOR); // Read the file

  if (!original_image.data) {
    cout << "Invalid input" << endl;
    return -1;
  }

  namedWindow("original", WINDOW_AUTOSIZE);
  imshow("original", original_image);

  // Get 4 points for PerspectiveTransform
  vector<Point2f> source_points = getQuadrilateral(original_image);
  img_dims image_dimensions = getImageDimensions(source_points);
  Mat warped_image = warpImage(original_image, source_points, image_dimensions);

  namedWindow("warped", WINDOW_AUTOSIZE);
  imshow("warped", warped_image);

  // Lumination Normalization (CLAHE)
  // Mat clahe_image = applyCLAHE(warped_image);
  // namedWindow("clahe", WINDOW_AUTOSIZE);
  // imshow("clahe", clahe_image);

  // Remove shadows and noise, get binary mask through Otsu Thresholding
  Mat grayscale_warped;
  cvtColor(warped_image, grayscale_warped, COLOR_BGR2GRAY);

  Mat shadowless_image = removeShadows(grayscale_warped);
  Mat thresh_binary;
  threshold(shadowless_image, thresh_binary, 0, 255,
            THRESH_BINARY | THRESH_OTSU);

  // Detect Blobs
  // NOTE: hyperparameters here too
  SimpleBlobDetector::Params blob_params;
  blob_params.minArea = 5;
  blob_params.minCircularity = 0.5f;
  blob_params.minInertiaRatio = 0.1f;
  blob_params.minConvexity = 0.5;

  Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(blob_params);
  vector<KeyPoint> keypoints;
  detector->detect(thresh_binary, keypoints);
  drawKeypoints(grayscale_warped, keypoints, grayscale_warped,
                cv::Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

  namedWindow("grayscale_warped", WINDOW_AUTOSIZE);
  imshow("grayscale_warped", grayscale_warped);

  namedWindow("thresh_binary", WINDOW_AUTOSIZE);
  imshow("thresh_binary", thresh_binary);

  vector<Point2f> points;
  KeyPoint::convert(keypoints, points);

  vector<Point> spray_points = getSprayPoints(points, image_dimensions);

  while ((waitKey(0) & 0xFF) != 'q') {
  };
  return 0;
}

// CLAHE (Contrast Limited Adaptive Histogram Equalization)
// https://stackoverflow.com/questions/24341114/simple-illumination-correction-in-images-opencv-c
// https://docs.opencv.org/master/d6/dc7/group__imgproc__hist.html#gad689d2607b7b3889453804f414ab1018
Mat applyCLAHE(const Mat &image, double clip_limit,
               const Size &tile_grid_size) {
  // clip_limit and tile_grid_size are hyperparameters
  Mat lab_image;
  cvtColor(image, lab_image, COLOR_BGR2Lab);

  // Extract the L channel
  vector<Mat> lab_planes(3);
  split(lab_image, lab_planes); // now we have the L image in lab_planes[0]

  // apply the CLAHE algorithm to the L channel
  Ptr<CLAHE> clahe = createCLAHE(clip_limit, tile_grid_size);
  Mat dst;
  clahe->apply(lab_planes[0], dst);

  // Merge the the color planes back into an Lab image
  dst.copyTo(lab_planes[0]);
  merge(lab_planes, lab_image);

  // convert back to RGB
  Mat result;
  cvtColor(lab_image, result, COLOR_Lab2BGR);

  return result;
}

// https://stackoverflow.com/questions/44020751/how-to-detect-rectangle-from-houghlines-transform-in-opencv-java
// https://stackoverflow.com/questions/44127342/detect-card-minarea-quadrilateral-from-contour-opencv
Vec3f calcParams(const Point2f &p1, const Point2f &p2) // Solve for Ax+By+C=0
{
  float a, b, c;
  // If horizontal, A=0
  if (p2.y - p1.y == 0) {
    a = 0.0f;
    b = -1.0f;
    // If vertical, B=0
  } else if (p2.x - p1.x == 0) {
    a = -1.0f;
    b = 0.0f;
  } else {
    // A=gradient -> y=mx+c
    a = (p2.y - p1.y) / (p2.x - p1.x);
    b = -1.0f;
  }

  c = (-a * p1.x) - b * p1.y;
  return (Vec3f(a, b, c));
}

Point2f findIntersection(const Vec3f& params1, const Vec3f& params2) {
  float x = -1.0f, y = -1.0f;
  // Matrix determinant
  float det = params1[0] * params2[1] - params2[0] * params1[1];
  if (det < 0.1f && det > -0.1f)  // lines are approximately parallel
  {
    return (Point2f(-1.0f, -1.0f));
  } else {
    x = (params2[1] * -params1[2] - params1[1] * -params2[2]) / det;
    y = (params1[0] * -params2[2] - params2[0] * -params1[2]) / det;
  }
  return (Point2f(x, y));
}

vector<Point2f> getQuadrilateral(const Mat& image,
                                 int        threshold1,
                                 int        threshold2,
                                 int        blur_radius) {
  Mat grayscale;
  cvtColor(image, grayscale, COLOR_BGR2GRAY);
  GaussianBlur(grayscale, grayscale, Size(blur_radius, blur_radius), 0);

  Mat binary;
  Canny(grayscale, binary, threshold1, threshold2);
  // threshold(grayscale, binary, 0, 255, THRESH_BINARY | THRESH_OTSU);
  // adaptiveThreshold(grayscale, binary, 255, ADAPTIVE_THRESH_GAUSSIAN_C,
  // THRESH_BINARY, 9, 0);

  vector<vector<Point>> contours;
  findContours(binary, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

  vector<int> indices(contours.size());
  iota(indices.begin(), indices.end(), 0);

  vector<vector<Point>> hulls(contours.size());
  for (int i = 0; i < contours.size(); i++) {
    convexHull(Mat(contours[i]), hulls[i], false);
  }

  vector<float> hull_areas;
  for (int i = 0; i < contours.size(); i++) {
    hull_areas.push_back(contourArea(hulls[i]));
    // hull_areas.push_back(contourArea(hulls[i]) / hulls[i].size());
  }

  // Sort in Descending Order
  sort(indices.begin(), indices.end(), [&hull_areas](int lhs, int rhs) {
    return hull_areas[lhs] > hull_areas[rhs];
  });

  // Find the convex hull object for each contour
  vector<vector<Point>> hull(1);
  convexHull(Mat(contours[indices[0]]), hull[0], false);

  Mat convex_hull_mask(grayscale.rows, grayscale.cols, CV_8UC1);
  convex_hull_mask = Scalar(0);
  vector<Vec4i> lines;

  drawContours(convex_hull_mask, hull, 0, Scalar(255));
  imshow("convex_hull_mask", convex_hull_mask);
  HoughLinesP(convex_hull_mask, lines, 1, CV_PI / 180, 50, 10, 20);

  if (lines.size() < 4) {
    throw length_error("Not enough lines to perform corner detection");
  }
  cout << lines.size() << " lines" << endl;

  for (size_t i = 0; i < lines.size(); i++) {
    line(grayscale, Point(lines[i][0], lines[i][1]),
         Point(lines[i][2], lines[i][3]), Scalar(255), 1, 8);
  }

  vector<Vec3f> params(lines.size());
  for (int l = 0; l < lines.size(); l++) {
    params.push_back(calcParams(Point2f(lines[l][0], lines[l][1]),
                                Point2f(lines[l][2], lines[l][3])));
  }

  vector<Point2f> points;
  for (int i = 0; i < params.size(); i++) {
    // j starts at i so we don't have duplicated points
    for (int j = i; j < params.size(); j++) {
      Point2f intersec = findIntersection(params[i], params[j]);
      // if not parallel
      if ((intersec.x > 0) && (intersec.y > 0) &&
          (intersec.x < grayscale.cols) && (intersec.y < grayscale.rows)) {
        cout << "point: " << intersec << endl;
        points.push_back(intersec);
      }
    }
  }

  if (points.size() < 4) {
    throw length_error("Not enough points to perform K-means clustering");
  }

  for (int i = 0; i < points.size(); i++) {
    circle(grayscale, points[i], 3, Scalar(255), -1);
  }

  Mat             labels;
  vector<Point2f> centers;
  // 4 corners
  double compactness =
      kmeans(points, 4, labels,
             TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 20, 0.1), 10,
             KMEANS_PP_CENTERS, centers);

  for (int i = 0; i < centers.size(); i++) {
    circle(grayscale, centers[i], 3, Scalar(0), -1);
  }

  namedWindow("canny", WINDOW_AUTOSIZE);
  imshow("canny", binary);
  namedWindow("grayscale", WINDOW_AUTOSIZE);
  imshow("grayscale", grayscale);

  namedWindow("canny", WINDOW_AUTOSIZE);
  imshow("canny", binary);
  namedWindow("grayscale", WINDOW_AUTOSIZE);
  imshow("grayscale", grayscale);

  cout << "Center Size:" << centers.size() << endl;
  vector<Point2f> sorted_centers = orderPoints(centers);

  return sorted_centers;
}

// https://www.pyimagesearch.com/2014/08/25/4-point-opencv-getperspective-transform-example/
vector<Point2f> orderPoints(const vector<Point2f> &points) {
  int sums_min, sums_max, diffs_min, diffs_max;
  vector<float> sums(points.size());
  for (int i = 0; i < points.size(); i++) {
    sums[i] = points[i].x + points[i].y;
  }
  sums_min = distance(sums.begin(), min_element(sums.begin(), sums.end()));
  sums_max = distance(sums.begin(), max_element(sums.begin(), sums.end()));

  vector<float> diffs(points.size());
  for (int i = 0; i < points.size(); i++) {
    diffs[i] = points[i].y - points[i].x;
  }
  diffs_min = distance(diffs.begin(), min_element(diffs.begin(), diffs.end()));
  diffs_max = distance(diffs.begin(), max_element(diffs.begin(), diffs.end()));

  cout << sums_min << sums_max << diffs_min << diffs_max << endl;
  vector<Point2f> sorted_points;
  // Top Left, Top Right, Bottom Right, Bottom Left
  sorted_points.push_back(points[sums_min]);
  sorted_points.push_back(points[diffs_min]);
  sorted_points.push_back(points[sums_max]);
  sorted_points.push_back(points[diffs_max]);

  return sorted_points;
}

img_dims getImageDimensions(const vector<Point2f> &points, int longer_length,
                            int shorter_length) {
  // Top Left, Top Right, Bottom Right, Bottom Left
  float widthA, widthB, maxWidth, heightA, heightB, maxHeight;
  widthA =
      pow(points[2].x - points[3].x, 2) + pow(points[2].y - points[3].y, 2);
  widthB =
      pow(points[1].x - points[0].x, 2) + pow(points[1].y - points[0].y, 2);
  maxWidth = max(widthA, widthB);
  heightA =
      pow(points[1].x - points[2].x, 2) + pow(points[1].y - points[2].y, 2);
  heightB =
      pow(points[0].x - points[3].x, 2) + pow(points[0].y - points[3].y, 2);
  maxHeight = max(heightA, heightB);

  bool is_landscape = maxHeight < maxWidth;

  int height = is_landscape ? shorter_length : longer_length;
  int width = is_landscape ? longer_length : shorter_length;

  img_dims result = {height, width};
  return result;
}

Mat warpImage(const Mat &image, const vector<Point2f> &points,
              img_dims image_dimensions) {
  Mat homography = findHomography(
      points,
      vector<Point2f>{Point2f(0, 0), Point2f(image_dimensions.width, 0),
                      Point2f(image_dimensions.width, image_dimensions.height),
                      Point2f(0, image_dimensions.height)});

  Mat warped_image;
  warpPerspective(image, warped_image, homography,
                  Size(image_dimensions.width, image_dimensions.height));
  return warped_image;
}

// https://stackoverflow.com/questions/44047819/increase-image-brightness-without-overflow/44054699#44054699
Mat removeShadows(const Mat &image, int dilation_size, int blur_size) {
  Mat element = getStructuringElement(
      MORPH_ELLIPSE, Size(2 * dilation_size + 1, 2 * dilation_size + 1));

  Mat dilated_image;
  dilate(image, dilated_image, element);

  namedWindow("dilated", WINDOW_AUTOSIZE);
  imshow("dilated", dilated_image);

  Mat blurred_image;
  medianBlur(dilated_image, blurred_image, blur_size);

  namedWindow("blurred", WINDOW_AUTOSIZE);
  imshow("blurred", blurred_image);

  Mat diff_image;
  absdiff(blurred_image, image, diff_image);
  // Invert colors
  diff_image = cv::Scalar::all(255) - diff_image;

  namedWindow("diff", WINDOW_AUTOSIZE);
  imshow("diff", diff_image);

  Mat normalized_image;
  normalize(diff_image, normalized_image, 0, 255, NORM_MINMAX);
  namedWindow("normalized_image", WINDOW_AUTOSIZE);
  imshow("normalized_image", normalized_image);
  return normalized_image;
}

vector<Point> getSprayPoints(const vector<Point2f> &points,
                             const img_dims &image_dimensions,
                             int spray_radius) {
  Mat gaussian_image(image_dimensions.height, image_dimensions.width, CV_32FC1);
  gaussian_image = Scalar(0);
  for (Point2f point : points) {
    // arbitrary positive value
    gaussian_image.at<float>(point) = 1.0f;
  }

  // std deviation=spray_radius, convolution size=3*stdev
  GaussianBlur(gaussian_image, gaussian_image,
               Size(spray_radius * 6 + 1, spray_radius * 6 + 1), spray_radius,
               spray_radius);

  // https://stackoverflow.com/questions/5550290/find-local-maxima-in-grayscale-image-using-opencv
  Mat1b element(Size(3, 3), 1u);
  element.at<uchar>(1, 1) = 0u;

  Mat dilated_image;
  dilate(gaussian_image, dilated_image, element);

  Mat1b local_maxima = (gaussian_image > dilated_image);
  // only for visualization
  normalize(gaussian_image, gaussian_image, 0, 255, NORM_MINMAX);
  namedWindow("gaussian_image", WINDOW_AUTOSIZE);
  imshow("gaussian_image", gaussian_image);
  namedWindow("spray_locations", WINDOW_AUTOSIZE);
  imshow("spray_locations", local_maxima);

  vector<Point> spray_locations;
  findNonZero(local_maxima, spray_locations);

  return spray_locations;
}
