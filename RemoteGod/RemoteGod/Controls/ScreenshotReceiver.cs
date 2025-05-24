using System;
using System.IO;
using System.Net.Sockets;
using System.Threading.Tasks;
using Android.Content;
using Android.Provider;
using Microsoft.Maui.Controls;
using Microsoft.Maui.ApplicationModel;

namespace RemoteGod
{
    public class ScreenshotReceiver
    {
        public async Task ReceiveAndSaveScreenshotAsync(string serverIp, int port, Page uiPage)
        {
            try
            {
                using var client = new TcpClient();
                var connectTask = client.ConnectAsync(serverIp, port);
                if (await Task.WhenAny(connectTask, Task.Delay(5000)) != connectTask)
                {
                    throw new TimeoutException("Timeout connecting to server");
                }

                await MainThread.InvokeOnMainThreadAsync(() =>
                    uiPage.DisplayAlert("ТУТ", "1:\n", "OK"));

                using NetworkStream stream = client.GetStream();

                byte[] lengthBytes = new byte[4];
                await stream.ReadAsync(lengthBytes, 0, 4);
                int length = BitConverter.ToInt32(lengthBytes, 0);

                byte[] buffer = new byte[length];
                int totalRead = 0;
                while (totalRead < length)
                {
                    int read = await stream.ReadAsync(buffer, totalRead, length - totalRead);
                    if (read == 0) break;
                    totalRead += read;
                }
                await MainThread.InvokeOnMainThreadAsync(() =>
                    uiPage.DisplayAlert("ТУТ", "2:\n", "OK"));

                string screenshotsDir = Path.Combine(
                    Android.OS.Environment.GetExternalStoragePublicDirectory(Android.OS.Environment.DirectoryPictures).AbsolutePath,
                    "Screenshots");

                Directory.CreateDirectory(screenshotsDir);

                string filename = $"screenshot_{DateTime.Now:yyyyMMdd_HHmmss}.bmp";
                string fullPath = Path.Combine(screenshotsDir, filename);

                await File.WriteAllBytesAsync(fullPath, buffer);

                Context context = Android.App.Application.Context;
                Intent mediaScanIntent = new Intent(Intent.ActionMediaScannerScanFile);
                Android.Net.Uri contentUri = Android.Net.Uri.FromFile(new Java.IO.File(fullPath));
                mediaScanIntent.SetData(contentUri);
                context.SendBroadcast(mediaScanIntent);
            }
            catch (Exception ex)
            {
                string message = $"Не вдалося отримати скріншот:\n{ex.Message}";
                if (ex.InnerException != null)
                    message += $"\nДеталі: {ex.InnerException.Message}";

                await MainThread.InvokeOnMainThreadAsync(() =>
                    uiPage.DisplayAlert("Помилка", message, "OK"));
            }

        }

        public static async Task<bool> DownloadAndSaveScreenshotAsync(string ip, int port, string fileName)
        {
            try
            {
                using var client = new TcpClient();
                await client.ConnectAsync(ip, port);
                using NetworkStream stream = client.GetStream();

                using MemoryStream ms = new();
                await stream.CopyToAsync(ms);
                byte[] imageBytes = ms.ToArray();

#if ANDROID
                var contentValues = new ContentValues();
                contentValues.Put(MediaStore.IMediaColumns.DisplayName, fileName);
                contentValues.Put(MediaStore.IMediaColumns.MimeType, "image/bmp");
                contentValues.Put(MediaStore.IMediaColumns.RelativePath, "Pictures/Screenshots");

                var uri = Android.App.Application.Context.ContentResolver.Insert(MediaStore.Images.Media.ExternalContentUri, contentValues);

                using var outputStream = Android.App.Application.Context.ContentResolver.OpenOutputStream(uri!);
                await outputStream.WriteAsync(imageBytes);
                outputStream.Close();
#endif

                return true;
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error downloading or saving screenshot: " + ex.Message);
                return false;
            }
        }
    }
}
