namespace RemoteGod;

public class KeyboardController
{
    private readonly ServerConnector serverConnector;

    public KeyboardController(ServerConnector connector)
    {
        serverConnector = connector;
    }

    public void OpenOnScreenKeyboard()
    {
        serverConnector.SendCommand("OPEN_KEYBOARD");
    }

    public async Task<bool> SendClipboardText(string text)
    {
        string response = await serverConnector.SendCommandWithResponse($"SET_CLIPBOARD {text}");
        return response == "OK";
    }

}
