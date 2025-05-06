#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <iostream>
#include <string>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Media.h>
#include <vector>
#include <functiondiscoverykeys_devpkey.h>
#include <initguid.h>
#include <mmreg.h>
#include <exception>
#include <comdef.h>
#include <audiopolicy.h>


DEFINE_GUID(CLSID_PolicyConfigClient, 0x870af99c, 0x171d, 0x4f9e, 0xaf, 0x0d, 0xe6, 0x3d, 0xf4, 0x0c, 0x2b, 0xc9);
const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);

#pragma comment(lib, "windowsapp")
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "Uuid.lib")
const IID IID_IAudioEndpointVolume =
{ 0x5CDF2C82, 0x841E, 0x4546, { 0x97, 0x22, 0x0C, 0xF7, 0x40, 0x78, 0x22, 0x9A } };

struct __declspec(uuid("f8679f50-850a-41cf-9c72-430f290290c8")) IPolicyConfig;
struct IPolicyConfig : public IUnknown {
    virtual HRESULT GetMixFormat(LPCWSTR, WAVEFORMATEX**) { return E_NOTIMPL; }
    virtual HRESULT GetDeviceFormat(LPCWSTR, INT, WAVEFORMATEX**) { return E_NOTIMPL; }
    virtual HRESULT ResetDeviceFormat(LPCWSTR) { return E_NOTIMPL; }
    virtual HRESULT SetDeviceFormat(LPCWSTR, WAVEFORMATEX*, WAVEFORMATEX*) { return E_NOTIMPL; }
    virtual HRESULT GetProcessingPeriod(LPCWSTR, INT, PINT64, PINT64) { return E_NOTIMPL; }
    virtual HRESULT SetProcessingPeriod(LPCWSTR, PINT64) { return E_NOTIMPL; }
    virtual HRESULT GetShareMode(LPCWSTR, struct DeviceShareMode*) { return E_NOTIMPL; }
    virtual HRESULT SetShareMode(LPCWSTR, struct DeviceShareMode*) { return E_NOTIMPL; }
    virtual HRESULT GetPropertyValue(LPCWSTR, const PROPERTYKEY&, PROPVARIANT*) { return E_NOTIMPL; }
    virtual HRESULT SetPropertyValue(LPCWSTR, const PROPERTYKEY&, PROPVARIANT*) { return E_NOTIMPL; }
    virtual HRESULT SetDefaultEndpoint(LPCWSTR wszDeviceId, ERole role) = 0;
    virtual HRESULT SetEndpointVisibility(LPCWSTR, INT) { return E_NOTIMPL; }
};


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
// get all audio outputs
std::vector<std::wstring> GetAudioOutputs() {
    std::vector<std::wstring> audioOutputs;

    CoInitialize(nullptr);

    IMMDeviceEnumerator* pEnumerator = nullptr;
    IMMDeviceCollection* pCollection = nullptr;
    HRESULT hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pEnumerator));

    if (FAILED(hr)) {
        CoUninitialize();
        return audioOutputs;
    }

    hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCollection);
    if (FAILED(hr)) {
        pEnumerator->Release();
        CoUninitialize();
        return audioOutputs;
    }

    UINT count;
    pCollection->GetCount(&count);

    for (UINT i = 0; i < count; i++) {
        IMMDevice* pDevice = nullptr;
        hr = pCollection->Item(i, &pDevice);

        if (SUCCEEDED(hr)) {
            IPropertyStore* pProps = nullptr;
            hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);

            if (SUCCEEDED(hr)) {
                PROPVARIANT varName;
                PropVariantInit(&varName);

                hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
                if (SUCCEEDED(hr)) {
                    audioOutputs.push_back(varName.pwszVal);
                    PropVariantClear(&varName);
                }
                pProps->Release();
            }
            pDevice->Release();
        }
    }

    pCollection->Release();
    pEnumerator->Release();
    CoUninitialize();
    return audioOutputs;
}
// find device id
std::wstring FindDeviceIdByName(const std::wstring& deviceName)
{
    CoInitialize(nullptr);
    IMMDeviceEnumerator* pEnumerator = nullptr;
    HRESULT hr = CoCreateInstance(
        CLSID_MMDeviceEnumerator,
        nullptr,
        CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator),
        (void**)&pEnumerator);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to create device enumerator" << std::endl;
        CoUninitialize();
        return L"";
    }

    IMMDeviceCollection* pDevices = nullptr;
    hr = pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pDevices);
    if (FAILED(hr)) {
        std::wcerr << L"Failed to enumerate audio devices" << std::endl;
        pEnumerator->Release();
        CoUninitialize();
        return L"";
    }

    UINT deviceCount = 0;
    pDevices->GetCount(&deviceCount);
    for (UINT i = 0; i < deviceCount; ++i) {
        IMMDevice* pDevice = nullptr;
        hr = pDevices->Item(i, &pDevice);
        if (FAILED(hr)) {
            continue;
        }

        IPropertyStore* pProps = nullptr;
        hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
        if (FAILED(hr)) {
            pDevice->Release();
            continue;
        }

        PROPVARIANT varName;
        PropVariantInit(&varName);
        hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
        if (SUCCEEDED(hr)) {
            if (deviceName == varName.pwszVal) {
                LPWSTR deviceId = nullptr;
                hr = pDevice->GetId(&deviceId);
                if (SUCCEEDED(hr)) {
                    std::wstring id(deviceId);
                    CoTaskMemFree(deviceId);
                    pProps->Release();
                    pDevice->Release();
                    pDevices->Release();
                    pEnumerator->Release();
                    CoUninitialize();
                    return id;
                }
            }
        }

        PropVariantClear(&varName);
        pProps->Release();
        pDevice->Release();
    }

    pDevices->Release();
    pEnumerator->Release();
    CoUninitialize();
    return L"";
}

