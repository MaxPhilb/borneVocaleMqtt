/// @file PaAudioInputAdapter.hpp
/// @author Victorien RAMAGET
/// @date Created on 2/10/2020
/// @copyright Copyright (c) 2020 Vivoka (vivoka.com)

#pragma once

// ext includes
#include <vsdk/details/csdk/audio/IAudioInputAdapter.hpp>
#include <portaudio.h>

class PaAudioInputAdapter : public Vsdk::details::Csdk::IAudioInputAdapter
{
private:
    PaStream *         _stream = nullptr;
    PaStreamParameters _streamParameters;

public:
    bool configure(nlohmann::json const & adapterParams) override;
    bool open() override;
    bool start() override;
    bool stop() override;
    bool resume() override;
    bool close() override;

public:
    int onProcessSamples(int16_t const * samples, std::size_t sampleCount);

private:
    auto setDevice(int deviceIndex) -> PaError;
    auto useDefaultDevice() -> PaError;

private:
    template<typename F, typename... Args>
    bool paAssert(PaError e, F format, Args &&... args) {
        return paAssertImpl(e, fmt::format(format, std::forward<Args>(args)...));
    }
    bool paAssertImpl(PaError e, std::string const & msg);
};
