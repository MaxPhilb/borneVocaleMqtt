/// @file PaAudioInputAdapter.cpp
/// @author Victorien RAMAGET
/// @date Created on 2/10/2020
/// @copyright Copyright (c) 2020 Vivoka (vivoka.com)

#include "PaAudioInputAdapter.hpp"

// Project includes
#include "Utils.hpp"

using namespace PortAudio::Utils;

namespace
{
    int processSamples(void const * inputBuffer, void *, unsigned long framesPerBuffer,
                     PaStreamCallbackTimeInfo const *, PaStreamCallbackFlags,
                     void * instance)
    {
        return static_cast<PaAudioInputAdapter *>(instance)->onProcessSamples(
            reinterpret_cast<int16_t const *>(inputBuffer), framesPerBuffer
        );
    }
} // !namespace

bool PaAudioInputAdapter::configure(nlohmann::json const & adapterParams)
{
    constexpr auto const key = "device_name";
    paAssert(adapterParams.contains(key),
            "No device name found in adapter configuration: missing key '{}'", key);

    auto deviceName = adapterParams[key].get<std::string>();
    paAssert(!deviceName.empty(), "No device name found in adapter configuration:"
                                 " key '{}' is empty", key);

    if (deviceName != "default")
    {
        if (!paAssert(deviceExists(deviceName),
                      "Input device '{}' not found. Available devices: [{}]",
                       deviceName, availableDevices()))
            return false;

        if (!paAssert(setDevice(deviceIndex(deviceName)), "Failed to set device"))
            return false;
    }
    else if (!paAssert(useDefaultDevice(), "Failed to retrieve system default input device"))
        return false;

    _streamParameters.hostApiSpecificStreamInfo = nullptr;
    _streamParameters.channelCount              = static_cast<int>(_channelCount);
    _streamParameters.sampleFormat              = paInt16;
    _streamParameters.suggestedLatency =  Pa_GetDeviceInfo(_streamParameters.device)
            ->defaultLowInputLatency;
    return true;
}

bool PaAudioInputAdapter::open()
{
    if (paAssert(Pa_OpenStream(&_stream, &_streamParameters, nullptr,
                               static_cast<double>(_sampleRate), _samplesPerChannel * _channelCount,
                               paClipOff, &processSamples, this), "Failed to open stream"))
    {
        return paAssert(Pa_SetStreamFinishedCallback(_stream, [] (void * instance) {
            static_cast<PaAudioInputAdapter *>(instance)->_isRecording = false;
        }), "Failed to set a stream finished callback");
    }
    return false;
}

bool PaAudioInputAdapter::start()
{
    if (paAssert(Pa_StartStream(_stream), "Failed to start stream"))
        _isRecording = true;

    return _isRecording;
}

bool PaAudioInputAdapter::stop()
{
    return paAssert(Pa_StopStream(_stream), "Failed to stop stream");
}

bool PaAudioInputAdapter::resume()
{
    return start();
}

bool PaAudioInputAdapter::close()
{
    return paAssert(Pa_CloseStream(_stream), "Failed to close stream");
}

int PaAudioInputAdapter::onProcessSamples(int16_t const * samples, std::size_t sampleCount)
{
    IAudioInputAdapter::onSamplesRecorded(samples, sampleCount);
    return _isRecording ? paContinue : paComplete;
}

bool PaAudioInputAdapter::paAssertImpl(PaError e, std::string const & msg)
{
    if (e != PaErrorCode::paNoError)
    {
        _lastError = fmt::format("{}: {}", msg, Pa_GetErrorText(e));
        return false;
    }
    else
    {
        _lastError.clear();
        return true;
    }
}

auto PaAudioInputAdapter::useDefaultDevice() -> PaError
{
    return setDevice(Pa_GetDefaultInputDevice());
}

auto PaAudioInputAdapter::setDevice(int deviceIndex) -> PaError
{
    return (_streamParameters.device = deviceIndex) != paNoDevice ? paNoError : paInternalError;
}
