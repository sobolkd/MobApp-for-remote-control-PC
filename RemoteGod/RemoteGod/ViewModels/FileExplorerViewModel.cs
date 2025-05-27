using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;
using System.Windows.Input;
using RemoteGod.Models;
using Microsoft.Maui.Controls;

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

        public ICommand NavigateUpCommand { get; }
        public ICommand RefreshCommand { get; }

        public FileExplorerViewModel()
        {
            NavigateUpCommand = new Command(async () => await NavigateUpAsync());
            RefreshCommand = new Command(async () => await RefreshAsync());
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
            if (path == null)
                return;

            _sendCommandAsync = sendCommandAsync;

            string cleanPath = path.Replace("[DIR] ", "").Replace("[DRIVE] ", "").Trim();
            cleanPath = cleanPath.Replace(@"\\", @"\").Replace(@"//", @"/").Replace(@"\/", @"\").Replace(@"/\", @"\");


            if (string.IsNullOrWhiteSpace(cleanPath))
            {
                await LoadDriversAsync(sendCommandAsync);
                return;
            }

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
                return;

            var parent = Directory.GetParent(CurrentPath)?.FullName;

            if (!string.IsNullOrEmpty(parent))
            {
                await LoadDirectoryAsync(parent, _sendCommandAsync);
            }
            else
            {
                await LoadDriversAsync(_sendCommandAsync);
            }
        }

        private async Task RefreshAsync()
        {
            if (!string.IsNullOrEmpty(CurrentPath) && _sendCommandAsync != null)
            {
                await LoadDirectoryAsync(CurrentPath, _sendCommandAsync);
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        protected void OnPropertyChanged([CallerMemberName] string name = "") =>
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(name));
    }
}
