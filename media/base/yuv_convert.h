// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_YUV_CONVERT_H_
#define MEDIA_BASE_YUV_CONVERT_H_

#include "base/basictypes.h"

namespace media {

// Type of YUV surface.
// The value of these enums matter as they are used to shift vertical indices.
enum YUVType {
  YV16 = 0,           // YV16 is half width and full height chroma channels.
  YV12 = 1,           // YV12 is half width and half height chroma channels.
};

// Mirror means flip the image horizontally, as in looking in a mirror.
// Rotate happens after mirroring.
enum Rotate {
  ROTATE_0,           // Rotation off.
  ROTATE_90,          // Rotate clockwise.
  ROTATE_180,         // Rotate upside down.
  ROTATE_270,         // Rotate counter clockwise.
  MIRROR_ROTATE_0,    // Mirror horizontally.
  MIRROR_ROTATE_90,   // Mirror then Rotate clockwise.
  MIRROR_ROTATE_180,  // Mirror vertically.
  MIRROR_ROTATE_270,  // Transpose.
};

// Filter affects how scaling looks.
enum ScaleFilter {
  FILTER_NONE = 0,        // No filter (point sampled).
  FILTER_BILINEAR_H = 1,  // Bilinear horizontal filter.
  FILTER_BILINEAR_V = 2,  // Bilinear vertical filter.
  FILTER_BILINEAR = 3,    // Bilinear filter.
};

// Convert a frame of YUV to 32 bit ARGB.
// Pass in YV16/YV12 depending on source format
void ConvertYUVToRGB32(const uint8* yplane,
                       const uint8* uplane,
                       const uint8* vplane,
                       uint8* rgbframe,
                       int width,
                       int height,
                       int ystride,
                       int uvstride,
                       int rgbstride,
                       YUVType yuv_type);

// Scale a frame of YUV to 32 bit ARGB.
// Supports rotation and mirroring.
void ScaleYUVToRGB32(const uint8* yplane,
                     const uint8* uplane,
                     const uint8* vplane,
                     uint8* rgbframe,
                     int source_width,
                     int source_height,
                     int width,
                     int height,
                     int ystride,
                     int uvstride,
                     int rgbstride,
                     YUVType yuv_type,
                     Rotate view_rotate,
                     ScaleFilter filter);

// Biliner Scale a frame of YV12 to 32 bits ARGB on a specified rectangle.
// |yplane|, etc and |rgbframe| should point to the top-left pixels of the
// source and destination buffers.
void ScaleYUVToRGB32WithRect(const uint8* yplane,
                             const uint8* uplane,
                             const uint8* vplane,
                             uint8* rgbframe,
                             int source_width,
                             int source_height,
                             int dest_width,
                             int dest_height,
                             int dest_rect_left,
                             int dest_rect_top,
                             int dest_rect_right,
                             int dest_rect_bottom,
                             int ystride,
                             int uvstride,
                             int rgbstride);

void ConvertRGB32ToYUV(const uint8* rgbframe,
                       uint8* yplane,
                       uint8* uplane,
                       uint8* vplane,
                       int width,
                       int height,
                       int rgbstride,
                       int ystride,
                       int uvstride);

void ConvertRGB24ToYUV(const uint8* rgbframe,
                       uint8* yplane,
                       uint8* uplane,
                       uint8* vplane,
                       int width,
                       int height,
                       int rgbstride,
                       int ystride,
                       int uvstride);

void ConvertYUY2ToYUV(const uint8* src,
                      uint8* yplane,
                      uint8* uplane,
                      uint8* vplane,
                      int width,
                      int height);

void ConvertNV21ToYUV(const uint8* src,
                      uint8* yplane,
                      uint8* uplane,
                      uint8* vplane,
                      int width,
                      int height);

// Empty SIMD register state after calling optimized scaler functions.
void EmptyRegisterState();

}  // namespace media

#endif  // MEDIA_BASE_YUV_CONVERT_H_
