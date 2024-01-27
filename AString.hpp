#ifndef _AString_
#define _AString_

#include <atlstr.h>
#include <iostream>
#include <string>
#include <cstdint>

namespace Math {
    template <class T>
    static inline T RoundUp(T value, T alignment) {
        return (value + alignment - 1) & ~(alignment - 1);
    }

    template <class T>
    static inline T Max(T a, T b) {
        return (a > b) ? a : b;
    }

    template<class T> struct RemoveReference { using type = T;};
    template<class T> struct RemoveReference<T&> { using type = T;};
    template<class T> struct RemoveReference<T&&> { using type = T;};

};

template<class T> using RemoveReferenceT = typename Math::RemoveReference<T>::type;


class AString{
public:
    AString();
    AString(std::uint32_t _capacity);
    AString(const AString& string);
    AString(const std::string& string);
    AString(const char* string);
    AString(const char* start, const char* end);
    ~AString();

    _NODISCARD std::uint32_t      length() const;
    _NODISCARD bool               IsEmpty() const;

    _NODISCARD char* data();
    _NODISCARD const char* c_str() const;
    _NODISCARD const char* c_end() const;

    _NODISCARD char* begin();
    _NODISCARD const char* c_begin() const;
    _NODISCARD char* end();

    static const AString& GetEmpty();

    void Assign(const char* string);
    void Assign(const char* start, const char* end);
    void Assign(const AString& string);
    void Assign(AString&& string);

    void clear();
    void ClearAndFreeMemory();
    void SetReserved(std::size_t capacity);

    void SetCapacity(std::uint32_t _length);

    _NODISCARD char& operator [] (size_t index);
    AString& operator = (const char* string);
    AString& operator = (const AString& string);
    AString& operator + (char c);
    AString& operator + (std::uint32_t value);
    AString& operator + (std::uint64_t value);
    AString& operator + (const std::string& string);
    AString& operator + (const char* string);
    AString& operator + (const AString& string);
    AString& operator += (char c);
    AString& operator += (std::uint32_t value);
    AString& operator += (std::uint64_t value);
    AString& operator += (const std::string& string);
    AString& operator += (const char* string);
    AString& operator += (const AString& string);
    _NODISCARD bool operator == (const char* other) const;
    _NODISCARD bool operator == (const AString& other) const;
    _NODISCARD bool operator != (const char* other) const;
    _NODISCARD bool operator != (const AString& other) const;
    _NODISCARD bool operator < (const AString& other) const;
    _NODISCARD bool operator > (const AString& other) const;

    AString& Append(const AString& string);
    AString& Append(const char* string, size_t _length);
    AString& Append(const char* start, const char* end);
    AString& AppendFormat(_In_z_ _Printf_format_string_ const char* Format, ...);

    _NODISCARD std::int32_t Compare(const AString& other) const;
    _NODISCARD std::int32_t Compare(const char* other) const;
    _NODISCARD std::int32_t CompareI(const AString& other) const;
    _NODISCARD std::int32_t CompareI(const char* other) const;
    _NODISCARD bool         Equals(const char* other) const;
    _NODISCARD bool         Equals(const AString& other) const;
    _NODISCARD bool         EqualsI(const char* other) const;
    _NODISCARD bool         EqualsI(const AString& other) const;

    _NODISCARD bool         MustBeFreed() const;

    _NODISCARD        std::uint64_t Hash();
    _NODISCARD        std::uint32_t Hashw32();

    AString& Format(const char* Format,...);
    AString& VFormat(const char* Format, va_list args);

    _NODISCARD std::int32_t           Scan(_Scanf_s_format_string_ const char* _Format, ...) const;
    _NODISCARD static std::int32_t    ScanS(const char* Buffer, _Scanf_s_format_string_ const char* _Format, ...);


    std::uint32_t               Replace(char from, char to, std::uint32_t maxReplaces = 0);
    std::uint32_t               Replace(const char* from, const char* to, std::uint32_t maxReplaces = 0);

    void                        ToLower();
    void                        ToUpper();
    void                        Trim(std::uint32_t startCharsToTrim, std::uint32_t endCharsToTrim);
    void                        TrimStart(char charToTrimFromStart);
    void                        TrimEnd(char charToTrimFromStart);

    _NODISCARD bool        Find(const char* _str);
    _NODISCARD const char* Find(char c, const char* startPos = nullptr, const char* endPos = nullptr) const;
    _NODISCARD const char* Find(const char* subString, const char* startPos = nullptr, const char* endPos = nullptr) const;
    _NODISCARD const char* Find(const AString& subString, const char* startPos = nullptr, const char* endPos = nullptr) const;

