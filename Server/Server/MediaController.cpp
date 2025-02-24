#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <iostream>
#include <string>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Media.h>

#pragma comment(lib, "windowsapp")

//Get Name of Song
std::string GetNowPlayingInfo() {
    try {
        using namespace winrt::Windows::Media::Control;
        using namespace winrt::Windows::Foundation;

        winrt::init_apartment();

        auto smtc = GlobalSystemMediaTransportControlsSessionManager::RequestAsync().get();
        if (!smtc) {
            return "Unknown Track";
        }

        auto session = smtc.GetCurrentSession();
        if (!session) {
            return "Unknown Track";
        }

        auto mediaProperties = session.TryGetMediaPropertiesAsync().get();

        std::wstring title = mediaProperties.Title().c_str();
        return std::string(title.begin(), title.end());
    }
    catch (...) {
        return "Unknown Track";
    }
}

// Get Volume 
int GetCurrentVolume() {
    HRESULT hr;
    CoInitialize(nullptr);

    IMMDeviceEnumerator* pEnumerator = nullptr;
    IMMDevice* pDevice = nullptr;
    IAudioEndpointVolume* pVolume = nullptr;

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (FAILED(hr)) return -1;

    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
    if (FAILED(hr)) { pEnumerator->Release(); return -1; }

    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pVolume);
    if (FAILED(hr)) { pDevice->Release(); pEnumerator->Release(); return -1; }

    float volume = 0.0f;
    pVolume->GetMasterVolumeLevelScalar(&volume);

    pVolume->Release();
    pDevice->Release();
    pEnumerator->Release();
    CoUninitialize();

    return static_cast<int>(volume * 100);
}

// Play/Pause
void PlayPause() {
    INPUT input[1] = {};
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wScan = 0;
    input[0].ki.time = 0;
    input[0].ki.dwExtraInfo = 0;
    input[0].ki.wVk = VK_MEDIA_PLAY_PAUSE;
    input[0].ki.dwFlags = 0;
    SendInput(1, input, sizeof(INPUT));

    input[0].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, input, sizeof(INPUT));

    std::cout << "Toggled Play/Pause" << std::endl;
}

// Next Track
void NextTrack() {
    INPUT input[1] = {};
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wScan = 0;
    input[0].ki.time = 0;
    input[0].ki.dwExtraInfo = 0;
    input[0].ki.wVk = VK_MEDIA_NEXT_TRACK;
    input[0].ki.dwFlags = 0;
    SendInput(1, input, sizeof(INPUT));

    input[0].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, input, sizeof(INPUT));

    std::cout << "Next Track" << std::endl;
}

// Previous Track
void PrevTrack() {
    INPUT input[1] = {};
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wScan = 0;
    input[0].ki.time = 0;
    input[0].ki.dwExtraInfo = 0;
    input[0].ki.wVk = VK_MEDIA_PREV_TRACK;
    input[0].ki.dwFlags = 0;
    SendInput(1, input, sizeof(INPUT));

    input[0].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, input, sizeof(INPUT));

    std::cout << "Previous Track" << std::endl;
}

// Mute/Unmute
void Mute() {
    INPUT input[1] = {};
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wScan = 0;
    input[0].ki.time = 0;
    input[0].ki.dwExtraInfo = 0;
    input[0].ki.wVk = VK_VOLUME_MUTE;
    input[0].ki.dwFlags = 0;
    SendInput(1, input, sizeof(INPUT));

    input[0].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, input, sizeof(INPUT));

    std::cout << "Mute/Unmute" << std::endl;
}

// Change Volume
void ChangeVolume(int volume) {
    if (volume < 0) volume = 0;
    if (volume > 100) volume = 100;

    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr)) {
        std::cout << "COM Initialization failed" << std::endl;
        return;
    }

    IMMDeviceEnumerator* pEnumerator = nullptr;
    IMMDevice* pDevice = nullptr;
    IAudioEndpointVolume* pVolume = nullptr;

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (FAILED(hr)) {
        CoUninitialize();
        return;
    }

    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
    if (FAILED(hr)) {
        pEnumerator->Release();
        CoUninitialize();
        return;
    }

    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&pVolume);
    if (FAILED(hr)) {
        pDevice->Release();
        pEnumerator->Release();
        CoUninitialize();
        return;
    }

    float currentVolume = 0.0f;
    pVolume->GetMasterVolumeLevelScalar(&currentVolume);

    float newVolume = volume / 100.0f;
    if (currentVolume != newVolume) {
        pVolume->SetMasterVolumeLevelScalar(newVolume, NULL);
    }

    pVolume->Release();
    pDevice->Release();
    pEnumerator->Release();
    CoUninitialize();
}

