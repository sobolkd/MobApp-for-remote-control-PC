# Remote Control Application (C++ Server + .NET MAUI Mobile App)

This project consists of a **C++ server** running on a Windows machine and a **.NET MAUI mobile app** that allows you to 
remotely control various functions of your PC. The app connects to the server via a local network and provides control
over multiple features such as the cursor, media, files, and many more.

## Features

- **Remote Mouse Control**: Move and click the mouse cursor remotely.
- **Media Control**: Play, pause, and adjust the volume of media.
- **File Management**: Browse and manage files on the PC.
- **Additional Functions**: Includes a variety of smaller functions for advanced control.

## Technologies Used

- **C++**: Server-side application running on Windows.
- **.NET MAUI**: Mobile application (Android) for controlling the PC.
- **Windows Sockets (Winsock)**: Communication protocol for connecting the mobile app and the C++ server.

## Requirements

### Server (C++)

- Windows operating system
- Visual Studio or any C++ compatible IDE
- Winsock library (for socket communication)

### Mobile App (.NET MAUI)

- .NET 8
- Visual Studio with .NET MAUI support enabled

## Installation

1. Clone this repository.

   ```bash
   git clone https://github.com/sobolkd/MobApp-for-remote-control-PC.git
