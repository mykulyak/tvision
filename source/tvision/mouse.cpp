#include <tvision/event.h>

uchar THWMouse::buttonCount = 0;
bool THWMouse::handlerInstalled = false;

THWMouse::THWMouse() noexcept { resume(); }

void THWMouse::resume() noexcept
{
    buttonCount = THardwareInfo::getButtonCount();
    show();
}

THWMouse::~THWMouse() { suspend(); }

void THWMouse::suspend() noexcept
{
    hide();
    buttonCount = 0;
}

void THWMouse::show() noexcept { THardwareInfo::cursorOn(); }

void THWMouse::hide() noexcept { THardwareInfo::cursorOff(); }

void THWMouse::setRange(ushort rx, ushort ry) noexcept { }

void THWMouse::getEvent(MouseEventType& me) noexcept
{
    me.buttons = 0;
    me.wheel = 0;
    me.where.x = 0;
    me.where.y = 0;
    me.eventFlags = 0;
}

TMouse::TMouse() noexcept
{
    //    show();
}

TMouse::~TMouse()
{
    //    hide();
}
