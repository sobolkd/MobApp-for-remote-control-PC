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
    private readonly SystemController systemController;

    private double cursorX = 500;
    private double cursorY = 300;
    public string SelectedAudioOutput { get; set; }
    public MainPage()
    {
        InitializeComponent();
        serverConnector = new ServerConnector();
        displayController = new DisplayController(serverConnector);
        keyboardController = new KeyboardController(serverConnector);
        _scrollController = new ScrollController(serverConnector);
        mediaController = new MediaController(serverConnector, this);
        systemController = new SystemController(serverConnector, this);
        serverConnector.OnIpReceived += ip =>
        {
            MainThread.BeginInvokeOnMainThread(() =>
            {
                Title = ip;
                ShowIpLabel.Text = "PC IP: " + ip;
            });
        };

        serverConnector.OnCommandFailed += message =>
        {
            MainThread.BeginInvokeOnMainThread(() =>
            {
                Title = message;
                ShowIpLabel.Text = "PC IP:" + message + "%";
            });
        };

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
        OrientationPicker.IsVisible = true;
        ChangeOrientationText.IsVisible = true;
        Remote_Keyboard.IsVisible = false;
        File_Manager.IsVisible = false;
        System.IsVisible = false;
        Spy_Mode.IsVisible = false;
        Display_Functions.IsVisible = false;
        Remote_Cursor.IsVisible = false;
        Media_Control.IsVisible = false;
        SleepModeLabel.IsVisible= true;
        SleepModeSwitch.IsVisible= true;
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

        mediaController.UpdateNowPlaying();
        mediaController.UpdateVolume();
        mediaController.NameAudioOutputs();

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
    }
    void System_Clicked(object sender, EventArgs e)
    {
        Back.IsVisible = true;
        SystemStack.IsVisible = true;
        Remote_Cursor.IsVisible = false;
        Display_Functions.IsVisible = false;
        Remote_Keyboard.IsVisible = false;
        Media_Control.IsVisible = false;
        File_Manager.IsVisible = false;
        System.IsVisible = false;
        Spy_Mode.IsVisible = false;
         _ = UpdateDataPc();
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
        OrientationPicker.IsVisible = false;
        ChangeOrientationText.IsVisible = false;
        SleepModeLabel.IsVisible = false;
        SleepModeSwitch.IsVisible = false;
        SystemStack.IsVisible = false;
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
    private void OrientationPicker_SelectedIndexChanged(object sender, EventArgs e)
    {
        var picker = sender as Picker;
        if (picker.SelectedIndex == -1)
            return;

        string selected = picker.Items[picker.SelectedIndex];

        string command = selected switch
        {
            "0°" => "CHANGE_ORIENTATION 0",
            "90°" => "CHANGE_ORIENTATION 90",
            "180°" => "CHANGE_ORIENTATION 180",
            "270°" => "CHANGE_ORIENTATION 270",
            _ => null
        };

        if (command != null)
        {
            serverConnector.SendCommand(command);
        }
    }
    private void SleepModeSwitch_Toggled(object sender, ToggledEventArgs e)
    {
        string command = e.Value switch
        {
            true => "SLEEP_MODE_ON",
            false => "SLEEP_MODE_OFF"
        };
        serverConnector.SendCommand(command);
    }
    public void GetAudioOutputs(string audioOutputs)
    {
        if (string.IsNullOrWhiteSpace(audioOutputs))
            return;

        var outputs = audioOutputs.Split(new[] { "\r\n", "\n" }, StringSplitOptions.RemoveEmptyEntries).ToList();

        AudioOutputsStack.Children.Clear();

        foreach (var output in outputs)
        {
            var label = new Label
            {
                Text = output,
                FontSize = 16,
                TextColor = Colors.Black,
                Padding = new Thickness(10)
            };

            var frame = new Frame
            {
                Content = label,
                BackgroundColor = Colors.LightGray,
                CornerRadius = 10,
                Margin = new Thickness(5)
            };
            var tapGestureRecognizer = new TapGestureRecognizer();
            tapGestureRecognizer.Tapped += async (sender, e) => await ChangeAudioOutput(output);

            frame.GestureRecognizers.Add(tapGestureRecognizer);

            AudioOutputsStack.Children.Add(frame);
        }
    }
    private async Task ChangeAudioOutput(string audioOutput)
    {
        if (string.IsNullOrEmpty(audioOutput))
        {
            return;
        }
        var command = "CHANGE_AUDIO_OUTPUTS " + audioOutput;
        serverConnector.SendCommand(command);
    }
    private void RestartPC_Clicked(object sender, EventArgs e)
    {
        serverConnector.SendCommand("RESTART_PC");
    }
    private void ShutDownPC_Clicked(object sender, EventArgs e)
    {
        serverConnector.SendCommand("SHUT_DOWN_PC");
    }
    private void LockScreen_Clicked(object sender, EventArgs e)
    {
        serverConnector.SendCommand("LOCK_SCREEN_PC");
    }
    private void EndSession_Clicked(object sender, EventArgs e)
    {
        serverConnector.SendCommand("END_SESSION");
    }
    private void CallTaskManager_Clicked(object sender, EventArgs e)
    {
        serverConnector.SendCommand("CALL_TASK_MANAGER");
    }
    private void ShowDesktop_Clicked(object sender, EventArgs e)
    {
        serverConnector.SendCommand("SHOW_DESKTOP");
    }
    private void AltTab_Clicked(object sender, EventArgs e)
    {
        serverConnector.SendCommand("ALT_TAB");
    }
    public void UpdateCpuUsage(string cpuUsage)
    {
        CPUUsageLabel.Text = "Curent CPU usage: " + cpuUsage + "%";
    }
    public void UpdateMemoryUsage(string memoryUsage)
    {
        MemoryUsageLabel.Text = "Curent memory usage: " + memoryUsage + "%";
    }
    public void UpdateDiskUsage(string diskUsage)
    {
        DiskUsageLabel.Text = "Disk space occupied: " + diskUsage + "%";
    }
    public async Task UpdateDataPc()
    {
        while (SystemStack.IsVisible)
        {
            await systemController.GetCPUUsage();
            await systemController.GetMemoryUsage();
            await systemController.GetDiskUsage();
            await Task.Delay(1000);
        }
    }
}
