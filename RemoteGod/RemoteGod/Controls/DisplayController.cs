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
    }
}
