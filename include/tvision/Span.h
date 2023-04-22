#ifndef TVision_TSpan_h
#define TVision_TSpan_h

template <class T> class TSpan {

    // This is actually a generalization of TStringView for any kind of element
    // type (and without enforcing the 'const' qualifier).
    // It exists for compatibility with Borland C++ and because std::span (C++ 20)
    // may not be widely available yet.

    T* ptr;
    size_t len;

public:
    // These are defined inline because otherwise they trigger
    // a bug in Borland C++ when T is const.

    constexpr TSpan()
        : ptr(0)
        , len(0)
    {
    }

    constexpr TSpan(T* first, size_t n)
        : ptr(first)
        , len(n)
    {
    }

    constexpr TSpan(decltype(nullptr))
        : TSpan()
    {
    }

    template <size_t N>
    constexpr TSpan(T (&array)[N])
        : TSpan(array, N)
    {
    }

    constexpr operator TSpan<const T>() const { return TSpan<const T>(ptr, len); }

    constexpr T* data() const { return ptr; }

    constexpr size_t size() const { return len; }

    constexpr size_t size_bytes() const { return size() * sizeof(T); }

    constexpr bool empty() const { return bool(size() == 0); }

    constexpr T& operator[](size_t pos) const { return ptr[pos]; }

    constexpr T& front() const { return ptr[0]; }

    constexpr T& back() const { return ptr[len - 1]; }

    constexpr TSpan subspan(size_t pos) const { return TSpan<T>(ptr + pos, len - pos); }

    constexpr TSpan subspan(size_t pos, size_t n) const
    {
        return TSpan<T>(ptr + pos, n <= len - pos ? n : len - pos);
    }

    constexpr T* begin() const { return &ptr[0]; }

    constexpr const T* cbegin() const { return &ptr[0]; }

    constexpr T* end() const { return &ptr[len]; }

    constexpr const T* cend() const { return &ptr[len]; }
};

#endif // TVision_TSpan_h
