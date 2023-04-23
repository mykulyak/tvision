#ifndef TVISION_CODEPAGE_H
#define TVISION_CODEPAGE_H

#include <array>
#include <unordered_map>

namespace tvision {

class CpTranslator {

    CpTranslator() noexcept;
    static CpTranslator instance;

    struct CpTable {
        std::string_view cp;
        const uint32_t* toUtf8Int;
        const std::unordered_map<uint32_t, char> fromUtf8;

        CpTable(std::string_view cp, const std::string_view toUtf8[256],
            const std::array<uint32_t, 256>& toUtf8Int) noexcept;
    };

    static const CpTable tables[2];
    static const CpTable* activeTable;

    static void useTable(const CpTable* table) noexcept { activeTable = table; }

public:
    static void use(std::string_view cp) noexcept
    {
        for (const CpTable& t : tables)
            if (t.cp == cp) {
                useTable(&t);
                return; // Watch out!
            }
        useTable(&tables[0]);
    }

    static uint32_t toUtf8Int(unsigned char c) noexcept { return activeTable->toUtf8Int[c]; }

    static char fromUtf8(std::string_view s) noexcept;

    static char printableFromUtf8(std::string_view s) noexcept
    {
        uchar c = fromUtf8(s);
        if (c < ' ')
            return '\0';
        return c;
    }
};

} // namespace tvision

#endif // TVISION_CODEPAGE_H
