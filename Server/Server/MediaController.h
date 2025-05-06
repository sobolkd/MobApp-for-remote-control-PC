#pragma once

#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <string>
#include <vector>
#include <functiondiscoverykeys_devpkey.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Media.h>
#include <initguid.h>
#include <mmreg.h>
#include <mmdeviceapi.h>

std::string GetNowPlayingInfo();
int GetCurrentVolume();
void PlayPause();
void NextTrack();
void PrevTrack();
void Mute();
void ChangeVolume(int volume);
std::vector<std::wstring> GetAudioOutputs();
bool SetAudioOutput(const std::wstring& deviceId);
