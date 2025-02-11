using System.Diagnostics;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace RemoteGod;

public class ServerConnector
{
    const int UdpPort = 8888;
    const int TcpPort = 9999;
    const string Keyword = "HELLO_SERVER";

    public string? ServerIp { get; private set; }
    public event Action<string>? OnIpReceived;
    public event Action<string>? OnCommandFailed;

    public void StartListening()
    {
        Task.Run(ListenForServer);
    }

    private async void ListenForServer()
    {
        using var udpClient = new UdpClient(UdpPort);
        while (true)
        {
            var result = await udpClient.ReceiveAsync();
            string message = Encoding.UTF8.GetString(result.Buffer);
            Debug.WriteLine($"Received UDP message: {message}");

            if (message == Keyword)
            {
                try
                {
                    ServerIp = result.RemoteEndPoint.Address.ToString();
                    Debug.WriteLine($"Trying to get IP from TCP: {ServerIp}");

                    string receivedIp = await GetServerIp(ServerIp);
                    Debug.WriteLine($"Received Ip message: {receivedIp}");

                    OnIpReceived?.Invoke(receivedIp);
                    break;
                }
                catch (Exception ex)
                {
                    Debug.WriteLine($"Error: {ex}");
                    break;
                }
            }
        }
    }

    private async Task<string> GetServerIp(string serverIp)
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

    public async void SendCommand(string command)
    {
        if (ServerIp == null) return;
        try
        {
            using var tcpClient = new TcpClient();
            await tcpClient.ConnectAsync(ServerIp, TcpPort);
            using var stream = tcpClient.GetStream();
            byte[] data = Encoding.UTF8.GetBytes(command);
            await stream.WriteAsync(data, 0, data.Length);
        }
        catch
        {
            OnCommandFailed?.Invoke("Command failed");
        }
    }
}
