#include <iostream>
#include <tvision/HardwareInfo.h>

BOOL THardwareInfo::insertState = true;
THardwareInfo::PlatformType THardwareInfo::platform = THardwareInfo::plDPMI32;
HANDLE THardwareInfo::consoleHandle[3];
DWORD THardwareInfo::consoleMode;
DWORD THardwareInfo::pendingEvent;
INPUT_RECORD THardwareInfo::irBuffer;
CONSOLE_CURSOR_INFO THardwareInfo::crInfo;
CONSOLE_SCREEN_BUFFER_INFO THardwareInfo::sbInfo;

const ushort THardwareInfo::NormalCvt[89] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0x8500, 0x8600 };

const ushort THardwareInfo::ShiftCvt[89]
    = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0F00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          0x5400, 0x5500, 0x5600, 0x5700, 0x5800, 0x5900, 0x5A00, 0x5B00, 0x5C00, 0x5D00, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0500, 0x0700, 0, 0, 0, 0x8700, 0x8800 };

const ushort THardwareInfo::CtrlCvt[89] = { 0, 0, 0x0231, 0x0332, 0x0433, 0x0534, 0x0635, 0x0736,
    0x0837, 0x0938, 0x0A39, 0x0B30, 0, 0, 0, 0x9400, 0x0011, 0x0017, 0x0005, 0x0012, 0x0014, 0x0019,
    0x0015, 0x0009, 0x000F, 0x0010, 0, 0, 0, 0, 0x0001, 0x0013, 0x0004, 0x0006, 0x0007, 0x0008,
    0x000A, 0x000B, 0x000C, 0, 0, 0, 0, 0, 0x001A, 0x0018, 0x0003, 0x0016, 0x0002, 0x000E, 0x000D,
    0, 0, 0x352F, 0, 0x372A, 0, 0, 0, 0x5E00, 0x5F00, 0x6000, 0x6100, 0x6200, 0x6300, 0x6400,
    0x6500, 0x6600, 0x6700, 0, 0, 0x7700, 0x8D00, 0x8400, 0x4A2D, 0x7300, 0, 0x7400, 0x4E2B, 0x7500,
    0x9100, 0x7600, 0x0400, 0x0600, 0, 0, 0, 0x8900, 0x8A00 };

const ushort THardwareInfo::AltCvt[89] = { 0, 0x0100, 0x7800, 0x7900, 0x7A00, 0x7B00, 0x7C00,
    0x7D00, 0x7E00, 0x7F00, 0x8000, 0x8100, 0x8200, 0x8300, 0x0E00, 0xA500, 0x1000, 0x1100, 0x1200,
    0x1300, 0x1400, 0x1500, 0x1600, 0x1700, 0x1800, 0x1900, 0, 0, 0xA600, 0, 0x1E00, 0x1F00, 0x2000,
    0x2100, 0x2200, 0x2300, 0x2400, 0x2500, 0x2600, 0, 0, 0, 0, 0, 0x2C00, 0x2D00, 0x2E00, 0x2F00,
    0x3000, 0x3100, 0x3200, 0, 0, 0, 0, 0, 0, 0x0200, 0, 0x6800, 0x6900, 0x6A00, 0x6B00, 0x6C00,
    0x6D00, 0x6E00, 0x6F00, 0x7000, 0x7100, 0, 0, 0x9700, 0x9800, 0x9900, 0x8200, 0x9B00, 0, 0x9D00,
    0, 0x9F00, 0xA000, 0xA100, 0xA200, 0xA300, 0, 0, 0, 0x8B00, 0x8C00 };

uint32_t THardwareInfo::getTickCount() noexcept
{
    // To change units from ms to clock ticks.
    //   X ms * 1s/1000ms * 18.2ticks/s = X/55 ticks, roughly.
    return GetTickCount() / 55;
}

BOOL __stdcall THardwareInfo::ctrlBreakHandler(DWORD dwCtrlType) noexcept
{
    if (dwCtrlType == CTRL_C_EVENT || dwCtrlType == CTRL_BREAK_EVENT) {
        TSystemError::ctrlBreakHit = true;
        return TRUE;
    } else
        return FALSE; // Don't handle 'CLOSE', 'LOGOFF' or 'SHUTDOWN' events.
}
