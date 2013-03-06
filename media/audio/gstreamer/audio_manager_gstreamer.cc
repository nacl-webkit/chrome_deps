// Copyright

#include "media/audio/gstreamer/audio_manager_gstreamer.h"

#include "gstreamer/GStreamerUtilities.h"
#include "media/audio/gstreamer/audio_output_stream_gstreamer.h"

namespace media {

static AudioManagerGstreamer* g_manager = 0;

AudioManager* CreateAudioManager() {
  WebCore::initializeGStreamer();
  if (!g_manager)
    g_manager = new AudioManagerGstreamer();
  return g_manager;
}

AudioManagerGstreamer::AudioManagerGstreamer() {

}

bool AudioManagerGstreamer::HasAudioOutputDevices() {
  return true;
}

bool AudioManagerGstreamer::HasAudioInputDevices() {
  NOTIMPLEMENTED();
}

bool AudioManagerGstreamer::CanShowAudioInputSettings() {
  return false;
}

void AudioManagerGstreamer::ShowAudioInputSettings() {
  NOTREACHED();
}

void AudioManagerGstreamer::GetAudioInputDeviceNames(
    AudioDeviceNames* device_names) {
  NOTIMPLEMENTED();
}

AudioInputStream* AudioManagerGstreamer::MakeAudioInputStream(
    const AudioParameters& params, const std::string& device_id) {
  NOTIMPLEMENTED();
}

bool AudioManagerGstreamer::IsRecordingInProcess() {
  NOTIMPLEMENTED();
}

void AudioManagerGstreamer::AddOutputDeviceChangeListener(
    AudioDeviceListener* listener) {
  NOTIMPLEMENTED();
}

void AudioManagerGstreamer::RemoveOutputDeviceChangeListener(
    AudioDeviceListener* listener) {
  NOTIMPLEMENTED();
}

AudioOutputStream* AudioManagerGstreamer::MakeLinearOutputStream(
    const AudioParameters& params) {
  return new AudioOutputStreamGstreamer(params, this);
}

AudioOutputStream* AudioManagerGstreamer::MakeLowLatencyOutputStream(
    const AudioParameters& params) {
  return new AudioOutputStreamGstreamer(params, this);
}

AudioInputStream* AudioManagerGstreamer::MakeLinearInputStream(
    const AudioParameters& params, const std::string& device_id) {
  NOTIMPLEMENTED();
}

AudioInputStream* AudioManagerGstreamer::MakeLowLatencyInputStream(
    const AudioParameters& params, const std::string& device_id) {
  NOTIMPLEMENTED();
}

AudioManagerGstreamer::~AudioManagerGstreamer() {
}

}  // namespace media
