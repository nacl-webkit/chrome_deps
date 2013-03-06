// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/gstreamer/audio_output_stream_gstreamer.h"

#include "base/message_loop.h"
#include "media/audio/gstreamer/audio_manager_gstreamer.h"
#include <gst/gst.h>
#include "media/audio/gstreamer/audio_stream_gstreamer.h"

namespace media {

AudioOutputStreamGstreamer::AudioOutputStreamGstreamer(const AudioParameters& params, AudioManagerGstreamer* manager)
    : channels_(params.channels()),
      channel_layout_(params.channel_layout()),
      sample_rate_(params.sample_rate()),
      bytes_per_sample_(params.bits_per_sample() / 8),
      bytes_per_frame_(channels_ * params.bits_per_sample() / 8),
      manager_(manager),
      message_loop_(MessageLoop::current()),
      audio_bus_(AudioBus::Create(params)),
      render_bus_(AudioBus::CreateWrapper(params.channels())),
      stream_(0),
      state_(kCreated) {
  render_bus_->set_frames(params.frames_per_buffer());
}

bool AudioOutputStreamGstreamer::Open() {
  DCHECK(IsOnAudioThread());
  if (state_ == kCreated) {
    state_ = kIsOpened;
    return true;
  }
  return false;
}

void AudioOutputStreamGstreamer::Start(AudioSourceCallback* callback) {
  DCHECK(IsOnAudioThread());
  if (state_ != kIsOpened && state_ != kIsStopped)
    return;

  if (!stream_)
      stream_ = new AudioStreamGStreamer(callback, sample_rate_, render_bus_.get());

  stream_->start();
  state_ = kIsPlaying;
}

void AudioOutputStreamGstreamer::Stop() {
  if (stream_)
    stream_->stop();
  state_ = kIsStopped;
}

void AudioOutputStreamGstreamer::SetVolume(double volume) {
  NOTIMPLEMENTED();
}

void AudioOutputStreamGstreamer::GetVolume(double* volume) {
  NOTIMPLEMENTED();
}

void AudioOutputStreamGstreamer::Close() {
  delete stream_;
  state_ = kIsClosed;
  manager_->ReleaseOutputStream(this);
}

bool AudioOutputStreamGstreamer::IsOnAudioThread() const {
  return message_loop_ && message_loop_ == MessageLoop::current();
}

} // namespace media
