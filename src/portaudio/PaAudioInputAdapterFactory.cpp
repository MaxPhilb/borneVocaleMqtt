/// @file PaAudioInputAdapterFactory.cpp
/// @author Victorien RAMAGET
/// @date Created on 2/10/2020
/// @copyright Copyright (c) 2020 Vivoka (vivoka.com)

#include "PaAudioInputAdapterFactory.hpp"

// Project includes
#include "PaAudioInputAdapter.hpp"

// Third-party includes
#include <portaudio.h>

PaAudioInputAdapterFactory::PaAudioInputAdapterFactory()
{
    if (auto const code = Pa_Initialize())
        throw std::runtime_error(fmt::format("PortAudio error: {}", Pa_GetErrorText(code)));
}

PaAudioInputAdapterFactory::~PaAudioInputAdapterFactory()
{
    Pa_Terminate();
}

auto PaAudioInputAdapterFactory::adapterType() const -> char const *
{
    return "PORTAUDIO_INPUT";
}

auto PaAudioInputAdapterFactory::createAdapter() const -> Vsdk::details::Csdk::IAudioInputAdapter *
{
    return new PaAudioInputAdapter;
}
