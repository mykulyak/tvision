#include <algorithm>
#include <tvision/stringlookupvalidator.h>
#include <tvision/tobjstrm.h>

const char* const TStringLookupValidator::name = "TStringLookupValidator";

const char* TStringLookupValidator::errorMsg = "Input is not in list of valid strings";

__link(RLookupValidator);

TStreamableClass RStringLookupValidator(
    TStringLookupValidator::name, TStringLookupValidator::build, __DELTA(TStringLookupValidator));

TStringLookupValidator::TStringLookupValidator(const StringVector& aStrings) noexcept
    : TLookupValidator()
    , strings(aStrings)
{
}

TStringLookupValidator::~TStringLookupValidator() { }

void TStringLookupValidator::error() { MessageBox::error(errorMsg); }

bool TStringLookupValidator::lookup(const char* s)
{
    std::string str(s);
    return std::find(strings.cbegin(), strings.cend(), str) != strings.cend();
}

#ifndef NO_STREAMABLE

TStringLookupValidator::TStringLookupValidator(StreamableInit s) noexcept
    : TLookupValidator(s)
{
}

void TStringLookupValidator::write(opstream& os)
{
    TLookupValidator::write(os);
    os << strings.size();
    for (auto& item : strings) {
        os.writeString(item);
    }
}

void* TStringLookupValidator::read(ipstream& is)
{
    TLookupValidator::read(is);

    StringVector::size_type size = 0;
    is >> size;
    for (StringVector::size_type i = 0; i < size; ++i) {
        strings.push_back(is.readStlString());
    }

    return this;
}

#endif

void TStringLookupValidator::newStringList(const StringVector& aStrings) { strings = aStrings; }

#ifndef NO_STREAMABLE

TStreamable* TStringLookupValidator::build() { return new TStringLookupValidator(streamableInit); }

#endif
