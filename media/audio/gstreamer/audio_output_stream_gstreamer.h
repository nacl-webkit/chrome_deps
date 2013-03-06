// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_AUDIO_AUDIO_OUTPUT_STREAM_GSTREAMER_H_
#define MEDIA_AUDIO_AUDIO_OUTPUT_STREAM_GSTREAMER_H_

#include "media/audio/audio_io.h"

#include "media/audio/audio_parameters.h"

class MessageLoop;
class AudioStreamGStreamer;

namespace media {

class AudioManagerGstreamer;

class AudioOutputStreamGstreamer : public AudioOutputStream {
 public:
  AudioOutputStreamGstreamer(const AudioParameters& params, AudioManagerGstreamer* manager);

  // Audio sources must implement AudioSourceCallback. This interface will be
  // called in a random thread which very likely is a high priority thread. Do
  // not rely on using this thread TLS or make calls that alter the thread
  // itself such as creating Windows or initializing COM.
  virtual ~AudioOutputStreamGstreamer() {}

  // Open the stream. false is returned if the stream cannot be opened.  Open()
  // must always be followed by a call to Close() even if Open() fails.
  virtual bool Open() OVERRIDE;

  // Starts playing audio and generating AudioSourceCallback::OnMoreData().
  // Since implementor of AudioOutputStream may have internal buffers, right
  // after calling this method initial buffers are fetched.
  //
  // The output stream does not take ownership of this callback.
  virtual void Start(AudioSourceCallback* callback) OVERRIDE;

  // Stops playing audio. Effect might not be instantaneous as the hardware
  // might have locked audio data that is processing.
  virtual void Stop() OVERRIDE;

  // Sets the relative volume, with range [0.0, 1.0] inclusive.
  virtual void SetVolume(double volume) OVERRIDE;

  // Gets the relative volume, with range [0.0, 1.0] inclusive.
  virtual void GetVolume(double* volume) OVERRIDE;

  // Close the stream. This also generates AudioSourceCallback::OnClose().
  // After calling this method, the object should not be used anymore.
  virtual void Close() OVERRIDE;
 private:
  bool IsOnAudioThread() const;

  const uint32 channels_;
  const ChannelLayout channel_layout_;
  const uint32 sample_rate_;
  const uint32 bytes_per_sample_;
  const uint32 bytes_per_frame_;
  AudioManagerGstreamer* manager_;
  MessageLoop* message_loop_;
  scoped_ptr<AudioBus> audio_bus_;
  scoped_ptr<AudioBus> render_bus_;
  AudioStreamGStreamer* stream_;

  enum InternalState {
    kInError = 0,
    kCreated,
    kIsOpened,
    kIsPlaying,
    kIsStopped,
    kIsClosed
  };
  InternalState state_;
};

}  // namespace media

#endif // MEDIA_AUDIO_AUDIO_OUTPUT_STREAM_GSTREAMER_H_
