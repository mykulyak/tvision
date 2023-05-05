#ifndef TVision_MsgBox_h
#define TVision_MsgBox_h

#include <string>
#include <string_view>

class TRect;

class MessageBox {
public:
    enum Flags {
        mfWarning = 0x0000, // Display a Warning box
        mfError = 0x0001, // Dispaly a Error box
        mfInformation = 0x0002, // Display an Information Box
        mfConfirmation = 0x0003, // Display a Confirmation Box

        mfYesButton = 0x0100, // Put a Yes button into the dialog
        mfNoButton = 0x0200, // Put a No button into the dialog
        mfOKButton = 0x0400, // Put an OK button into the dialog
        mfCancelButton = 0x0800, // Put a Cancel button into the dialog

        // Standard Yes, No, Cancel dialog
        mfYesNoCancel = mfYesButton | mfNoButton | mfCancelButton,
        // Standard OK, Cancel dialog
        mfOKCancel = mfOKButton | mfCancelButton,
    };

    static ushort error(const char* message) noexcept;
    static ushort error(const std::string& message) noexcept { return error(message.c_str()); }

    static ushort warning(const char* message) noexcept;
    static ushort warning(const std::string& message) noexcept { return warning(message.c_str()); }

    static ushort info(const char* message) noexcept;
    static ushort info(const std::string& message) noexcept { return info(message.c_str()); }

    static ushort confirm(const TRect& r, const char* message) noexcept;
    static ushort confirm(const TRect& r, const std::string& message) noexcept
    {
        return confirm(r, message.c_str());
    }

    static ushort confirm(const char* message) noexcept;
    static ushort confirm(const std::string& message) noexcept { return confirm(message.c_str()); }

protected:
    static ushort messageBox(std::string_view msg, ushort aOptions) noexcept;
    static ushort messageBox(unsigned aOptions, const char* fmt, ...) noexcept;
    static ushort messageBoxRect(const TRect& r, ushort aOptions, const char* fmt, ...) noexcept;
    static ushort messageBoxRect(const TRect& r, std::string_view msg, ushort aOptions) noexcept;
    static ushort inputBox(
        const std::string& aTitle, const std::string& aLabel, char* s, uchar limit) noexcept;
    static ushort inputBoxRect(const TRect& bounds, const std::string& aTitle,
        const std::string& aLabel, char* s, uchar limit) noexcept;

private:
    static const char* yesText;
    static const char* noText;
    static const char* okText;
    static const char* cancelText;
    static const char* warningText;
    static const char* errorText;
    static const char* informationText;
    static const char* confirmText;

    static const char* buttonName[];
    static const char* Titles[];
};

#endif // TVision_MsgBox_h
