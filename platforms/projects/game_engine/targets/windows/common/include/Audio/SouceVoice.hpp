#pragma once

#include "AudioIntegration.hpp"
#include <Exceptions.hpp>


class SourceVoice {

public:
    SourceVoice(AudioIntegration* pIntegration, WAVEFORMATEX wfx)
        : mWfx{ wfx } {
        using namespace Exceptions;

        ThrowIfFailed(pIntegration->GetXAudio().CreateSourceVoice(&mpSourceVoice, &wfx), __FILE__, __LINE__);
    }

    ~SourceVoice() {
        if (mpSourceVoice) {
            mpSourceVoice->Stop();
            mpSourceVoice->DestroyVoice();
            mpSourceVoice = nullptr;
        }
    }

    IXAudio2SourceVoice& GetVoice() {
        return *mpSourceVoice;
    }

    WAVEFORMATEX& GetWfx() {
        return mWfx;
    }

private:
    IXAudio2SourceVoice* mpSourceVoice{ nullptr };
    WAVEFORMATEX mWfx;
};