using System;
using System.Diagnostics;
using Microsoft.Maui.Controls;

namespace RemoteGod;

public partial class MainPage : ContentPage
{
    private readonly ServerConnector serverConnector;
    private readonly DisplayController displayController;
    private KeyboardController keyboardController;


    private double cursorX = 500;
    private double cursorY = 300;

    public MainPage()
    {
        InitializeComponent();
        serverConnector = new ServerConnector();
        displayController = new DisplayController(serverConnector);
        keyboardController = new KeyboardController(serverConnector);

        serverConnector.OnIpReceived += ip => MainThread.BeginInvokeOnMainThread(() => Title = ip);
        serverConnector.OnCommandFailed += message => MainThread.BeginInvokeOnMainThread(() => Title = message);

        serverConnector.StartListening();
    }

    void Remote_Cursor_Clicked(object sender, EventArgs e)
    {
        Back.IsVisible = true;
        JoystickView.IsVisible = true;
        Left_Click.IsVisible = true;
        Right_Click.IsVisible = true;
        Remote_Cursor.IsVisible = false;
        Display_Functions.IsVisible = false; 
        Remote_Keyboard.IsVisible = false;
        Media_Control.IsVisible = false;
        File_Manager.IsVisible = false;
        System.IsVisible = false;
        Spy_Mode.IsVisible = false;
        Browser.IsVisible = false;

    }

    void Display_Functions_Clicked(object sender, EventArgs e)
    {
        Display_Functions.IsVisible = false;
        Remote_Cursor.IsVisible = false;
        Media_Control.IsVisible = false;
        Back.IsVisible = true;
        BrightnessSlider.IsVisible = true;
        Remote_Keyboard.IsVisible = false;
        File_Manager.IsVisible = false;
        System.IsVisible = false;
        Spy_Mode.IsVisible = false;
        Browser.IsVisible = false;
    }

    void Remote_Keyboard_Clicked (object sender, EventArgs e)
    {
        Back.IsVisible = true;
        Display_Functions.IsVisible = false;
        Remote_Cursor.IsVisible = false;
        Media_Control.IsVisible = false;
        Back.IsVisible = true;
        Remote_Keyboard.IsVisible = false;
        File_Manager.IsVisible = false;
        System.IsVisible = false;
        Spy_Mode.IsVisible = false;
        Browser.IsVisible = false;
        Call_Keyboard.IsVisible = true;
    }

    void Media_Control_Clicked(object sender, EventArgs e)
    {
        Back.IsVisible = true;
        Remote_Cursor.IsVisible = false;
        Display_Functions.IsVisible = false;
        Remote_Keyboard.IsVisible = false;
        Media_Control.IsVisible = false;
        File_Manager.IsVisible = false;
        System.IsVisible = false;
        Spy_Mode.IsVisible = false;
        Browser.IsVisible = false;
    }

    void File_Manager_Clicked (object sender, EventArgs e)
    {
        Back.IsVisible = true;
        Remote_Cursor.IsVisible = false;
        Display_Functions.IsVisible = false;
        Remote_Keyboard.IsVisible = false;
        Media_Control.IsVisible = false;
        File_Manager.IsVisible = false;
        System.IsVisible = false;
        Spy_Mode.IsVisible = false;
        Browser.IsVisible = false;
    }

    void System_Clicked(object sender, EventArgs e)
    {
        Back.IsVisible = true;
        Remote_Cursor.IsVisible = false;
        Display_Functions.IsVisible = false;
        Remote_Keyboard.IsVisible = false;
        Media_Control.IsVisible = false;
        File_Manager.IsVisible = false;
        System.IsVisible = false;
        Spy_Mode.IsVisible = false;
        Browser.IsVisible = false;
    }

    void Spy_Mode_Clicked(object sender, EventArgs e)
    {
        Back.IsVisible = true;
        Remote_Cursor.IsVisible = false;
        Display_Functions.IsVisible = false;
        Remote_Keyboard.IsVisible = false;
        Media_Control.IsVisible = false;
        File_Manager.IsVisible = false;
        System.IsVisible = false;
        Spy_Mode.IsVisible = false;
        Browser.IsVisible = false;
    }

    void Browser_Clicked(object sender, EventArgs e)
    {
        Back.IsVisible = true;
        Remote_Cursor.IsVisible = false;
        Display_Functions.IsVisible = false;
        Remote_Keyboard.IsVisible = false;
        Media_Control.IsVisible = false;
        File_Manager.IsVisible = false;
        System.IsVisible = false;
        Spy_Mode.IsVisible = false;
        Browser.IsVisible = false;
    }

    void Back_Clicked(object sender, EventArgs e)
    {
        Display_Functions.IsVisible = true;
        Remote_Cursor.IsVisible = true;
        Remote_Keyboard.IsVisible = true;
        Media_Control.IsVisible = true;
        File_Manager.IsVisible = true;
        System.IsVisible = true;
        Spy_Mode.IsVisible = true;
        Browser.IsVisible = true;
        Back.IsVisible = false;
        JoystickView.IsVisible = false;
        Left_Click.IsVisible = false;
        Right_Click.IsVisible = false;
        BrightnessSlider.IsVisible = false;
        Call_Keyboard.IsVisible = false;
    }

    void Call_Keyboard_Clicked (object sender, EventArgs e)
    {
        keyboardController.OpenOnScreenKeyboard();
    }

    void Joystick_Moved(object sender, (double X, double Y) e)
    {
        double scaleFactor = 10;

        cursorX += e.X * scaleFactor;
        cursorY += e.Y * scaleFactor;

        int intCursorX = (int)cursorX;
        int intCursorY = (int)cursorY;

        cursorX = Math.Clamp(intCursorX, 0, 1920);
        cursorY = Math.Clamp(intCursorY, 0, 1080);

        serverConnector.SendCommand($"MOVE {intCursorX} {intCursorY}");
    }

    void BrightnessSlider_Changed(object sender, ValueChangedEventArgs e)
    {
        int brightness = (int)e.NewValue;
        displayController.SetBrightness(brightness);
    }

    void LeftClick_Clicked(object sender, EventArgs e) => serverConnector.SendCommand("CLICK LEFT");

    void RightClick_Clicked(object sender, EventArgs e) => serverConnector.SendCommand("CLICK RIGHT");
}
