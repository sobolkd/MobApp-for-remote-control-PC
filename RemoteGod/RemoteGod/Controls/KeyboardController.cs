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
}
