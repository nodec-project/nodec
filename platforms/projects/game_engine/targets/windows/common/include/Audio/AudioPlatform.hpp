#pragma once

#include <Exceptions.hpp>

#include <wrl/client.h>
#include <x3daudio.h>
#include <xaudio2.h>

#pragma comment(lib, "xaudio2.lib")

// XAudio2 Sample codes:
//  * https://github.com/walbourn/directx-sdk-samples/blob/main/XAudio2/XAudio2Sound3D/audio.cpp

inline bool operator==(const WAVEFORMATEX &left, const WAVEFORMATEX &right) {
    return (left.wFormatTag == right.wFormatTag)
           && (left.nChannels == right.nChannels)
           && (left.nSamplesPerSec == right.nSamplesPerSec)
           && (left.nBlockAlign == right.nBlockAlign)
           && (left.wBitsPerSample == right.wBitsPerSample)
           && (left.cbSize == right.cbSize);
}

inline bool operator!=(const WAVEFORMATEX &left, const WAVEFORMATEX &right) {
    return !(left == right);
}

class AudioPlatform {
public:
    AudioPlatform() {
        using namespace Exceptions;

        ThrowIfFailed(XAudio2Create(&mpXAudio2), __FILE__, __LINE__);

#if defined(_DEBUG)
        // To see the trace output, you need to view ETW logs for this application:
        //    Go to Control Panel, Administrative Tools, Event Viewer.
        //    View->Show Analytic and Debug Logs.
        //    Applications and Services Logs / Microsoft / Windows / XAudio2.
        //    Right click on Microsoft Windows XAudio2 debug logging, Properties, then Enable Logging, and hit OK
        XAUDIO2_DEBUG_CONFIGURATION debug = {};
        debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
        debug.BreakMask = XAUDIO2_LOG_ERRORS;
        mpXAudio2->SetDebugConfiguration(&debug, 0);
#endif

        // Create a mastering voice
        ThrowIfFailed(mpXAudio2->CreateMasteringVoice(&mpMasteringVoice), __FILE__, __LINE__);

        DWORD channelMask;
        ThrowIfFailed(mpMasteringVoice->GetChannelMask(&channelMask), __FILE__, __LINE__);

        X3DAudioInitialize(channelMask, X3DAUDIO_SPEED_OF_SOUND, mX3DAudioHandle);
    }

    ~AudioPlatform() {
    }

    IXAudio2 &GetXAudio() noexcept {
        return *mpXAudio2.Get();
    }
    X3DAUDIO_HANDLE &GetX3DAudioHandle() noexcept {
        return mX3DAudioHandle;
    }

private:
    Microsoft::WRL::ComPtr<IXAudio2> mpXAudio2;
    IXAudio2MasteringVoice *mpMasteringVoice{nullptr};
    X3DAUDIO_HANDLE mX3DAudioHandle{0x00};
};