// Copyright

#ifndef MEDIA_AUDIO_AUDIO_MANAGER_GSTREAMER_H_
#define MEDIA_AUDIO_AUDIO_MANAGER_GSTREAMER_H_

#include "media/audio/audio_manager_base.h"

namespace media {

class AudioManagerGstreamer : public AudioManagerBase {
 public:
  AudioManagerGstreamer();

  // Returns true if the OS reports existence of audio devices. This does not
  // guarantee that the existing devices support all formats and sample rates.
  virtual bool HasAudioOutputDevices() OVERRIDE;

  // Returns true if the OS reports existence of audio recording devices. This
  // does not guarantee that the existing devices support all formats and
  // sample rates.
  virtual bool HasAudioInputDevices() OVERRIDE;

  // Returns true if the platform specific audio input settings UI is known
  // and can be shown.
  virtual bool CanShowAudioInputSettings() OVERRIDE;

  // Opens the platform default audio input settings UI.
  // Note: This could invoke an external application/preferences pane, so
  // ideally must not be called from the UI thread or other time sensitive
  // threads to avoid blocking the rest of the application.
  virtual void ShowAudioInputSettings() OVERRIDE;

  // Appends a list of available input devices. It is not guaranteed that
  // all the devices in the list support all formats and sample rates for
  // recording.
  virtual void GetAudioInputDeviceNames(AudioDeviceNames* device_names) OVERRIDE;

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
  virtual AudioInputStream* MakeAudioInputStream(
      const AudioParameters& params, const std::string& device_id) OVERRIDE;

  // Used to determine if something else is currently making use of audio input.
  virtual bool IsRecordingInProcess() OVERRIDE;

  virtual void AddOutputDeviceChangeListener(AudioDeviceListener* listener) OVERRIDE;
  virtual void RemoveOutputDeviceChangeListener(
      AudioDeviceListener* listener) OVERRIDE;


  // From AudioManagerBase
  virtual AudioOutputStream* MakeLinearOutputStream(
      const AudioParameters& params);
  virtual AudioOutputStream* MakeLowLatencyOutputStream(
      const AudioParameters& params);
  virtual AudioInputStream* MakeLinearInputStream(
      const AudioParameters& params, const std::string& device_id);
  virtual AudioInputStream* MakeLowLatencyInputStream(
      const AudioParameters& params, const std::string& device_id);

 protected:
  virtual ~AudioManagerGstreamer();

 private:

  DISALLOW_COPY_AND_ASSIGN(AudioManagerGstreamer);
};

}  // namespace media

#endif // MEDIA_AUDIO_AUDIO_MANAGER_GSTREAMER_H_
