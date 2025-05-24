using System;
using System.IO;
using System.Net.Sockets;
using System.Threading.Tasks;
using Android.Content;
using Android.Provider;
using Microsoft.Maui.ApplicationModel;
using Application = Android.App.Application;

namespace RemoteGod
{
    public class ScreenshotReceiverService
    {
        private const int Port = 7778;

        public async Task DownloadScreenshotFromServer(string serverIp)
        {
            try
            {
                using var client = new TcpClient();
                await client.ConnectAsync(serverIp, Port);

                using var stream = client.GetStream();

                // Читаємо розмір
                byte[] sizeBuffer = new byte[4];
                if (!await ReadExactAsync(stream, sizeBuffer, 4)) return;

                int fileSize = BitConverter.ToInt32(sizeBuffer, 0);
                byte[] imageData = new byte[fileSize];

                // Читаємо файл
                if (!await ReadExactAsync(stream, imageData, fileSize)) return;

                await SaveImageToGallery(imageData);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error downloading screenshot: {ex.Message}");
            }
        }

        private async Task SaveImageToGallery(byte[] imageData)
        {
            var filename = $"screenshot_{DateTime.Now:yyyyMMdd_HHmmss}.jpg";

            var values = new ContentValues();
            values.Put(MediaStore.IMediaColumns.DisplayName, filename);
            values.Put(MediaStore.IMediaColumns.MimeType, "image/jpeg");
            values.Put(MediaStore.IMediaColumns.RelativePath, "Pictures/RemoteScreenshots");

            var uri = Application.Context.ContentResolver.Insert(MediaStore.Images.Media.ExternalContentUri, values);
            if (uri == null) return;

            using var outputStream = Application.Context.ContentResolver.OpenOutputStream(uri);
            await outputStream.WriteAsync(imageData, 0, imageData.Length);
        }

        private static async Task<bool> ReadExactAsync(Stream stream, byte[] buffer, int size)
        {
            int offset = 0;
            while (offset < size)
            {
                int bytesRead = await stream.ReadAsync(buffer, offset, size - offset);
                if (bytesRead == 0) return false;
                offset += bytesRead;
            }
            return true;
        }
    }
}
