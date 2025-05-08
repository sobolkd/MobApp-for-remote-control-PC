using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RemoteGod
{
    public class SystemController
    {
        private readonly ServerConnector serverConnector;
        private readonly MainPage mainPage;

        public SystemController(ServerConnector connector, MainPage page)
        {
            serverConnector = connector;
            mainPage = page;
        }
        public async Task GetCPUUsage()
        {
            string responce = await serverConnector.SendCommandWithResponse("CPU_USAGE");
            mainPage.UpdateCpuUsage(responce);
        }
        public async Task GetMemoryUsage()
        {
            string responce = await serverConnector.SendCommandWithResponse("MEMORY_USAGE");
            mainPage.UpdateMemoryUsage(responce);
        }
        public async Task GetDiskUsage()
        {
            string responce = await serverConnector.SendCommandWithResponse("DISK_USAGE");
            mainPage.UpdateDiskUsage(responce);
        }
    }
}
