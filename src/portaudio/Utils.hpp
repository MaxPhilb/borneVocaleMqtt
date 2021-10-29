/// @file PortAudio.hpp
/// @author Victorien RAMAGET
/// @date Created on 2/10/2020
/// @copyright Copyright (c) 2020 Vivoka (vivoka.com)

#pragma once

// C++ includes
#include <string>

// ext includes
#include <portaudio.h>

namespace PortAudio { namespace Utils
{
    inline auto deviceExists(std::string const & deviceName) -> PaError
    {
        auto const nbDevices = Pa_GetDeviceCount();
        for (int i = 0; i < nbDevices; ++i)
        {
            if (deviceName == Pa_GetDeviceInfo(i)->name)
                return paNoError;
        }
        return paInvalidDevice;
    }

    inline auto deviceIndex(std::string const & deviceName) -> int
    {
        auto const nbDevices = Pa_GetDeviceCount();
        for (int i = 0; i < nbDevices; ++i)
        {
            auto const deviceInfo = Pa_GetDeviceInfo(i);
            if (deviceName == deviceInfo->name)
                return i;
        }
        return paNoDevice;
    }

    inline auto availableDevices() -> std::string
    {
        std::string result;

        auto const nbDevices = Pa_GetDeviceCount();
        for (int i = 0; i < nbDevices; ++i)
        {
            auto const deviceInfo = Pa_GetDeviceInfo(i);
            result += (result.empty() ? deviceInfo->name : std::string(", ") + deviceInfo->name);
        }
        return result;
    }
}} // !namespace PortAudio::Utils