    _NODISCARD const char* FindI(char c, const char* startPos = nullptr, const char* endPos = nullptr) const;
    _NODISCARD const char* FindI(const char* subString, const char* startPos = nullptr, const char* endPos = nullptr) const;
    _NODISCARD const char* FindI(const AString& subString, const char* startPos = nullptr, const char* endPos = nullptr) const;

    _NODISCARD const char* FindLast(char c, const char* startPos = nullptr, const char* endPos = nullptr) const;
    _NODISCARD const char* FindLast(const char* subString, const char* startPos = nullptr, const char* endPos = nullptr) const;
    _NODISCARD const char* FindLast(const AString& subString, const char* startPos = nullptr, const char* endPos = nullptr) const;

    _NODISCARD const char* FindLastI(char c, const char* startPos = nullptr, const char* endPos = nullptr) const;
    _NODISCARD const char* FindLastI(const char* subString, const char* startPos = nullptr, const char* endPos = nullptr) const;
    _NODISCARD const char* FindLastI(const AString& subString, const char* startPos = nullptr, const char* endPos = nullptr) const;

    _NODISCARD bool          EndsWith(char c) const;
    _NODISCARD bool          EndsWith(const char* string) const;
    _NODISCARD bool          EndsWith(const AString& string) const;

    _NODISCARD bool          EndsWithI(const char* other) const;
    _NODISCARD bool          EndsWithI(const AString& other) const;

    _NODISCARD bool          BeginsWith(char c) const;
    _NODISCARD bool          BeginsWith(const char* string) const;
    _NODISCARD bool          BeginsWith(const AString& string) const;

    _NODISCARD bool          BeginsWithI(const char* string) const;
    _NODISCARD bool          BeginsWithI(const AString& string) const;

    _NODISCARD static bool   Match(const char* pattern, const char* string);
    _NODISCARD bool          Matches(const char* pattern) const;
    _NODISCARD bool          Matches(const AString& pattern) const;
    _NODISCARD static bool   MatchI(const char* pattern, const char* string);
    _NODISCARD bool          MatchesI(const char* pattern) const;
    _NODISCARD bool          MatchesI(const AString& pattern) const;
    _NODISCARD bool          MidStr(AString left, AString right, AString& Ret);
    

    static void                 Copy(const char* src, char* dst);
    static void                 Copy(const char* src, char* dst, size_t len);

    _NODISCARD static std::size_t    StrLen(const char* string);
    _NODISCARD static std::int32_t   StrNCmp(const char* a, const char* b, size_t num);
    _NODISCARD static std::int32_t   StrNCmpI(const char* a, const char* b, size_t num);


    _NODISCARD static bool   IsWhitespace(char c);
    _NODISCARD static bool   IsUppercaseLetter(char c);
    _NODISCARD static bool   IsLowercaseLetter(char c);
    _NODISCARD static bool   IsLetter(char c);
    _NODISCARD static bool   IsNumber(char c);

    static constexpr auto npos{ static_cast<std::uint32_t>(-1) };

protected:
    enum : std::uint32_t { 
        MEM_MUST_BE_FREED_FLAG  = 0x00000001,
        DO_NOT_FREE_MEMORY_FLAG = 0x80000020,
        CAPACITY_MASK = 0x7FFFFFFF,
        RESERVED_MASK = 0xFFFFFFFE
    };

    enum : std::uint64_t { 
        k_offset_basis = 0xcbf29ce484222325ull,
        k_prime = 1099511628211ull
    };

    void SetCapacity(std::uint32_t reserved, bool mustFreeMemory);
    _NODISCARD std::uint32_t      get_capacity() const;
    __declspec(noinline) void Grow(std::uint32_t new_length);
    __declspec(noinline) void GrowNoCopy(std::uint32_t new_length); 
    bool StrstrEx(const char* Str1, const char* Str2);

    char*            str; 
    std::uint32_t    intlength;
    std::uint32_t    capacity;

    static const char* const   s_EmptyString;
    static const AString    s_EmptyAString;
};

template <std::uint32_t RESERVED = 256 >
class AStackString : public AString{
public:
    explicit AStackString();
    explicit AStackString(const AString& string);
    explicit AStackString(AString&& string);
    explicit AStackString(const AStackString& string);
    explicit AStackString(AStackString&& string);
    explicit AStackString(const char* string);
    explicit AStackString(const char* start, const char* end);
    inline ~AStackString() = default;

    AStackString< RESERVED >& operator = (const char* string);
    AStackString< RESERVED >& operator = (const AString& string);
    AStackString< RESERVED >& operator = (AString&& string);
    AStackString< RESERVED >& operator = (const AStackString& string);
    AStackString< RESERVED >& operator = (AStackString&& string);

private:
    char _str[RESERVED + 1];
};

#endif
