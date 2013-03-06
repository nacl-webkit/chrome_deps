/*
 *  Copyright (C) 2011, 2012 Igalia S.L
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef MEDIA_AUDIO_AUDIO_STREAM_SOURCE_GSTREAMER_H_
#define MEDIA_AUDIO_AUDIO_STREAM_SOURCE_GSTREAMER_H_

#if USE(GSTREAMER)

#include <gst/gst.h>

#define WEBKIT_TYPE_PEPPER_SRC (webkit_pepper_src_get_type())
#define WEBKIT_PEPPER_SRC(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), WEBKIT_TYPE_PEPPER_SRC, WebKitPepperSrc))

typedef struct _WebKitPepperSrc WebKitPepperSrc;

GType webkit_pepper_src_get_type();

#endif // USE(GSTREAMER)
#endif
