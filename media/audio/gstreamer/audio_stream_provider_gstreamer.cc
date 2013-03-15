// Copyright

#include "media/audio/gstreamer/audio_stream_provider_gstreamer.h"

#include "base/bind.h"
#include "base/message_loop.h"

using namespace media;

AudioStreamProviderGStreamer::AudioStreamProviderGStreamer(media::AudioOutputStream::AudioSourceCallback* callback, media::AudioBus* bus)
    : message_loop_(MessageLoop::current()),
    callback_(callback),
    bus_(bus),
    state_(NoData) {
}

AudioStreamProviderGStreamer::~AudioStreamProviderGStreamer() {
}

int AudioStreamProviderGStreamer::GetMoreData(media::AudioBus* bus) {
    callback_->WaitTillDataReady();

    base::AutoLock lock(lock_);
    int frames = 0;

    if (state_ == WaitingForData)
      return 0;

    message_loop_->PostTask(FROM_HERE, base::Bind(
        &AudioStreamProviderGStreamer::GetMoreDataOnAudioThread, this));

    if (state_ == HasData) {
      bus_->CopyTo(bus);
      frames = bus_->frames();
    }
    state_ = WaitingForData;
    return frames;
}

void AudioStreamProviderGStreamer::CancelGetMoreData() {
    base::AutoLock lock(lock_);
    state_ = NoData;
}

void AudioStreamProviderGStreamer::GetMoreDataOnAudioThread() {
    base::AutoLock lock(lock_);
    if (state_ != WaitingForData)
        return;

    state_ = NoData;
    AudioBuffersState s;
    if (callback_->OnMoreData(bus_, s) > 0)
      state_ = HasData;
}
