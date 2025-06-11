﻿using System;
using System.Diagnostics;
using Microsoft.Maui.Controls;
using static Android.Print.PrintAttributes;
using RemoteGod.ViewModels;
using RemoteGod.Models;
using AndroidX.Fragment.App.StrictMode;
using System.Text.RegularExpressions;

namespace RemoteGod;

public partial class MainPage : ContentPage
{
    private readonly ServerConnector serverConnector;
    private readonly DisplayController displayController;
    private KeyboardController keyboardController;
    private readonly ScrollController _scrollController;
    private MediaController mediaController;
    private readonly SystemController systemController;
    private FileExplorerViewModel viewModel;
    public static DateTime Now { get; }
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
        viewModel = new FileExplorerViewModel();
        BindingContext = viewModel;
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
        HideAllButtons();
        Back.IsVisible = false;
        serverConnector.StartListening();
    }

    private void OnPanUpdated(object sender, PanUpdatedEventArgs e)
    {
        _scrollController.Scroll(sender, e);
    }
    private void HideAllButtons()
    {
        Back.IsVisible = true;
        Remote_Mouse.IsVisible = false;
        Remote_Keyboard.IsVisible = false;
        Display_Functions.IsVisible = false;
        Media_Control.IsVisible = false;
        File_Manager.IsVisible = false;
        System.IsVisible = false;
    }
    private void ShowAllButtons()
    {
        Back.IsVisible = false;
        Remote_Mouse.IsVisible = true;
        Remote_Keyboard.IsVisible = true;
        Display_Functions.IsVisible = true;
        Media_Control.IsVisible = true;
        File_Manager.IsVisible = true;
        System.IsVisible = true;
    }
    void Remote_Mouse_Clicked(object sender, EventArgs e)
    {
        HideAllButtons();
        RemoteMouse.IsVisible = true;
        //  string time = DateTime.Now.ToString("HH:mm:ss:fff");
        //  serverConnector.SendCommand($"Check_time: {time}");
    }
    void Remote_Keyboard_Clicked(object sender, EventArgs e)
    {
        HideAllButtons();
        RemoteKeyboard.IsVisible = true;
    }
    void Display_Functions_Clicked(object sender, EventArgs e)
    {
        HideAllButtons();
        DisplayFunctions.IsVisible = true;
        UpdateBrightnessSlider();
    }
    void Media_Control_Clicked(object sender, EventArgs e)
    {
        HideAllButtons();
        Media_Controls.IsVisible = true;
        mediaController.UpdateNowPlaying();
        mediaController.UpdateVolume();
        mediaController.NameAudioOutputs();

    }
    private async void File_Manager_Clicked(object sender, EventArgs e)
    {
        HideAllButtons();
        FileExplorerPanel.IsVisible = true;
        await viewModel.LoadDriversAsync(serverConnector.SendCommandWithResponse);
    }
    private async void OnFileSelected(object sender, SelectionChangedEventArgs e)
    {
        if (e.CurrentSelection.Count == 0)
            return;

        var selected = e.CurrentSelection[0] as FileItem;

        ((CollectionView)sender).SelectedItem = null;

        if (selected == null)
            return;

        if (selected.IsDirectory)
        {
            await viewModel.LoadDirectoryAsync(selected.Path, serverConnector.SendCommandWithResponse);
        }
        else
        {
            string action = await Application.Current.MainPage.DisplayActionSheet(
                $"Select an action for {selected.Name}", "Cancel", null,
                "Download", "Copy", "Move", "Delete");

            switch (action)
            {
                case "Download":
                    if (!await serverConnector.EnsureStoragePermissionAsync())
                    {
                        await Application.Current.MainPage.DisplayAlert("Permission", "Storage permission not granted", "OK");
                        return;
                    }

                    string fileName = selected.Name;
                    string downloadsDir = Android.OS.Environment.GetExternalStoragePublicDirectory(Android.OS.Environment.DirectoryDownloads).AbsolutePath;
                    string downloadPath = Path.Combine(downloadsDir, fileName);

                    bool success = await serverConnector.DownloadFileAsync(selected.Path, downloadPath);

                    await Application.Current.MainPage.DisplayAlert(
                        success ? "Success" : "Error",
                        success ? "File downloaded to Downloads" : "Failed to download file",
                        "OK");
                    break;

                case "Copy":
                    viewModel.CopySourcePath = selected.Path;
                    await Application.Current.MainPage.DisplayAlert("Copy", $"Copied '{selected.Name}'. Now navigate to target folder and press Paste.", "OK");
                    break;

                case "Move":
                    viewModel.MoveSourcePath = selected.Path;
                    await Application.Current.MainPage.DisplayAlert("Move", $"Moved '{selected.Name}'. Now navigate to target folder and press Paste.", "OK");
                    break;

                case "Delete":
                    bool confirm = await Application.Current.MainPage.DisplayAlert(
                        "Confirmation",
                        $"Are you sure you want to delete {selected.Name}?",
                        "Yes", "No");
                    if (confirm)
                    {
                        string response = await serverConnector.SendCommandWithResponse($"DELETE_{selected.Path}");
                        await Application.Current.MainPage.DisplayAlert("", response, "OK");

                        await viewModel.RefreshAsync();
                    }
                    break;
            }
        }
    }

    void System_Clicked(object sender, EventArgs e)
    {
        HideAllButtons();
        SystemStack.IsVisible = true;
        _ = UpdateDataPc();
    }
    void Back_Clicked(object sender, EventArgs e)
    {
        RemoteMouse.IsVisible = false;
        RemoteKeyboard.IsVisible = false;
        DisplayFunctions.IsVisible = false;
        Media_Controls.IsVisible = false;
        FileExplorerPanel.IsVisible = false;
        SystemStack.IsVisible = false;
        ShowAllButtons();
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

    private async void MakeScreen_Clicked(object sender, EventArgs e)
    {
        string remotePath = await serverConnector.SendCommandWithResponse("MAKE_SCREEN");

        if (remotePath != null)
        {
            if (!await serverConnector.EnsureStoragePermissionAsync())
            {
                await Application.Current.MainPage.DisplayAlert("Permission", "Storage permission not granted", "OK");
                return;
            }

            string fileName = Path.GetFileName(remotePath);
            string downloadsDir = Android.OS.Environment.GetExternalStoragePublicDirectory(Android.OS.Environment.DirectoryDownloads).AbsolutePath;
            string downloadPath = Path.Combine(downloadsDir, fileName);

            bool success = await serverConnector.DownloadFileAsync(remotePath, downloadPath);

            await Application.Current.MainPage.DisplayAlert(
                success ? "Success" : "Error",
                success ? "File downloaded to Downloads" : "Failed to download file",
                "OK");
        }
        else
        {
            await Application.Current.MainPage.DisplayAlert("Error", "Server did not return a file path", "OK");
        }
    }

    private async void OnPasteClicked(object sender, EventArgs e)
    {
        if (string.IsNullOrEmpty(viewModel.CopySourcePath) && string.IsNullOrEmpty(viewModel.MoveSourcePath))
        {
            await Application.Current.MainPage.DisplayAlert("Error", "Nothing to paste. Copy or Move a file first.", "OK");
            return;
        }

        string destinationPath = viewModel.CurrentPath;

        if (string.IsNullOrEmpty(destinationPath))
        {
            await Application.Current.MainPage.DisplayAlert("Error", "Select a destination folder first.", "OK");
            return;
        }

        string command = null;

        if (!string.IsNullOrEmpty(viewModel.MoveSourcePath))
        {
            command = $"CUT_{viewModel.MoveSourcePath}_{destinationPath}";
        }
        else if (!string.IsNullOrEmpty(viewModel.CopySourcePath))
        {
            command = $"COPY_{viewModel.CopySourcePath}_{destinationPath}";
        }

        if (command == null)
        {
            await Application.Current.MainPage.DisplayAlert("Error", "Invalid paste operation.", "OK");
            return;
        }

        string response = await serverConnector.SendCommandWithResponse(command);

        await Application.Current.MainPage.DisplayAlert("Paste", response, "OK");

        viewModel.CopySourcePath = null;
        viewModel.MoveSourcePath = null;

        await viewModel.RefreshAsync();
    }
    private void OnCancelClicked(object sender, EventArgs e)
    {
        viewModel.CancelPasteMove();
    }

    private async void OnLoginClicked(object sender, EventArgs e)
    {
        LoginErrorLabel.IsVisible = false;

        string login = LoginEntry.Text?.Trim() ?? "";
        string pass = PasswordEntry.Text?.Trim() ?? "";

        if (login == "" || pass == "")
        {
            LoginErrorLabel.Text = "Fill in all fields.";
            LoginErrorLabel.IsVisible = true;
            return;
        }

        if (login.Length < 4)
        {
            LoginErrorLabel.Text = "Login must be at least 4 characters.";
            LoginErrorLabel.IsVisible = true;
            return;
        }

        if (pass.Length < 5)
        {
            LoginErrorLabel.Text = "Password must be at least 5 characters.";
            LoginErrorLabel.IsVisible = true;
            return;
        }

        if (!Regex.IsMatch(login, @"^[a-zA-Z0-9_]+$"))
        {
            LoginErrorLabel.Text = "Login can contain only letters, numbers, and underscores.";
            LoginErrorLabel.IsVisible = true;
            return;
        }

        string response = await serverConnector.SendCommandWithResponse($"LOGIN_{login} {pass}");

        if (response == "LOGIN_OK")
        {
            LoginOverlay.IsVisible = false;
            ShowAllButtons();
        }
        else if (response == "LOGIN_FAILED")
        {
            LoginErrorLabel.Text = "Incorrect login or password";
            LoginErrorLabel.IsVisible = true;
        }
        else
        {
            LoginErrorLabel.Text = "Server connection error";
            LoginErrorLabel.IsVisible = true;
        }
    }
}
