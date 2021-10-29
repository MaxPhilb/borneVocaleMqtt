/// @file PaAudioInputAdapterFactory.hpp
/// @author Victorien RAMAGET
/// @date Created on 2/10/2020
/// @copyright Copyright (c) 2020 Vivoka (vivoka.com)

#pragma once

// VSDK includes
#include <vsdk/details/csdk/audio/IAudioInputAdapterFactory.hpp>

struct PaAudioInputAdapterFactory : public Vsdk::details::Csdk::IAudioInputAdapterFactory
{
    PaAudioInputAdapterFactory();
    ~PaAudioInputAdapterFactory() override;
    auto adapterType() const -> char const * override;
    auto createAdapter() const -> Vsdk::details::Csdk::IAudioInputAdapter * override;
};
