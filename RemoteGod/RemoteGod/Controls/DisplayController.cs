using System;

namespace RemoteGod
{
    public class DisplayController
    {
        private readonly ServerConnector serverConnector;

        public DisplayController(ServerConnector connector)
        {
            serverConnector = connector;
        }

        public void SetBrightness(int level)
        {
            if (level < 0 || level > 100)
            {
                Console.WriteLine("Invalid brightness level. It should be between 0 and 100.");
                return;
            }

            serverConnector.SendCommand($"BRIGHTNESS {level}");
        }

        public async Task SetResolution(string selected)
        {
            if (string.IsNullOrEmpty(selected))
                return;

            string[] parts = selected.Split('x');
            if (parts.Length != 2 || !int.TryParse(parts[0], out int width) || !int.TryParse(parts[1], out int height))
            {
                return;
            }
            await serverConnector.SendCommandWithResponse($"SET_RESOLUTION {width} {height}");
        }

        public async Task<int?> GetBrightness()
        {
            string response = await serverConnector.SendCommandWithResponse("GET_BRIGHTNESS");
            if (int.TryParse(response, out int brightness))
            {
                return brightness;
            }
            return null;
        }

    }
}
