#ifndef TVISION_BASE64_H
#define TVISION_BASE64_H

#include <string>
#include <string_view>

namespace tvision {

// Returns the region of 'output' that contains the encoded data.
// Pre: the capacity of 'output' is no less than 4/3 of 'input.size()'.
std::string_view encodeBase64(std::string_view input, char* output) noexcept;

// Returns the region of 'output' that contains the decoded data.
// Pre: the capacity of 'output' is no less than 3/4 of 'input.size()'.
std::string_view decodeBase64(std::string_view input, char* output) noexcept;

inline std::string encodeBase64(std::string_view input)
{
    std::string result((input.size() * 4) / 3 + 4, '\0');
    auto encoded = encodeBase64(input, &result[0]);
    result.resize(encoded.size());
    return result;
}

inline std::string decodeBase64(std::string_view input)
{
    std::string result((input.size() * 3) / 4 + 3, '\0');
    auto encoded = decodeBase64(input, &result[0]);
    result.resize(encoded.size());
    return result;
}

} // namespace tvision

#endif // TVISION_BASE64_H
