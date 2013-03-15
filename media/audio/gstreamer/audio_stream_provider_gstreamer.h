// Copyright

#ifndef MEDIA_AUDIO_AUDIO_STREAM_PROVIDER_GSTREAMER_H_
#define MEDIA_AUDIO_AUDIO_STREAM_PROVIDER_GSTREAMER_H_

#include "base/memory/ref_counted.h"
#include "base/synchronization/lock.h"
#include "media/base/audio_bus.h"
#include "media/audio/audio_io.h"

class MessageLoop;

class AudioStreamProviderGStreamer : public base::RefCountedThreadSafe<AudioStreamProviderGStreamer> {
public:
    AudioStreamProviderGStreamer(media::AudioOutputStream::AudioSourceCallback* callback, media::AudioBus* bus);
    virtual ~AudioStreamProviderGStreamer();
    int GetMoreData(media::AudioBus* bus);
    void CancelGetMoreData();
    friend class base::RefCountedThreadSafe<AudioStreamProviderGStreamer>;

private:
    void GetMoreDataOnAudioThread();

    MessageLoop* message_loop_;
    media::AudioOutputStream::AudioSourceCallback* callback_;
    media::AudioBus* bus_;
    base::Lock lock_;
    enum DataState {
      NoData = 0,
      WaitingForData,
      HasData
    };

    DataState state_;

    DISALLOW_COPY_AND_ASSIGN(AudioStreamProviderGStreamer);
};

#endif // MEDIA_AUDIO_AUDIO_STREAM_PROVIDER_GSTREAMER_H_
