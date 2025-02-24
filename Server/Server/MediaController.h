#ifndef MEDIA_CONTROLLER_H
#define MEDIA_CONTROLLER_H

#include <string>

std::string GetNowPlayingInfo();
int GetCurrentVolume();
void PlayPause();
void NextTrack();
void PrevTrack();
void Mute();
void ChangeVolume(int volume);

#endif
