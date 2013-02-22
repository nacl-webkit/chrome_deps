// Copyright

#include "media/audio/bridge/audio_manager_bridge.h"

namespace media {

static AudioManagerBridge* g_manager = 0;

AudioManager* CreateAudioManager() {
  if (!g_manager)
    g_manager = new AudioManagerBridge();
  return g_manager;
}

AudioManagerBridge::AudioManagerBridge() {

}

// Returns true if the OS reports existence of audio devices. This does not
// guarantee that the existing devices support all formats and sample rates.
bool AudioManagerBridge::HasAudioOutputDevices() {
    NOTIMPLEMENTED();
}

// Returns true if the OS reports existence of audio recording devices. This
// does not guarantee that the existing devices support all formats and
// sample rates.
bool AudioManagerBridge::HasAudioInputDevices() {
    NOTIMPLEMENTED();
}

// Returns true if the platform specific audio input settings UI is known
// and can be shown.
bool AudioManagerBridge::CanShowAudioInputSettings() {
    return false;
}

// Opens the platform default audio input settings UI.
// Note: This could invoke an external application/preferences pane, so
// ideally must not be called from the UI thread or other time sensitive
// threads to avoid blocking the rest of the application.
void AudioManagerBridge::ShowAudioInputSettings() {
    NOTREACHED();
}

// Appends a list of available input devices. It is not guaranteed that
// all the devices in the list support all formats and sample rates for
// recording.
void AudioManagerBridge::GetAudioInputDeviceNames(AudioDeviceNames* device_names) {
    NOTIMPLEMENTED();
}

// Factory for all the supported stream formats. |params| defines parameters
// of the audio stream to be created.
//
// |params.sample_per_packet| is the requested buffer allocation which the
// audio source thinks it can usually fill without blocking. Internally two
// or three buffers are created, one will be locked for playback and one will
// be ready to be filled in the call to AudioSourceCallback::OnMoreData().
//
// Returns NULL if the combination of the parameters is not supported, or if
// we have reached some other platform specific limit.
//
// |params.format| can be set to AUDIO_PCM_LOW_LATENCY and that has two
// effects:
// 1- Instead of triple buffered the audio will be double buffered.
// 2- A low latency driver or alternative audio subsystem will be used when
//    available.
//
// Do not free the returned AudioOutputStream. It is owned by AudioManager.
AudioOutputStream* AudioManagerBridge::MakeAudioOutputStream(
    const AudioParameters& params) {
    NOTIMPLEMENTED();
}

// Creates new audio output proxy. A proxy implements
// AudioOutputStream interface, but unlike regular output stream
// created with MakeAudioOutputStream() it opens device only when a
// sound is actually playing.
AudioOutputStream* AudioManagerBridge::MakeAudioOutputStreamProxy(
    const AudioParameters& params) {
    NOTIMPLEMENTED();
}

// Factory to create audio recording streams.
// |channels| can be 1 or 2.
// |sample_rate| is in hertz and can be any value supported by the platform.
// |bits_per_sample| can be any value supported by the platform.
// |samples_per_packet| is in hertz as well and can be 0 to |sample_rate|,
// with 0 suggesting that the implementation use a default value for that
// platform.
// Returns NULL if the combination of the parameters is not supported, or if
// we have reached some other platform specific limit.
//
// Do not free the returned AudioInputStream. It is owned by AudioManager.
// When you are done with it, call |Stop()| and |Close()| to release it.
AudioInputStream* AudioManagerBridge::MakeAudioInputStream(
    const AudioParameters& params, const std::string& device_id) {
    NOTIMPLEMENTED();
}

// Used to determine if something else is currently making use of audio input.
bool AudioManagerBridge::IsRecordingInProcess() {
    NOTIMPLEMENTED();
}

void AudioManagerBridge::AddOutputDeviceChangeListener(AudioDeviceListener* listener) {
    NOTIMPLEMENTED();
}

void AudioManagerBridge::RemoveOutputDeviceChangeListener(
    AudioDeviceListener* listener) {
    NOTIMPLEMENTED();
}

AudioOutputStream* AudioManagerBridge::MakeLinearOutputStream(
    const AudioParameters& params) {
    NOTREACHED();
}

AudioOutputStream* AudioManagerBridge::MakeLowLatencyOutputStream(
    const AudioParameters& params) {
    NOTREACHED();
}

AudioInputStream* AudioManagerBridge::MakeLinearInputStream(
    const AudioParameters& params, const std::string& device_id) {
    NOTREACHED();
}

// Creates the input stream for the |AUDIO_PCM_LOW_LATENCY| format.
AudioInputStream* AudioManagerBridge::MakeLowLatencyInputStream(
    const AudioParameters& params, const std::string& device_id) {
    NOTREACHED();
}

AudioManagerBridge::~AudioManagerBridge() {

}

}  // namespace media
