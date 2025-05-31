namespace RemoteGod
{
    public partial class App : Application
    {
        private readonly ServerConnector serverConnector;
        public App()
        {
            InitializeComponent();

            MainPage = new AppShell();
            serverConnector = new ServerConnector();
            AppDomain.CurrentDomain.ProcessExit += OnProcessExit;
        }

        private void OnProcessExit(object? sender, EventArgs e)
        {
            try
            {
                serverConnector.SendCommand("DISCONNECTED");
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error sending exit: " + ex.Message);
            }
        }
    }
}
