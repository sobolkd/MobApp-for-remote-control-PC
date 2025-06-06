﻿using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;
using System.Windows.Input;
using RemoteGod.Models;
using Microsoft.Maui.Controls;
using System.Text.RegularExpressions;

namespace RemoteGod.ViewModels
{
    public class FileExplorerViewModel : INotifyPropertyChanged
    {
        public ObservableCollection<FileItem> FileItems { get; } = new();

        private string _currentPath;
        public string CurrentPath
        {
            get => _currentPath;
            set
            {
                if (_currentPath != value)
                {
                    _currentPath = value;
                    OnPropertyChanged();
                }
            }
        }

        private Func<string, Task<string>> _sendCommandAsync;

        private string? _copySourcePath;
        public string? CopySourcePath
        {
            get => _copySourcePath;
            set
            {
                if (_copySourcePath != value)
                {
                    _copySourcePath = value;
                    OnPropertyChanged();
                    OnPropertyChanged(nameof(CanPaste));
                }
            }
        }

        private string? _moveSourcePath;
        public string? MoveSourcePath
        {
            get => _moveSourcePath;
            set
            {
                if (_moveSourcePath != value)
                {
                    _moveSourcePath = value;
                    OnPropertyChanged();
                    OnPropertyChanged(nameof(CanPaste));
                }
            }
        }
        public bool CanPaste => !string.IsNullOrEmpty(CopySourcePath) || !string.IsNullOrEmpty(MoveSourcePath);

        public ICommand NavigateUpCommand { get; }
        public ICommand RefreshCommand { get; }
        public ICommand PasteCommand { get; }

        public FileExplorerViewModel()
        {
            NavigateUpCommand = new Command(async () => await NavigateUpAsync());
            RefreshCommand = new Command(async () => await RefreshAsync());
            PasteCommand = new Command(async () => await PasteAsync());
        }

        public async Task LoadDriversAsync(Func<string, Task<string>> sendCommandAsync)
        {
            _sendCommandAsync = sendCommandAsync;

            try
            {
                string response = await sendCommandAsync("GET_DRIVERLIST");
                string[] lines = response.Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);
                FileItems.Clear();

                foreach (var line in lines)
                {
                    if (string.IsNullOrWhiteSpace(line))
                        continue;

                    var cleanPath = line.Replace("[DRIVE] ", "").Trim();

                    FileItems.Add(new FileItem
                    {
                        Name = cleanPath,
                        Path = cleanPath,
                        IsDirectory = true
                    });
                }

                CurrentPath = "";
            }
            catch (Exception)
            {
                FileItems.Clear();
            }
        }

        public async Task LoadDirectoryAsync(string path, Func<string, Task<string>> sendCommandAsync)
        {
            if (string.IsNullOrWhiteSpace(path))
            {
                FileItems.Clear();
                CurrentPath = string.Empty;
                return;
            }

            _sendCommandAsync = sendCommandAsync;

            string cleanPath = path.Replace("[DIR] ", "").Replace("[DRIVE] ", "").Trim();
            cleanPath = cleanPath.Replace(@"\\", @"\").Replace(@"//", @"/").Replace(@"\/", @"\").Replace(@"/\", @"\");

            CurrentPath = cleanPath;

            try
            {
                string command = $"CD_{cleanPath}";
                string response = await sendCommandAsync(command);

                string[] lines = response.Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);
                LoadFromServerResponse(lines);
            }
            catch (Exception)
            {
                FileItems.Clear();
            }
        }

        public void LoadFromServerResponse(string[] lines)
        {
            FileItems.Clear();

            foreach (var line in lines)
            {
                if (string.IsNullOrWhiteSpace(line))
                    continue;

                if (line.StartsWith("[DIR] "))
                {
                    string name = line.Substring(6).Trim();
                    var cleanName = name.Replace("[DIR] ", "").Replace("[DRIVE] ", "").Trim();

                    FileItems.Add(new FileItem
                    {
                        Name = cleanName,
                        IsDirectory = true,
                        Path = Path.Combine(CurrentPath ?? "", cleanName)
                    });
                }
                else if (line.StartsWith("[FILE] "))
                {
                    string name = line.Substring(7).Trim();

                    FileItems.Add(new FileItem
                    {
                        Name = name,
                        IsDirectory = false,
                        Path = Path.Combine(CurrentPath ?? "", name)
                    });
                }
            }
        }

        private async Task NavigateUpAsync()
        {
            if (string.IsNullOrWhiteSpace(CurrentPath))
            {
                await LoadDriversAsync(_sendCommandAsync);
                return;
            }

            var parent = Directory.GetParent(CurrentPath)?.FullName;

            if (Regex.IsMatch(CurrentPath, @"^[A-Z]:\\?$", RegexOptions.IgnoreCase))
            {
                await LoadDriversAsync(_sendCommandAsync);
                return;
            }
            if (!string.IsNullOrEmpty(parent))
            {
                await LoadDirectoryAsync(parent, _sendCommandAsync);
                return;
            }
            await LoadDriversAsync(_sendCommandAsync);
        }

        public async Task RefreshAsync()
        {
            if (!string.IsNullOrEmpty(CurrentPath) && _sendCommandAsync != null)
            {
                await LoadDirectoryAsync(CurrentPath, _sendCommandAsync);
            }
        }

        private async Task PasteAsync()
        {
            if (string.IsNullOrEmpty(CurrentPath))
                return;

            if (!string.IsNullOrEmpty(MoveSourcePath))
            {
                string command = $"MOVE_{MoveSourcePath}_{CurrentPath}";
                string response = await _sendCommandAsync(command);

                await Application.Current.MainPage.DisplayAlert("Move", response, "OK");

                MoveSourcePath = null;
            }
            else if (!string.IsNullOrEmpty(CopySourcePath))
            {
                string command = $"COPY_{CopySourcePath}_{CurrentPath}";
                string response = await _sendCommandAsync(command);

                await Application.Current.MainPage.DisplayAlert("Paste", response, "OK");

                CopySourcePath = null;
            }

            await LoadDirectoryAsync(CurrentPath, _sendCommandAsync);
        }

        public void CancelPasteMove()
        {
            CopySourcePath = null;
            MoveSourcePath = null;
        }


        public event PropertyChangedEventHandler PropertyChanged;
        protected void OnPropertyChanged([CallerMemberName] string name = "") =>
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
    }
}
