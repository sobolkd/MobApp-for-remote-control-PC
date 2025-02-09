using System.Net;
using System.Net.Sockets;
using System.Text;

namespace RemoteGod;

public partial class MainPage : ContentPage
{
    const int UdpPort = 8888;
    const int TcpPort = 9999;
    const string Keyword = "HELLO_SERVER";
    string? serverIp = null;

    public MainPage()
    {
        InitializeComponent();
        Task.Run(ListenForServer);
    }

    async void ListenForServer()
    {
        using var udpClient = new UdpClient(UdpPort);
        while (true)
        {
            var result = await udpClient.ReceiveAsync();
            string message = Encoding.UTF8.GetString(result.Buffer);
            if (message == Keyword)
            {
                serverIp = result.RemoteEndPoint.Address.ToString();
                string receivedIp = await GetServerIp(serverIp);
                MainThread.BeginInvokeOnMainThread(() => IpLabel.Text = receivedIp);
                break;
            }
        }
    }

    async Task<string> GetServerIp(string serverIp)
    {
        try
        {
            using var tcpClient = new TcpClient();
            await tcpClient.ConnectAsync(serverIp, TcpPort);
            using var stream = tcpClient.GetStream();
            byte[] buffer = new byte[256];
            int bytesRead = await stream.ReadAsync(buffer, 0, buffer.Length);
            return Encoding.UTF8.GetString(buffer, 0, bytesRead);
        }
        catch
        {
            return "Failed to connect";
        }
    }

    async void SendCommand(string command)
    {
        if (serverIp == null) return;
        try
        {
            using var tcpClient = new TcpClient();
            await tcpClient.ConnectAsync(serverIp, TcpPort);
            using var stream = tcpClient.GetStream();
            byte[] data = Encoding.UTF8.GetBytes(command);
            await stream.WriteAsync(data, 0, data.Length);
        }
        catch
        {
            MainThread.BeginInvokeOnMainThread(() => IpLabel.Text = "Command failed");
        }
    }

    void Remote_Cursor_Clicked(object sender, EventArgs e)
    {
        JoystickView.IsVisible = true;
        Left_Click.IsVisible = true;
        Right_Click.IsVisible = true;
        Remote_Cursor.IsVisible = false;
    }

    private double cursorX = 500;
    private double cursorY = 300;

    void Joystick_Moved(object sender, (double X, double Y) e)
    {
        double scaleFactor = 10;

        cursorX += e.X * scaleFactor;
        cursorY += e.Y * scaleFactor;

        int intCursorX = (int)cursorX;
        int intCursorY = (int)cursorY;

        IpLabel.Text = $"{intCursorX} + {intCursorY}";

        cursorX = Math.Clamp(intCursorX, 0, 1920);  // Max values
        cursorY = Math.Clamp(intCursorY, 0, 1080);  // Max values

        SendCommand($"MOVE {intCursorX} {intCursorY}");
    }






    void LeftClick_Clicked(object sender, EventArgs e) => SendCommand("CLICK LEFT");

    void RightClick_Clicked(object sender, EventArgs e) => SendCommand("CLICK RIGHT");
}
