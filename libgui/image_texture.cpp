#include "gui.hpp"

#include "image_texture.hpp"

NAMESPACE_BEGIN

namespace gui {
ImageTexture::ImageTexture() {}

ImageTexture::ImageTexture(const cv::Mat* frame) {
  image(frame);
}

ImageTexture::~ImageTexture() {
  destroy();  // just to make sure
}

void ImageTexture::destroy() {
  glBindTexture(GL_TEXTURE_2D, 0);
  glDeleteTextures(1, &opengl_texture_);
}

void ImageTexture::image(const cv::Mat* frame) {
  width_ = static_cast<float>(frame->cols);
  height_ = static_cast<float>(frame->rows);

  glGenTextures(1, &opengl_texture_);
  glBindTexture(GL_TEXTURE_2D, opengl_texture_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_BGR,
               GL_UNSIGNED_BYTE, frame->data);
}
}  // namespace gui

NAMESPACE_END
