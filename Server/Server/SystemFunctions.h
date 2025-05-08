#pragma once
#include <pdh.h>
#include <pdhmsg.h>

bool restartComputer();
bool shutdownComputer();
bool lockWorkstation();
bool logoffUser();
bool openTaskManager();
bool showDesktop();
bool altTab();
double getCpuUsage();
double getMemoryUsage();
double getDiskUsage();