// change audio outputs
bool SetAudioOutput(const std::wstring& deviceName) {
    try {
        std::wstring deviceId = FindDeviceIdByName(deviceName);
        if (deviceId.empty()) {
            std::wcerr << L"Device not found: " << deviceName << std::endl;
            return false;
        }

        std::wcout << L"Device ID: " << deviceId << std::endl;

        CoInitialize(nullptr);
        IPolicyConfig* pPolicyConfig = nullptr;
        HRESULT hr = CoCreateInstance(
            CLSID_PolicyConfigClient,
            nullptr, CLSCTX_ALL,
            __uuidof(IPolicyConfig),
            (LPVOID*)&pPolicyConfig);

        if (FAILED(hr) || !pPolicyConfig) {
            CoUninitialize();
            throw std::runtime_error("Failed to create PolicyConfigClient instance");
        }

        hr = pPolicyConfig->SetDefaultEndpoint(deviceId.c_str(), eConsole);
        if (FAILED(hr)) {
            pPolicyConfig->Release();
            CoUninitialize();
            throw std::runtime_error("Failed to set default endpoint for eConsole");
        }

        hr = pPolicyConfig->SetDefaultEndpoint(deviceId.c_str(), eMultimedia);
        if (FAILED(hr)) {
            pPolicyConfig->Release();
            CoUninitialize();
            throw std::runtime_error("Failed to set default endpoint for eMultimedia");
        }

        hr = pPolicyConfig->SetDefaultEndpoint(deviceId.c_str(), eCommunications);
        if (FAILED(hr)) {
            pPolicyConfig->Release();
            CoUninitialize();
            throw std::runtime_error("Failed to set default endpoint for eCommunications");
        }

        pPolicyConfig->Release();
        CoUninitialize();

        return true;
    }
    catch (const std::exception& e) {
        std::wcerr << L"Error: " << e.what() << std::endl;
        return false;
    }
    catch (_com_error& e) {
        std::wcerr << L"COM Error: " << e.ErrorMessage() << std::endl;
        return false;
    }
    catch (...) {
        std::wcerr << L"Unknown error occurred" << std::endl;
        return false;
    }
}




