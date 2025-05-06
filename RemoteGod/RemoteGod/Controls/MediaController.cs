using System;
using System.Threading.Tasks;

namespace RemoteGod
{
    public class MediaController
    {
        private readonly ServerConnector serverConnector;
        private readonly MainPage mainPage;

        public MediaController(ServerConnector connector, MainPage page)
        {
            serverConnector = connector;
            mainPage = page;
        }

        public void PrevTrack()
        {
            serverConnector.SendCommand("MEDIA PREV");
        }

        public void PlayPause()
        {
            serverConnector.SendCommand("MEDIA PLAYPAUSE");
        }

        public void NextTrack()
        {
            serverConnector.SendCommand("MEDIA NEXT");
        }

        public void Mute()
        {
            serverConnector.SendCommand("VOLUME MUTE");
        }

        public void ChangeVolume(double volume)
        {
            int vol = (int)volume;
            serverConnector.SendCommand($"VOLUME {vol}");
        }

        public async Task UpdateNowPlaying()
        {
            string response = await serverConnector.SendCommandWithResponse("GET NOWPLAYING");
            mainPage.UpdateNowPlaying(response);
        }

        public async Task NameAudioOutputs()
        {
            string response = await serverConnector.SendCommandWithResponse("GET_AUDIO_OUTPUTS");
            mainPage.GetAudioOutputs(response);
        }

        public async Task UpdateVolume()
        {
            string response = await serverConnector.SendCommandWithResponse("GET VOLUME");
            if (int.TryParse(response, out int volume))
            {
                mainPage.UpdateVolume(volume);
            }
        }
    }
}
