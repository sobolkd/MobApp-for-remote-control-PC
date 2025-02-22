using System;

namespace RemoteGod;

public class ScrollController
{
    private readonly ServerConnector serverConnector;
    private double lastY = 0;

    public ScrollController(ServerConnector connector)
    {
        serverConnector = connector;
    }

    public void Scroll(object sender, PanUpdatedEventArgs e)
    {
        if (e.StatusType == GestureStatus.Running)
        {
            double deltaY = e.TotalY - lastY;
            lastY = e.TotalY;

            if (Math.Abs(deltaY) > 5) // sensivity
            {
                int scrollAmount = (int)(deltaY * -3);
                serverConnector.SendCommand($"SCROLL {scrollAmount}");
            }
        }
        else if (e.StatusType == GestureStatus.Completed)
        {
            lastY = 0;
        }
    }
}
