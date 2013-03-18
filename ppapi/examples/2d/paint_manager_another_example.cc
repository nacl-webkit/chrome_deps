// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ppapi/c/pp_input_event.h"
#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/image_data.h"
#include "ppapi/cpp/input_event.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/size.h"
#include "ppapi/cpp/view.h"
#include "ppapi/utility/graphics/paint_manager.h"


static int color = 0;
static const int kSquareRadius = 40;

pp::Rect SquareForPoint(int x, int y) {
  return PP_MakeRectFromXYWH(x - kSquareRadius, y - kSquareRadius / 2,
                             kSquareRadius * 2 + 1, kSquareRadius + 1);
}
static void FillRect(pp::ImageData* image) {
  for (int y = 0; y < image->size().height(); y++) {
    for (int x = 0; x < image->size().width(); x++)
     *image->GetAddr32(pp::Point(x, y)) = 0xFF000000 | (0xFF * x / (image->size().width() - 1)) << (color << 3);
  }
}

class MyInstance : public pp::Instance, public pp::PaintManager::Client {
 public:
  MyInstance(PP_Instance instance)
      : pp::Instance(instance),
        paint_manager_(),
        last_x_(0),
        last_y_(0),
        is_scroll_pending_(false) {
    paint_manager_.Initialize(this, this, false);
    RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE);
  }

  virtual bool HandleInputEvent(const pp::InputEvent& event) {
    switch (event.GetType()) {
      case PP_INPUTEVENT_TYPE_MOUSEDOWN: {
        pp::MouseInputEvent mouse_event(event);
        if (mouse_event.GetButton() == PP_INPUTEVENT_MOUSEBUTTON_LEFT) {
          UpdateColor(true);
        }
        else if (mouse_event.GetButton() == PP_INPUTEVENT_MOUSEBUTTON_RIGHT) {
          last_x_ = static_cast<int>(mouse_event.GetPosition().x());
          last_y_ = static_cast<int>(mouse_event.GetPosition().y());
          is_scroll_pending_ = true;
          UpdateColor(false);
        }
        return true;
      }
      default:
        return false;
    }
  }

  virtual void DidChangeView(const pp::View& view) {
    paint_manager_.SetSize(view.GetRect().size());
  }

  // PaintManager::Client implementation.
  virtual bool OnPaint(pp::Graphics2D& graphics_2d,
                       const std::vector<pp::Rect>& paint_rects,
                       const pp::Rect& paint_bounds) {

    pp::ImageData *updated_image = new pp::ImageData
                                      (this, PP_IMAGEDATAFORMAT_BGRA_PREMUL,
                                       paint_bounds.size(), false);

    FillRect(updated_image);

    graphics_2d.ReplaceContents(updated_image);
    if (is_scroll_pending_) {
      paint_manager_.ScrollRect(SquareForPoint(last_x_, last_y_),
                                pp::Point(40,0));
      is_scroll_pending_ = false;
    }
    return true;
  }

 private:
  void UpdateColor(bool change_color) {
    if (change_color) {
      color++;
      if (color == 3)
        color = 0;
    }
    paint_manager_.Invalidate();
  }

  pp::PaintManager paint_manager_;

  int last_x_;
  int last_y_;
  bool is_scroll_pending_;
};

class MyModule : public pp::Module {
 public:
  virtual pp::Instance* CreateInstance(PP_Instance instance) {
    return new MyInstance(instance);
  }
};

namespace pp {

// Factory function for your specialization of the Module object.
Module* CreateModule() {
  return new MyModule();
}

}  // namespace pp
