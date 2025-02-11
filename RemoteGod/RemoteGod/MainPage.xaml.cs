using System;
using System.Diagnostics;
using Microsoft.Maui.Controls;

namespace RemoteGod;

public partial class MainPage : ContentPage
{
    private readonly ServerConnector serverConnector;
    private double cursorX = 500;
    private double cursorY = 300;

    public MainPage()
    {
        InitializeComponent();
        serverConnector = new ServerConnector();

        serverConnector.OnIpReceived += ip => MainThread.BeginInvokeOnMainThread(() => IpLabel.Text = ip);
        serverConnector.OnCommandFailed += message => MainThread.BeginInvokeOnMainThread(() => IpLabel.Text = message);

        serverConnector.StartListening();
    }

    void Remote_Cursor_Clicked(object sender, EventArgs e)
    {
        JoystickView.IsVisible = true;
        Left_Click.IsVisible = true;
        Right_Click.IsVisible = true;
        Remote_Cursor.IsVisible = false;
    }

    void Joystick_Moved(object sender, (double X, double Y) e)
    {
        double scaleFactor = 10;

        cursorX += e.X * scaleFactor;
        cursorY += e.Y * scaleFactor;

        int intCursorX = (int)cursorX;
        int intCursorY = (int)cursorY;

        IpLabel.Text = $"{intCursorX} + {intCursorY}";

        cursorX = Math.Clamp(intCursorX, 0, 1920);
        cursorY = Math.Clamp(intCursorY, 0, 1080);

        serverConnector.SendCommand($"MOVE {intCursorX} {intCursorY}");
    }

    void LeftClick_Clicked(object sender, EventArgs e) => serverConnector.SendCommand("CLICK LEFT");

    void RightClick_Clicked(object sender, EventArgs e) => serverConnector.SendCommand("CLICK RIGHT");
}
