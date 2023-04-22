#include <tvision/Desktop.h>
#include <tvision/Label.h>
#include <tvision/MessageBox.h>
#include <tvision/StaticText.h>

const char* MessageBox::yesText = "~Y~es";
const char* MessageBox::noText = "~N~o";
const char* MessageBox::okText = "O~K~";
const char* MessageBox::cancelText = "~C~ancel";
const char* MessageBox::warningText = "Warning";
const char* MessageBox::errorText = "Error";
const char* MessageBox::informationText = "Information";
const char* MessageBox::confirmText = "Confirm";

const char* MessageBox::buttonName[]
    = { MessageBox::yesText, MessageBox::noText, MessageBox::okText, MessageBox::cancelText };

static ushort commands[] = { cmYes, cmNo, cmOK, cmCancel };

const char* MessageBox::Titles[] = { MessageBox::warningText, MessageBox::errorText,
    MessageBox::informationText, MessageBox::confirmText };

ushort MessageBox::messageBoxRect(const TRect& r, std::string_view msg, ushort aOptions) noexcept
{
    TDialog* dialog;
    short i, x, buttonCount;
    TView* buttonList[5];
    ushort ccode;

    dialog = new TDialog(r, Titles[aOptions & 0x3]);

    dialog->insert(new TStaticText(TRect(3, 2, dialog->size.x - 2, dialog->size.y - 3), msg));

    for (i = 0, x = -2, buttonCount = 0; i < 4; i++) {
        if ((aOptions & (0x0100 << i)) != 0) {
            buttonList[buttonCount] = new TButton(
                TRect(0, 0, 10, 2), buttonName[i], commands[i], TButton::Flags::bfNormal);
            x += buttonList[buttonCount++]->size.x + 2;
        }
    }

    x = (dialog->size.x - x) / 2;

    for (i = 0; i < buttonCount; i++) {
        dialog->insert(buttonList[i]);
        buttonList[i]->moveTo(x, dialog->size.y - 3);
        x += buttonList[i]->size.x + 2;
    }

    dialog->selectNext(false);

    ccode = TProgram::application->execView(dialog);

    TObject::destroy(dialog);

    return ccode;
}

ushort MessageBox::messageBoxRect(const TRect& r, ushort aOptions, const char* fmt, ...) noexcept
{
    va_list argptr;
    va_start(argptr, fmt);

    char msg[256];
    vsnprintf(msg, 256, fmt, argptr);

    va_end(argptr);

    return MessageBox::messageBoxRect(r, msg, aOptions);
}

static TRect makeRect(std::string_view text)
{
    TRect r(0, 0, 40, 9);

    int width = strwidth(text);
    while (width > (r.b.x - 7) * (r.b.y - 6))
        ++r.b.y;

    r.move((TProgram::deskTop->size.x - r.b.x) / 2, (TProgram::deskTop->size.y - r.b.y) / 2);
    return r;
}

ushort MessageBox::messageBox(std::string_view msg, ushort aOptions) noexcept
{
    return messageBoxRect(makeRect(msg), msg, aOptions);
}

ushort MessageBox::messageBox(unsigned aOptions, const char* fmt, ...) noexcept
{
    va_list argptr;
    va_start(argptr, fmt);

    char msg[256];
    vsnprintf(msg, 256, fmt, argptr);

    va_end(argptr);

    return messageBoxRect(makeRect(msg), msg, aOptions);
}

ushort MessageBox::inputBox(
    std::string_view Title, std::string_view aLabel, char* s, uchar limit) noexcept
{
    TRect r(0, 0, 60, 8);
    r.move((TProgram::deskTop->size.x - r.b.x) / 2, (TProgram::deskTop->size.y - r.b.y) / 2);
    return inputBoxRect(r, Title, aLabel, s, limit);
}

ushort MessageBox::inputBoxRect(const TRect& bounds, std::string_view Title,
    std::string_view aLabel, char* s, uchar limit) noexcept
{
    TDialog* dialog;
    TView* control;
    TRect r;
    ushort c;

    dialog = new TDialog(bounds, Title);

    r = TRect(4 + aLabel.size(), 2, dialog->size.x - 3, 3);
    control = new TInputLine(r, limit);
    dialog->insert(control);

    r = TRect(2, 2, 3 + aLabel.size(), 3);
    dialog->insert(new TLabel(r, aLabel, control));

    r = TRect(dialog->size.x - 24, dialog->size.y - 4, dialog->size.x - 14, dialog->size.y - 2);
    dialog->insert(new TButton(r, MessageBox::okText, cmOK, TButton::Flags::bfDefault));

    r.a.x += 12;
    r.b.x += 12;
    dialog->insert(new TButton(r, MessageBox::cancelText, cmCancel, TButton::Flags::bfNormal));

    r.a.x += 12;
    r.b.x += 12;
    dialog->selectNext(false);
    dialog->setData(s);
    c = TProgram::application->execView(dialog);
    if (c != cmCancel)
        dialog->getData(s);
    TObject::destroy(dialog);
    return c;
}

ushort MessageBox::error(const char* message) noexcept
{
    return messageBox(message, mfError | mfOKButton);
}

ushort MessageBox::warning(const char* message) noexcept
{
    return messageBox(message, mfWarning | mfOKButton);
}

ushort MessageBox::info(const char* message) noexcept
{
    return messageBox(message, mfInformation | mfOKButton);
}

ushort MessageBox::confirm(const TRect& r, const char* message) noexcept
{
    return messageBoxRect(r, message, mfConfirmation | mfYesNoCancel);
}

ushort MessageBox::confirm(const char* message) noexcept
{
    return messageBox(message, mfConfirmation | mfYesNoCancel);
}
