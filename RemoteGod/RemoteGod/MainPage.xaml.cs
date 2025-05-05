using System;
using System.Diagnostics;
using Microsoft.Maui.Controls;
using static Android.Print.PrintAttributes;

namespace RemoteGod;

public partial class MainPage : ContentPage
{
    private readonly ServerConnector serverConnector;
    private readonly DisplayController displayController;
    private KeyboardController keyboardController;
    private readonly ScrollController _scrollController;
    private MediaController mediaController;

    private double cursorX = 500;
    private double cursorY = 300;

    public MainPage()
    {
        InitializeComponent();
        serverConnector = new ServerConnector();
        displayController = new DisplayController(serverConnector);
        keyboardController = new KeyboardController(serverConnector);
        _scrollController = new ScrollController(serverConnector);
        mediaController = new MediaController(serverConnector, this);

        serverConnector.OnIpReceived += ip => MainThread.BeginInvokeOnMainThread(() => Title = ip);
        serverConnector.OnCommandFailed += message => MainThread.BeginInvokeOnMainThread(() => Title = message);

        serverConnector.StartListening();
    }

    private void OnPanUpdated(object sender, PanUpdatedEventArgs e)
    {
        _scrollController.Scroll(sender, e);
    }
    void Remote_Cursor_Clicked(object sender, EventArgs e)
    {
        Back.IsVisible = true;
        JoystickView.IsVisible = true;
        Left_Click.IsVisible = true;
        Right_Click.IsVisible = true;
        ScrollBar.IsVisible = true;
        CTRL_V.IsVisible = true;
        CTRL_C.IsVisible = true;
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
        Back.IsVisible = true;
        BrightnessSlider.IsVisible = true;
        BrightnessLabel.IsVisible = true;
        ResolutionLabel.IsVisible = true;
        ResolutionPicker.IsVisible = true;
        ChangeResolutionButton.IsVisible = true;
        QuietModeLabel.IsVisible = true;
        QuietModeSwitch.IsVisible = true;
        Remote_Keyboard.IsVisible = false;
        File_Manager.IsVisible = false;
        System.IsVisible = false;
        Spy_Mode.IsVisible = false;
        Browser.IsVisible = false;
        Display_Functions.IsVisible = false;
        Remote_Cursor.IsVisible = false;
        Media_Control.IsVisible = false;
        UpdateBrightnessSlider();
    }
    void Remote_Keyboard_Clicked (object sender, EventArgs e)
    {
        Back.IsVisible = true;
        ClipboardEntry.IsVisible = true;
        SendTextToClipboard.IsVisible = true;
        Call_Keyboard.IsVisible = true;
        Display_Functions.IsVisible = false;
        Remote_Cursor.IsVisible = false;
        Media_Control.IsVisible = false;
        Remote_Keyboard.IsVisible = false;
        File_Manager.IsVisible = false;
        System.IsVisible = false;
        Spy_Mode.IsVisible = false;
        Browser.IsVisible = false;
    }

    void Media_Control_Clicked(object sender, EventArgs e)
    {
        Back.IsVisible = true;
        Media_Controls.IsVisible = true;
        Remote_Cursor.IsVisible = false;
        Display_Functions.IsVisible = false;
        Remote_Keyboard.IsVisible = false;
        Media_Control.IsVisible = false;
        File_Manager.IsVisible = false;
        System.IsVisible = false;
        Spy_Mode.IsVisible = false;
        Browser.IsVisible = false;

        mediaController.UpdateNowPlaying();
        mediaController.UpdateVolume();

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
        ScrollBar.IsVisible = false;
        CTRL_C.IsVisible = false;
        CTRL_V.IsVisible = false;
        Media_Controls.IsVisible = false;
        BrightnessLabel.IsVisible = false;
        ClipboardEntry.IsVisible = false;
        SendTextToClipboard.IsVisible = false;
        ResolutionLabel.IsVisible = false;
        ResolutionPicker.IsVisible = false;
        ChangeResolutionButton.IsVisible = false;
        QuietModeLabel.IsVisible = false;
        QuietModeSwitch.IsVisible = false;
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

    private async void QuietModeSwitch_Toggled(object sender, ToggledEventArgs e)
    {
        string command = e.Value ? "QUIET_MODE_ON" : "QUIET_MODE_OFF";
        serverConnector.SendCommand(command);
    }

    void BrightnessSlider_Changed(object sender, ValueChangedEventArgs e)
    {
        int brightness = (int)e.NewValue;
        displayController.SetBrightness(brightness);
        BrightnessLabel.Text = $"Brightness = {brightness}";
    }
    async void ChangeResolution_Clicked(object sender, EventArgs e)
    {
        string selected = ResolutionPicker.SelectedItem as string;
        if (!string.IsNullOrEmpty(selected))
        {
            await displayController.SetResolution(selected);

            await DisplayAlert("Resolution Change", "Resolution has been changed.", "OK");
        }
        else
        {
            await DisplayAlert("Error", "Please select a valid resolution.", "OK");
        }
    }

    async void UpdateBrightnessSlider()
    {
        int? brightness = await displayController.GetBrightness();
        if (brightness.HasValue)
        {
            BrightnessSlider.Value = brightness.Value;
            BrightnessLabel.Text = $"Brightness = {brightness.Value}";
        }
    }
    void LeftClick_Clicked(object sender, EventArgs e) => serverConnector.SendCommand("CLICK LEFT");
    void RightClick_Clicked(object sender, EventArgs e) => serverConnector.SendCommand("CLICK RIGHT");
    void CTRL_C_Clicked(object sender, EventArgs e) => serverConnector.SendCommand("CTRL C");
    void CTRL_V_Clicked(object sender, EventArgs e) => serverConnector.SendCommand("CTRL V");
    void PrevTrack_Clicked(object sender, EventArgs e)
    {
        mediaController.PrevTrack();
        WaitForUpdate();
    }
    void PlayPause_Clicked(object sender, EventArgs e) => mediaController.PlayPause();
    void NextTrack_Clicked(object sender, EventArgs e)
    {
        mediaController.NextTrack();
        WaitForUpdate();
    }
    void Mute_Clicked(object sender, EventArgs e) => mediaController.Mute();
    void VolumeSlider_Changed(object sender, ValueChangedEventArgs e) => mediaController.ChangeVolume(e.NewValue);
    public void UpdateNowPlaying(string nowPlaying)
    {
        NowPlayingLabel.Text = $"Now Playing: {nowPlaying}";
    }
    public void UpdateVolume(int volume)
    {
        VolumeSlider.Value = volume;
    }
    public async void WaitForUpdate()
    {
        await Task.Delay(200);
        mediaController.UpdateNowPlaying();
    }
    public async void SendTextToClipboard_Clicked(object sender, EventArgs e)
    {
        string text = ClipboardEntry.Text;
        if (!string.IsNullOrEmpty(text))
        {
            bool success = await keyboardController.SendClipboardText(text);
            if (success)
                await DisplayAlert("Success", "Text copied to PC clipboard!", "OK");
            else
                await DisplayAlert("Error", "Failed to copy text.", "OK");
        }
    }
}
