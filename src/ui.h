#pragma once
#include "api.h"

void uiInit();
void uiBootProgress(int percent, const char* label);
void uiSetupScreen(const char* apName, const char* apPass);
void uiPinScreen(int pos, const int digits[4]);
void uiConnecting(const char* ssid, int attempt = 0);
void uiDashboard(const UsageData& data, unsigned long lastFetchMs, int rssi, int batPct);
void uiDashboardAnim();   // called every loop tick; no-op on non-AMOLED boards
void uiError(const char* title, const char* detail = nullptr);
void uiLockout(int attempts, int maxAttempts, int lockoutSec);
