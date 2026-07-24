#ifndef _AString__
#define _AString__

#include <Windows.h>
#include <cstdarg>
#include <string>
#include <cstdint>
#include <Array.hpp>
#include <iterator>
#include <algorithm>
#include <map>
#include <memory>
#include <mutex>
#include <iosfwd>
#include <functional>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new
#ifndef _CONSTEXPR20
#if _HAS_CXX20
#define _CONSTEXPR20 constexpr
#else
#define _CONSTEXPR20 inline
#endif
#endif
#define CheckStack static_assert((Count % 2) == 0, "Capacity must be multiple of 2")

extern const char* const a_EmptyString;

extern const wchar_t* const w_EmptyString;

class MemContainer {
public:
    MemContainer() noexcept = default;

    MemContainer(const MemContainer&) noexcept = default;

    ~MemContainer() noexcept;

    template <class _Elem>
    _NODISCARD_RAW_PTR_ALLOC __declspec(allocator) _Elem* allocate(const std::size_t _Count) {
        if (_Count == 0 || _Count > (std::numeric_limits<size_t>::max)() / sizeof(_Elem) - 1) {
            throw std::bad_array_new_length();
        }
        const std::size_t _Bytes = _Count * sizeof(_Elem);
        std::lock_guard<std::mutex> lock(Mutex);
        _Elem* _Ptr = static_cast<_Elem*>(::operator new[](_Bytes));
        try {
            _Mypair.insert({reinterpret_cast<std::uint8_t*>(_Ptr), _Bytes});
        }
        catch (...) {
            ::operator delete[](_Ptr);
            throw std::bad_alloc();
        }
        return _Ptr;
    }

    void deallocate(const wchar_t* _Ptr) noexcept {
        deallocate(reinterpret_cast<std::uint8_t*>(const_cast<wchar_t*>(_Ptr)));
    }

    void deallocate(const char* _Ptr) noexcept {
        deallocate(reinterpret_cast<std::uint8_t*>(const_cast<char*>(_Ptr)));
    }

    void deallocate(std::uint8_t* _Ptr) noexcept;
    _NODISCARD std::size_t size(const wchar_t* _Ptr) noexcept {
        auto Size = size(reinterpret_cast<std::uint8_t*>(const_cast<wchar_t*>(_Ptr)));
        return (Size != -1) ? Size / 2 : -1;
    }

    _NODISCARD std::size_t size(const char* _Ptr) noexcept {
        auto Size = size(reinterpret_cast<std::uint8_t*>(const_cast<char*>(_Ptr)));
        return (Size != -1) ? Size / 1 : -1;
    }

    _NODISCARD std::size_t size(std::uint8_t* _Ptr) noexcept;
protected:
    std::mutex Mutex;
    std::map<std::uint8_t*, std::size_t> _Mypair;
};

extern MemContainer MemAlty;

template <class _Ty>
constexpr std::uint32_t Ulong64toUint32(_Ty Value) {
    if constexpr (sizeof(_Ty) > 4) {
        return static_cast<std::uint32_t>(static_cast<std::uint64_t>(Value) & 0xFFFFFFFFULL);
    }
    else {
        return static_cast<std::uint32_t>(Value);
    }
}

template <class _Ty>
_CONSTEXPR20 bool _Is_nullptr(const _Ty* const _Ptr) {
    static_assert(std::is_same_v<_Ty, char> || std::is_same_v<_Ty, wchar_t>, "Template argument must be either char or wchar_t");
    return _Ptr == nullptr || _Ptr[0] == static_cast<_Ty>(0);
}

enum : std::uint32_t {
    MEM_MUST_BE_FREED_FLAG = 0x00000001,
    MEM_VECTOR_FLAG = 0x80000020
};

template <class _Elem>
class EncodeCodecvt {
public:
    using _Alloc = _STD allocator<_Elem>;
    using _Alty = _STD _Rebind_alloc_t<_Alloc, _Elem>;
    using _Alty_traits = _STD allocator_traits<_Alty>;
    using size_type = typename _Alty_traits::size_type;
    using value_type = _Elem;
public:
    EncodeCodecvt() noexcept {
        _Construct_empty();
    }

    ~EncodeCodecvt() {
        if (_Is_Tidy_deallocate()) {
            MemAlty.deallocate(_Myptr());
        }
    }

    _NODISCARD _CONSTEXPR20 value_type* _Myptr() noexcept {
        return Myptr;
    }

    _NODISCARD _CONSTEXPR20 const value_type* _Myptr() const noexcept {
        return Myptr;
    }

    _NODISCARD _CONSTEXPR20 size_type _Mysize() noexcept {
        return Mysize;
    }

    _NODISCARD _CONSTEXPR20 const size_type _Mysize() const noexcept {
        return Mysize;
    }

    _NODISCARD bool ToAnsi(const wchar_t* _Right) {
        if (_Is_Tidy_deallocate()) MemAlty.deallocate(Myptr);
        _Construct_empty();
        std::uint32_t _Count = 0, _Length = Ulong64toUint32(wcslen(_Right));
        if (!_Length) return false;
        _Count = WideCharToMultiByte(CP_ACP, 0, _Right, _Length, 0, 0, 0, 0);
        if (!_Count) return false;
        Myres = ++_Count; _Count--;
        Myptr = MemAlty.allocate<value_type>(Myres);
        if (!Myptr) return false;
        Mysize = WideCharToMultiByte(CP_ACP, 0, _Right, _Length, (char*)(Myptr), _Count, 0, 0);
        return Mysize != 0;
    }

    _NODISCARD bool ToAnsi(const wchar_t* _Right, std::uint32_t _Length) {
        if (_Is_Tidy_deallocate()) MemAlty.deallocate(Myptr);
        _Construct_empty();
        if (!_Length) return false;
        std::uint32_t _Count = WideCharToMultiByte(CP_ACP, 0, _Right, _Length, 0, 0, 0, 0);
        if (!_Count) return false;
        Myres = ++_Count; _Count--;
        Myptr = MemAlty.allocate<value_type>(Myres);
        if (!Myptr) return false;
        Mysize = WideCharToMultiByte(CP_ACP, 0, _Right, _Length, (char*)(Myptr), _Count, 0, 0);
        return Mysize != 0;
    }

    _NODISCARD bool ToUtf8(const char* _Right) {
        if (_Is_Tidy_deallocate()) MemAlty.deallocate(Myptr);
        _Construct_empty();
        std::uint32_t _Count = 0, _Length = Ulong64toUint32(strlen(_Right));
        if (!_Length) return false;
        _Count = MultiByteToWideChar(CP_ACP, 0, _Right, _Length, 0, 0);
        if (!_Count) return false;
        Myres = ++_Count; _Count--;
        Myptr = MemAlty.allocate<value_type>(Myres);
        if (!Myptr) return false;
        Mysize = MultiByteToWideChar(CP_ACP, 0, _Right, _Length, (wchar_t*)(Myptr), _Count);
        return Mysize != 0;
    }

    _NODISCARD bool ToUtf8(const char* _Right, std::uint32_t _Length) {
        if (_Is_Tidy_deallocate()) MemAlty.deallocate(Myptr);
        _Construct_empty();
        if (!_Length) return false;
        std::uint32_t _Count = MultiByteToWideChar(CP_ACP, 0, _Right, _Length, 0, 0);
        if (!_Count) return false;
        Myres = ++_Count; _Count--;
        Myptr = MemAlty.allocate<value_type>(Myres);
        if (!Myptr) return false;
        Mysize = MultiByteToWideChar(CP_ACP, 0, _Right, _Length, (wchar_t*)(Myptr), _Count);
        return Mysize != 0;
    }
protected:
    value_type* Myptr;
    size_type   Mysize;
    size_type   Myres;
private:
    _CONSTEXPR20 void _Construct_empty() {
        Myptr = 0;
        Mysize = 0;
        Myres = 0;
    }

    _NODISCARD bool _Is_Tidy_deallocate() const noexcept {
        return ((Myres & MEM_MUST_BE_FREED_FLAG) == MEM_MUST_BE_FREED_FLAG) && Myptr != 0;
    }
};

template <class _Elem>
class BasicString;

using AString  = BasicString<char>;

using WString  = BasicString<wchar_t>;

namespace str_utils {
    inline size_t           xstrlen(const char* s)    { return strlen(s); }

    inline size_t           xstrlen(const wchar_t* s) { return wcslen(s); }

    inline int              xstrcmp(const char* a, const char* b)       { return strcmp(a, b); }

    inline int              xstrcmp(const wchar_t* a, const wchar_t* b) { return wcscmp(a, b); }

    inline int              xstricmp(const char* a, const char* b)       { return _stricmp(a, b); }

    inline int              xstricmp(const wchar_t* a, const wchar_t* b) { return _wcsicmp(a, b); }

    inline int              xtolower(char c)    { return tolower(c); }

    inline int              xtolower(wchar_t c) { return towlower(c); }

    inline int              xtoupper(char c)    { return toupper(c); }

    inline int              xtoupper(wchar_t c) { return towupper(c); }

    inline int xStrCmpNI(const char* a, const char* b, int n) {
        for (int i = 0; i < n; i++) {
            int ca = tolower(static_cast<unsigned char>(a[i]));
            int cb = tolower(static_cast<unsigned char>(b[i]));
            if (ca != cb) return ca - cb;
            if (!a[i] || !b[i]) break;
        }
        return 0;
    }

    inline int xStrCmpNI(const wchar_t* a, const wchar_t* b, int n) {
        for (int i = 0; i < n; i++) {
            int ca = towlower(a[i]);
            int cb = towlower(b[i]);
            if (ca != cb) return ca - cb;
            if (!a[i] || !b[i]) break;
        }
        return 0;
    }

    inline int              xvscprintf(const char* fmt, va_list args)    { return _vscprintf(fmt, args); }

    inline int              xvscprintf(const wchar_t* fmt, va_list args) { return _vscwprintf(fmt, args); }

    inline int              xvsnprintf(char* buf, size_t cnt, const char* fmt, va_list args)       { return vsnprintf(buf, cnt, fmt, args); }

    inline int              xvsnprintf(wchar_t* buf, size_t cnt, const wchar_t* fmt, va_list args) { return _vsnwprintf_s(buf, cnt, -1, fmt, args); }

    inline char*            xstrtok(char* str, const char* delim, char** ctx)          { return strtok_s(str, delim, ctx); }

    inline wchar_t*         xstrtok(wchar_t* str, const wchar_t* delim, wchar_t** ctx) { return wcstok_s(str, delim, ctx); }

    inline const char* xstrstr_i(const char* _String, const char* _SubString) {
        if (!*_SubString) return _String;
        for (; *_String; ++_String) {
            auto i = _String, n = _SubString;
            while (*i && *n && tolower(*i) == tolower(*n)) { ++i; ++n; }
            if (!*n) return _String;
        }
        return nullptr;
    }

    inline const wchar_t* xstrstr_i(const wchar_t* _String, const wchar_t* _SubString) {
        if (!*_SubString) return _String;
        for (; *_String; ++_String) {
            const wchar_t* i = _String, * n = _SubString;
            while (*i && *n && towlower(*i) == towlower(*n)) { ++i; ++n; }
            if (!*n) return _String;
        }
        return nullptr;
    }

    template <class _Elem> _Elem xupper_ascii(_Elem c) {
        if constexpr (std::is_same_v<_Elem, char>) {
            return (c >= 'A' && c <= 'Z') ? static_cast<_Elem>(c + 32) : c;
        } else {
            if ((c >= L'A' && c <= L'Z') || (c >= 0xC0 && c <= 0xD6)) return static_cast<_Elem>(c | 32);
            return c;
        }
    }

    template <class _Elem>
    bool xMatches(const _Elem* _Ptr, const _Elem* _Right) noexcept {
        constexpr _Elem _Star = static_cast<_Elem>('*');
        constexpr _Elem _QMark = static_cast<_Elem>('?');
        constexpr _Elem _Dot = static_cast<_Elem>('.');
        bool star;
    new_segment:
        star = false;
        if (*_Right == _Star) { star = true; do { _Right++; } while (*_Right == _Star); }
    test_match:
        int i;
        for (i = 0; _Right[i] && (_Right[i] != _Star); i++) {
            if (_Ptr[i] != _Right[i]) {
                if (!_Ptr[i]) return false;
                if ((_Right[i] == _QMark) && (_Ptr[i] != _Dot)) continue;
                if (!star) return false;
                _Ptr++; goto test_match;
            }
        }
        if (_Right[i] == _Star) { _Ptr += i; _Right += i; goto new_segment; }
        if (!_Ptr[i]) return true;
        if (i && _Right[i - 1] == _Star) return true;
        if (!star) return false;
        _Ptr++; goto test_match;
        return false;
    }

    template <class _Elem>
    bool xMatchesI(const _Elem* _Ptr, const _Elem* _Right) noexcept {
        constexpr _Elem _Star = static_cast<_Elem>('*');
        constexpr _Elem _QMark = static_cast<_Elem>('?');
        constexpr _Elem _Dot = static_cast<_Elem>('.');
        bool star;
    new_segment:
        star = false;
        if (*_Right == _Star) { star = true; do { _Right++; } while (*_Right == _Star); }
    test_match:
        int i;
        for (i = 0; _Right[i] && (_Right[i] != _Star); i++) {
            auto a = xupper_ascii(_Ptr[i]);
            auto b = xupper_ascii(_Right[i]);
            if (a != b) {
                if (!_Ptr[i]) return false;
                if ((_Right[i] == _QMark) && (_Ptr[i] != _Dot)) continue;
                if (!star) return false;
                _Ptr++; goto test_match;
            }
        }
        if (_Right[i] == _Star) { _Ptr += i; _Right += i; goto new_segment; }
        if (!_Ptr[i]) return true;
        if (i && _Right[i - 1] == _Star) return true;
        if (!star) return false;
        _Ptr++; goto test_match;
        return false;
    }
}
template <class _Elem>
class BasicString {
public:
    using _Traits = _STD char_traits<_Elem>;
    using _Alloc = _STD allocator<_Elem>;
    using _Alty = _STD _Rebind_alloc_t<_Alloc, _Elem>;
    using _Alty_traits = _STD allocator_traits<_Alty>;
    using _Scary_val = _STD _String_val<_STD conditional_t<_STD _Is_simple_alloc_v<_Alty>, _STD _Simple_types<_Elem>,
        _STD _String_iter_types<_Elem, typename _Alty_traits::size_type, typename _Alty_traits::difference_type,
        typename _Alty_traits::pointer, typename _Alty_traits::const_pointer>>>;
    using traits_type = _Traits;
    using allocator_type = _Alloc;
    using value_type = _Elem;
    using size_type = typename _Alty_traits::size_type;
    using difference_type = typename _Alty_traits::difference_type;
    using pointer = typename _Alty_traits::pointer;
    using const_pointer = typename _Alty_traits::const_pointer;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = _STD _String_iterator<_Scary_val>;
    using const_iterator = _STD _String_const_iterator<_Scary_val>;
    using reverse_iterator = _STD reverse_iterator<iterator>;
    using const_reverse_iterator = _STD reverse_iterator<const_iterator>;
    static constexpr auto npos{ static_cast<size_type>(-1) };
private:
    static constexpr size_type _BUF_SIZE = _Scary_val::_BUF_SIZE;
    static constexpr size_type _Alloc_mask = _Scary_val::_Alloc_mask;
    static constexpr size_type _Small_string_capacity = _Scary_val::_Small_string_capacity;
    static constexpr size_type _Least_allocation_size = _Small_string_capacity + 1 + 1;
    static constexpr bool _Can_memcpy_val = _STD _Is_specialization_v<_Traits, _STD char_traits>&& _STD is_trivial_v<pointer>;
    static constexpr size_t _Memcpy_val_offset = _STD _Size_after_ebco_v<_STD _Container_base>;
    static constexpr size_t _Memcpy_val_size = sizeof(_Scary_val) - _Memcpy_val_offset;
    template <class _Iter>
    using _Is_elem_cptr = _STD bool_constant<_STD _Is_any_of_v<_Iter, const _Elem* const, _Elem* const, const _Elem*, _Elem*>>;
#if _HAS_CXX17
    template <class _StringViewIsh>
    using _Is_string_view_ish =
        _STD enable_if_t<_STD conjunction_v<_STD is_convertible<const _StringViewIsh&, _STD basic_string_view<_Elem, _Traits>>,
        _STD negation<_STD is_convertible<const _StringViewIsh&, const _Elem*>>>,
        int>;
#endif
public:
    BasicString() noexcept {
        _Construct_empty();
    }

    BasicString(const BasicString& _Right) {
        _Construct_empty();
        Assign(_Right);
    }

    BasicString(const BasicString&& _Right) noexcept {
        _Construct_empty();
        Assign(_Right);
    }

    template <class U = _Elem, std::enable_if_t<std::is_same_v<U, char>, int> = 0>
    BasicString(const std::string& _Right) {
        _Construct_empty();
        Assign(_Right);
    }

    template <class U = _Elem, std::enable_if_t<std::is_same_v<U, wchar_t>, int> = 0>
    BasicString(const std::wstring& _Right) {
        _Construct_empty();
        Assign(_Right);
    }

    BasicString(const size_type _Count) {
        _Construct_empty();
        Assign(_Count);
    }

    BasicString(_STD initializer_list<_Elem> _Ilist) {
        _Construct_empty();
        Assign(_Ilist.begin(), _STD _Convert_size<size_type>(_Ilist.size()));
    }

    BasicString(_In_z_ const _Elem* const _Ptr) {
        _Construct_empty();
        Assign(_Ptr, _STD _Convert_size<size_type>(_Traits::length(_Ptr)));
    }

    BasicString(const BasicString& _Right, const size_type _Roff) {
        _Construct_empty();
        _Right._Check_offset(_Roff);
        Assign(_Right._Myptr() + _Roff, _Right._Clamp_suffix_size(_Roff, npos));
    }

    BasicString(const BasicString&& _Right, const size_type _Roff) {
        _Construct_empty();
        _Right._Check_offset(_Roff);
        Assign(_Right._Myptr() + _Roff, _Right._Clamp_suffix_size(_Roff, npos));
    }

    BasicString(const BasicString& _Right, const size_type _Roff, const size_type _Count) {
        _Construct_empty();
        _Right._Check_offset(_Roff);
        Assign(_Right._Myptr() + _Roff, _Right._Clamp_suffix_size(_Roff, _Count));
    }

    BasicString(const _Elem* const _Ptr, const _Elem* const _PtrEnd) {
        _Construct_empty();
        Assign(_Ptr, _PtrEnd);
    }

    BasicString(_In_reads_(_Count) const _Elem* const _Ptr, _CRT_GUARDOVERFLOW const size_type _Count) {
        _Construct_empty();
        Assign(_Ptr, _Count);
    }

    BasicString(_CRT_GUARDOVERFLOW const size_type _Count, const _Elem _Ch) {
        _Construct_empty();
        Assign(_Count, _Ch);
    }

    ~BasicString() {
        _Tidy_deallocate();
    }

    BasicString& Assign(const BasicString& _Right) {
        return Assign(_Right._Myptr());
    }

    BasicString& Assign(const BasicString&& _Right) noexcept {
        return Assign(_Right);
    }

    template <class U = _Elem, std::enable_if_t<std::is_same_v<U, char>, int> = 0>
    BasicString& Assign(const WString& _Right) {
        return Assign(_Right.data());
    }

    template <class U = _Elem, std::enable_if_t<std::is_same_v<U, char>, int> = 0>
    BasicString& Assign(const std::string& _Right) {
        return Assign(_Right.c_str());
    }

    template <class U = _Elem, std::enable_if_t<std::is_same_v<U, wchar_t>, int> = 0>
    BasicString& Assign(const std::wstring& _Right) {
        return Assign(_Right.c_str());
    }

    BasicString& Assign(const size_type _Count) noexcept {
        _Tidy_deallocate();
        size_type _New_capacity = _Count;
        this->str = _Allocate_for_capacity(_New_capacity);
        this->capacity = Ulong64toUint32(_New_capacity);
        this->intlength = 0;
        _Traits::assign(_Myptr()[0], _Elem());
        return *this;
    }

    BasicString& Assign(_STD initializer_list<_Elem> _Ilist) {
        return Assign(_Ilist.begin(), _STD _Convert_size<size_type>(_Ilist.size()));
    }

    BasicString& Assign(_In_z_ const _Elem* const _Ptr) {
        if (_Is_nullptr(_Ptr)) {
            return *this;
        }
        return Assign(_Ptr, _STD _Convert_size<size_type>(_Traits::length(_Ptr)));
    }

    template <class U = _Elem, std::enable_if_t<std::is_same_v<U, char>, int> = 0>
    BasicString& Assign(const wchar_t* const _Right) {
        EncodeCodecvt<char> Convert;
        if (!Convert.ToAnsi(_Right)) {
            return *this;
        }
        return Assign(Convert._Myptr(), Convert._Mysize());
    }

    BasicString& Assign(const _Elem* const _Ptr, const _Elem* const _PtrEnd) {
        if (_Is_nullptr(_Ptr)) {
            return *this;
        }
        return Assign(_Ptr, _STD _Convert_size<size_type>(_PtrEnd - _Ptr));
    }

    BasicString& Assign(_In_reads_(_Count) const _Elem* const _Ptr, _CRT_GUARDOVERFLOW const size_type _Count) {
        if (_Is_nullptr(_Ptr)) {
            return *this;
        }
        if (_Count <= _Myres()) {
            _Elem* const _Old_ptr = _Myptr();
            this->intlength = Ulong64toUint32(_Count);
            _Traits::move(_Old_ptr, _Ptr, _Count);
            _Traits::assign(_Old_ptr[_Count], _Elem());
            return *this;
        }
        return _Reallocate_for(_Count, [](_Elem* const _New_ptr, const size_type _Count, const _Elem* const _Ptr) {
            _Traits::copy(_New_ptr, _Ptr, _Count);
            _Traits::assign(_New_ptr[_Count], _Elem());
            },
            _Ptr
        );
    }

    BasicString& Assign(_CRT_GUARDOVERFLOW const size_type _Count, const _Elem _Ch) {
        if (_Count <= _Myres()) {
            _Elem* const _Old_ptr = _Myptr();
            this->intlength = Ulong64toUint32(_Count);
            _Traits::assign(_Old_ptr, _Count, _Ch);
            _Traits::assign(_Old_ptr[_Count], _Elem());
            return *this;
        }
        return _Reallocate_for(
            _Count, [](_Elem* const _New_ptr, const size_type _Count, const _Elem _Ch) {
                _Traits::assign(_New_ptr, _Count, _Ch);
                _Traits::assign(_New_ptr[_Count], _Elem());
            },
            _Ch
        );
    }

    BasicString& Assign(const BasicString& _Right, const size_type _Roff, size_type _Count = npos) {
        _Right._Check_offset(_Roff);
        _Count = _Right._Clamp_suffix_size(_Roff, _Count);
        return Assign(_Right._Myptr() + _Roff, _Count);
    }

    BasicString& Append(const BasicString& _Right) {
        return Append(_Right._Myptr(), _Right._Mysize());
    }

    BasicString& Append(const BasicString&& _Right) {
        return Append(_Right._Myptr(), _Right._Mysize());
    }

    template <class U = _Elem, std::enable_if_t<std::is_same_v<U, char>, int> = 0>
    BasicString& Append(const WString& _Right) {
        return Append(_Right.data());
    }

    template <class U = _Elem, std::enable_if_t<std::is_same_v<U, char>, int> = 0>
    BasicString& Append(const std::string& _Right) {
        return Append(_Right.data(), _Right.length());
    }

    template <class U = _Elem, std::enable_if_t<std::is_same_v<U, wchar_t>, int> = 0>
    BasicString& Append(const std::wstring& _Right) {
        return Append(_Right.data());
    }

    BasicString& Append(_STD initializer_list<_Elem> _Ilist) {
        return Append(_Ilist.begin(), _STD _Convert_size<size_type>(_Ilist.size()));
    }

    BasicString& Append(_In_z_ const _Elem* const _Ptr) {
        if (_Is_nullptr(_Ptr)) {
            return *this;
        }
        return Append(_Ptr, _STD _Convert_size<size_type>(_Traits::length(_Ptr)));
    }

    template <class U = _Elem, std::enable_if_t<std::is_same_v<U, char>, int> = 0>
    BasicString& Append(const wchar_t* const _Right) {
        EncodeCodecvt<char> Convert;
        if (!Convert.ToAnsi(_Right)) {
            return *this;
        }
        return Append(Convert._Myptr(), Convert._Mysize());
    }

    BasicString& Append(_In_reads_(_Count) const _Elem* const _Ptr, _CRT_GUARDOVERFLOW const size_type _Count) {
        if (_Is_nullptr(_Ptr)) {
            return *this;
        }
        const size_type _Old_size = _Mysize();
        if (_Count <= _Myres() - _Old_size) {
            this->intlength = Ulong64toUint32(_Old_size + _Count);
            _Elem* const _Old_ptr = _Myptr();
            _Traits::move(_Old_ptr + _Old_size, _Ptr, _Count);
            _Traits::assign(_Old_ptr[_Old_size + _Count], _Elem());
            return *this;
        }
        return _Reallocate_grow_by(
            _Count,
            [](_Elem* const _New_ptr, const _Elem* const _Old_ptr, const size_type _Old_size, const _Elem* const _Ptr,
                const size_type _Count) {
                    _Traits::copy(_New_ptr, _Old_ptr, _Old_size);
                    _Traits::copy(_New_ptr + _Old_size, _Ptr, _Count);
                    _Traits::assign(_New_ptr[_Old_size + _Count], _Elem());
            },
            _Ptr, _Count);
    }

    BasicString& Append(_CRT_GUARDOVERFLOW const size_type _Count, const _Elem _Ch) {
        const size_type _Old_size = _Mysize();
        if (_Count <= _Myres() - _Old_size) {
            this->intlength = Ulong64toUint32(_Old_size + _Count);
            _Elem* const _Old_ptr = _Myptr();
            _Traits::assign(_Old_ptr + _Old_size, _Count, _Ch);
            _Traits::assign(_Old_ptr[_Old_size + _Count], _Elem());
            return *this;
        }
        return _Reallocate_grow_by(
            _Count,
            [](_Elem* const _New_ptr, const _Elem* const _Old_ptr, const size_type _Old_size, const size_type _Count,
                const _Elem _Ch) {
                    _Traits::copy(_New_ptr, _Old_ptr, _Old_size);
                    _Traits::assign(_New_ptr + _Old_size, _Count, _Ch);
                    _Traits::assign(_New_ptr[_Old_size + _Count], _Elem());
            },
            _Count, _Ch);
    }

    BasicString& Append(const BasicString& _Right, const size_type _Roff, size_type _Count = npos) {
        _Right._Check_offset(_Roff);
        _Count = _Right._Clamp_suffix_size(_Roff, _Count);
        return Append(_Right._Myptr() + _Roff, _Count);
    }

    BasicString& AppendFormat(_In_z_ _Printf_format_string_ const _Elem* Format, ...) {
        va_list args;
        va_start(args, Format);
        return AppendVFormat(Format, args);
    }

    BasicString& AppendVFormat(_In_z_ _Printf_format_string_ const _Elem* Format, va_list args) {
        BasicString Buffer;
        Buffer.VFormat(Format, args);
        return Append(Buffer);
    }

    BasicString& Insert(const size_type _Off, const BasicString& _Right) {
        return Insert(_Off, _Right._Myptr(), _Right._Mysize());
    }

    BasicString& Insert(const size_type _Off, const std::string& _Right) {
        return Insert(_Off, _Right.data(), _Right.length());
    }

    BasicString& Insert(const size_type _Off, _In_z_ const _Elem* const _Ptr) {
        return Insert(_Off, _Ptr, _STD _Convert_size<size_type>(_Traits::length(_Ptr)));
    }

    BasicString& Insert(const size_type _Off, _CRT_GUARDOVERFLOW const size_type _Count, const _Elem _Ch) {
        _Check_offset(_Off);
        const size_type _Old_size = _Mysize();
        if (_Count <= _Myres() - _Old_size) {
            this->intlength = Ulong64toUint32(_Old_size + _Count);
            _Elem* const _Old_ptr = _Myptr();
            _Elem* const _Insert_at = _Old_ptr + _Off;
            _Traits::move(_Insert_at + _Count, _Insert_at, _Old_size - _Off + 1);
            _Traits::assign(_Insert_at, _Count, _Ch);
            return *this;
        }
        return _Reallocate_grow_by(
            _Count,
            [](_Elem* const _New_ptr, const _Elem* const _Old_ptr, const size_type _Old_size, const size_type _Off,
                const size_type _Count, const _Elem _Ch) {
                    _Traits::copy(_New_ptr, _Old_ptr, _Off);
                    _Traits::assign(_New_ptr + _Off, _Count, _Ch);
                    _Traits::copy(_New_ptr + _Off + _Count, _Old_ptr + _Off, _Old_size - _Off + 1);
            },
            _Off, _Count, _Ch);
    }

    BasicString& Insert(const size_type _Off, _In_reads_(_Count) const _Elem* const _Ptr, _CRT_GUARDOVERFLOW const size_type _Count) {
        _Check_offset(_Off);
        const size_type _Old_size = _Mysize();
        const bool _Check_overlap = _Count <= _Myres() - _Old_size;
        if (_Check_overlap) {
            this->intlength = Ulong64toUint32(_Old_size + _Count);
            _Elem* const _Old_ptr = _Myptr();
            _Elem* const _Insert_at = _Old_ptr + _Off;
            size_type _Ptr_shifted_after;
            if (_Ptr + _Count <= _Insert_at || _Ptr > _Old_ptr + _Old_size) {
                _Ptr_shifted_after = _Count;
            }
            else if (_Insert_at <= _Ptr) {
                _Ptr_shifted_after = 0;
            }
            else {
                _Ptr_shifted_after = static_cast<size_type>(_Insert_at - _Ptr);
            }
            _Traits::move(_Insert_at + _Count, _Insert_at, _Old_size - _Off + 1);
            _Traits::copy(_Insert_at, _Ptr, _Ptr_shifted_after);
            _Traits::copy(
                _Insert_at + _Ptr_shifted_after, _Ptr + _Count + _Ptr_shifted_after, _Count - _Ptr_shifted_after);
            return *this;
        }
        return _Reallocate_grow_by(
            _Count,
            [](_Elem* const _New_ptr, const _Elem* const _Old_ptr, const size_type _Old_size, const size_type _Off,
                const _Elem* const _Ptr, const size_type _Count) {
                    _Traits::copy(_New_ptr, _Old_ptr, _Off);
                    _Traits::copy(_New_ptr + _Off, _Ptr, _Count);
                    _Traits::copy(_New_ptr + _Off + _Count, _Old_ptr + _Off, _Old_size - _Off + 1);
            },
            _Off, _Ptr, _Count);
    }

    BasicString& Insert(const size_type _Off, const BasicString& _Right, const size_type _Roff, size_type _Count = npos) {
        _Right._Check_offset(_Roff);
        _Count = _Right._Clamp_suffix_size(_Roff, _Count);
        return Insert(_Off, _Right._Myptr() + _Roff, _Count);
    }

    BasicString& Erase(const size_type _Off = 0) {
        _Check_offset(_Off);
        _Eos(_Off);
        return *this;
    }

    BasicString& Erase(const size_type _Off, const size_type _Count) {
        _Check_offset(_Off);
        return _Erase_noexcept(_Off, _Count);
    }

    iterator Erase(const const_iterator _Where) noexcept {
        const auto _Off = static_cast<size_type>(_STD _Unfancy(_Where._Ptr) - _Myptr());
        _Erase_noexcept(_Off, 1);
        return begin() + static_cast<difference_type>(_Off);
    }

    iterator Erase(const const_iterator _First, const const_iterator _Last) noexcept {
        _Adl_verify_range(_First, _Last);
        const auto _Off = static_cast<size_type>(_STD _Unfancy(_First._Ptr) - _Myptr());
        _Erase_noexcept(_Off, static_cast<size_type>(_Last._Ptr - _First._Ptr));
        return begin() + static_cast<difference_type>(_Off);
    }

    _CONSTEXPR20 void Clear() noexcept {
        _Eos(0);
    }

    _CONSTEXPR20 void ToLower() noexcept {
        for (size_t i = 0; i < length(); i++) {
            _Myptr()[i] = _Elem(str_utils::xtolower(_Myptr()[i]));
        }
    }

    _CONSTEXPR20 void ToUpper() noexcept {
        for (size_t i = 0; i < length(); i++) {
            _Myptr()[i] = _Elem(str_utils::xtoupper(_Myptr()[i]));
        }
    }

    BasicString& Replace(const BasicString& old_value, const BasicString& new_value) {
        while (true) {
            BasicString::size_type pos(0);
            if ((pos = find(old_value, 0)) != BasicString::npos) {
                Replace(pos, old_value.length(), new_value);
            }
            else {
                break;
            }
        }
        return *this;
    }

    BasicString& Replace(const _Elem* const old_value, const _Elem* const new_value) {
        while (true) {
            BasicString::size_type pos(0);
            if ((pos = find(old_value, 0)) != BasicString::npos) {
                Replace(pos, _Traits::length(old_value), new_value);
            }
            else {
                break;
            }
        }
        return *this;
    }

    BasicString& Replace(const size_type _Off, const size_type _Nx, _In_z_ const _Elem* const _Ptr) {
        return Replace(_Off, _Nx, _Ptr, _STD _Convert_size<size_type>(_Traits::length(_Ptr)));
    }

    BasicString& Replace(const size_type _Off, size_type _Nx, const size_type _Count, const _Elem _Ch) {
        _Check_offset(_Off);
        _Nx = _Clamp_suffix_size(_Off, _Nx);
        if (_Count == _Nx) {
            _Traits::assign(_Myptr() + _Off, _Count, _Ch);
            return *this;
        }
        const size_type _Old_size = _Mysize();
        if (_Count < _Nx || _Count - _Nx <= _Myres() - _Old_size) {
            const auto _New_size = _Old_size + _Count - _Nx;
            this->intlength = Ulong64toUint32(_New_size);
            _Elem* const _Old_ptr = _Myptr();
            _Elem* const _Insert_at = _Old_ptr + _Off;
            _Traits::move(_Insert_at + _Count, _Insert_at + _Nx, _Old_size - _Nx - _Off + 1);
            _Traits::assign(_Insert_at, _Count, _Ch);
            return *this;
        }
        return _Reallocate_grow_by(
            _Count - _Nx,
            [](_Elem* const _New_ptr, const _Elem* const _Old_ptr, const size_type _Old_size, const size_type _Off,
                const size_type _Nx, const size_type _Count, const _Elem _Ch) {
                    _Traits::copy(_New_ptr, _Old_ptr, _Off);
                    _Traits::assign(_New_ptr + _Off, _Count, _Ch);
                    _Traits::copy(_New_ptr + _Off + _Count, _Old_ptr + _Off + _Nx, _Old_size - _Nx - _Off + 1);
            },
            _Off, _Nx, _Count, _Ch);
    }

    BasicString& Replace(const size_type _Off, size_type _Nx, _In_reads_(_Count) const _Elem* const _Ptr, const size_type _Count) {
        _Check_offset(_Off);
        _Nx = _Clamp_suffix_size(_Off, _Nx);
        if (_Nx == _Count) {
            _Traits::move(_Myptr() + _Off, _Ptr, _Count);
            return *this;
        }
        const size_type _Old_size = _Mysize();
        const size_type _Suffix_size = _Old_size - _Nx - _Off + 1;
        if (_Count < _Nx) {
            _Elem* const _Old_ptr = _Myptr();
            _Elem* const _Insert_at = _Old_ptr + _Off;
            _Traits::move(_Insert_at, _Ptr, _Count);
            _Traits::move(_Insert_at + _Count, _Insert_at + _Nx, _Suffix_size);
            const auto _New_size = _Old_size - (_Nx - _Count);
            this->intlength = Ulong64toUint32(_New_size);
            return *this;
        }
        const size_type _Growth = static_cast<size_type>(_Count - _Nx);
#if _HAS_CXX20
        if (!_STD is_constant_evaluated())
#endif
        {
            if (_Growth <= _Myres() - _Old_size) {
                this->intlength = Ulong64toUint32(_Old_size + _Growth);
                _Elem* const _Old_ptr = _Myptr();
                _Elem* const _Insert_at = _Old_ptr + _Off;
                _Elem* const _Suffix_at = _Insert_at + _Nx;
                size_type _Ptr_shifted_after;
                if (_Ptr + _Count <= _Insert_at || _Ptr > _Old_ptr + _Old_size) {
                    _Ptr_shifted_after = _Count;
                }
                else if (_Suffix_at <= _Ptr) {
                    _Ptr_shifted_after = 0;
                }
                else {
                    _Ptr_shifted_after = static_cast<size_type>(_Suffix_at - _Ptr);
                }
                _Traits::move(_Suffix_at + _Growth, _Suffix_at, _Suffix_size);
                _Traits::move(_Insert_at, _Ptr, _Ptr_shifted_after);
                _Traits::copy(_Insert_at + _Ptr_shifted_after, _Ptr + _Growth + _Ptr_shifted_after, _Count - _Ptr_shifted_after);
                return *this;
            }
        }
        return _Reallocate_grow_by(
            _Growth,
            [](_Elem* const _New_ptr, const _Elem* const _Old_ptr, const size_type _Old_size, const size_type _Off,
                const size_type _Nx, const _Elem* const _Ptr, const size_type _Count) {
                    _Traits::copy(_New_ptr, _Old_ptr, _Off);
                    _Traits::copy(_New_ptr + _Off, _Ptr, _Count);
                    _Traits::copy(_New_ptr + _Off + _Count, _Old_ptr + _Off + _Nx, _Old_size - _Nx - _Off + 1);
            },
            _Off, _Nx, _Ptr, _Count);
    }

    BasicString& Replace(const size_type _Off, const size_type _Nx, const BasicString& _Right) {
        return Replace(_Off, _Nx, _Right._Myptr(), _Right._Mysize());
    }

    BasicString& Replace(const size_type _Off, const size_type _Nx, const std::string& _Right) {
        return Replace(_Off, _Nx, _Right.data(), _Right.length());
    }

    BasicString& Replace(const size_type _Off, size_type _Nx, const BasicString& _Right, const size_type _Roff, size_type _Count = npos) {
        _Right._Check_offset(_Roff);
        _Count = _Right._Clamp_suffix_size(_Roff, _Count);
        return Replace(_Off, _Nx, _Right._Myptr() + _Roff, _Count);
    }

    BasicString& Replace(const const_iterator _First, const const_iterator _Last, _In_z_ const _Elem* const _Ptr) {
        _Adl_verify_range(_First, _Last);
        return Replace(static_cast<size_type>(_STD _Unfancy(_First._Ptr) - _Myptr()), static_cast<size_type>(_Last._Ptr - _First._Ptr), _Ptr);
    }

    BasicString& Replace(const const_iterator _First, const const_iterator _Last, const BasicString& _Right) {
        _Adl_verify_range(_First, _Last);
        return Replace(static_cast<size_type>(_STD _Unfancy(_First._Ptr) - _Myptr()), static_cast<size_type>(_Last._Ptr - _First._Ptr), _Right);
    }

    BasicString& Replace(const const_iterator _First, const const_iterator _Last, const size_type _Count, const _Elem _Ch) {
        _Adl_verify_range(_First, _Last);
        return Replace(static_cast<size_type>(_STD _Unfancy(_First._Ptr) - _Myptr()), static_cast<size_type>(_Last._Ptr - _First._Ptr), _Count, _Ch);
    }

    BasicString& Replace(const const_iterator _First, const const_iterator _Last, _In_reads_(_Count) const _Elem* const _Ptr, const size_type _Count) {
        _Adl_verify_range(_First, _Last);
        return Replace(static_cast<size_type>(_STD _Unfancy(_First._Ptr) - _Myptr()), static_cast<size_type>(_Last._Ptr - _First._Ptr), _Ptr, _Count);
    }

    template <class _StringViewIsh, _Is_string_view_ish<_StringViewIsh> = 0>
    BasicString& Replace(const size_type _Off, const size_type _Nx, const _StringViewIsh& _Right) {
        _STD basic_string_view<_Elem, _Traits> _As_view = _Right;
        return Replace(_Off, _Nx, _As_view.data(), _Convert_size<size_type>(_As_view.size()));
    }

    template <class _StringViewIsh, _Is_string_view_ish<_StringViewIsh> = 0>
    BasicString& Replace(const size_type _Off, const size_type _Nx, const _StringViewIsh& _Right, const size_type _Roff, const size_type _Count = npos) {
        _STD basic_string_view<_Elem, _Traits> _As_view = _Right;
        return Replace(_Off, _Nx, _As_view.substr(_Roff, _Count));
    }

    _CONSTEXPR20 void push_back(const _Elem _Ch) {
        const size_type _Old_size = _Mysize();
        if (_Old_size < _Myres()) {
            this->intlength = Ulong64toUint32(_Old_size + 1);
            _Elem* const _Ptr = _Myptr();
            _Traits::assign(_Ptr[_Old_size], _Ch);
            _Traits::assign(_Ptr[_Old_size + 1], _Elem());
            return;
        }
        _Reallocate_grow_by(
            1,
            [](_Elem* const _New_ptr, const _Elem* const _Old_ptr, const size_type _Old_size, const _Elem _Ch) {
                _Traits::copy(_New_ptr, _Old_ptr, _Old_size);
                _Traits::assign(_New_ptr[_Old_size], _Ch);
                _Traits::assign(_New_ptr[_Old_size + 1], _Elem());
            },
            _Ch);
    }
    _CONSTEXPR20 void pop_back() noexcept {
        const size_type _Old_size = _Mysize();
        _Eos(_Old_size - 1);
    }

    _CONSTEXPR20 void resize(_CRT_GUARDOVERFLOW const size_type _New_size, const _Elem _Ch = _Elem()) {
        const size_type _Old_size = size();
        if (_New_size <= _Old_size) {
            _Eos(_New_size);
        }
        else {
            Append(_New_size - _Old_size, _Ch);
        }
    }

    void reserve(_CRT_GUARDOVERFLOW const size_type _Newcap) {
        if (!_Is_Tidy_deallocate()) {
            const size_type _Old_size = _Mysize();
            size_type _New_capacity = _Newcap;
            if (_New_capacity < _Old_size) _New_capacity = _Old_size;
            const pointer _New_ptr = _Allocate_for_capacity(_New_capacity);
            _Traits::copy(_STD _Unfancy(_New_ptr), _STD _Unfancy(_Myptr()), _Old_size + 1);
            this->str = _New_ptr;
            this->capacity = Ulong64toUint32(_New_capacity);
            return;
        }
        if (_Myres() >= _Newcap) {
            return;
        }
        const size_type _Old_size = _Mysize();
        _Reallocate_grow_by( _Newcap - _Old_size,
            [](_Elem* const _New_ptr, const _Elem* const _Old_ptr, const size_type _Old_size) {
                _Traits::copy(_New_ptr, _Old_ptr, _Old_size + 1);
            });
        this->intlength = Ulong64toUint32(_Old_size);
    }

    void swap(BasicString& _Right) noexcept {
        if (this != _STD addressof(_Right)) {
            _Swap_data(_Right);
        }
    }

    void Split(const _Elem* _Delimiter, Array<BasicString>& _list) {
        _Elem* Safeptr = 0;
        _Elem* split = 0;
        if (IsEmpty()) {
            return;
        }
        split = str_utils::xstrtok(_Myptr(),_Delimiter,&Safeptr);
        while (split) {
            _list.push_back(split);
            split = (_Elem*)str_utils::xstrtok(0,_Delimiter, &Safeptr);
        }
    }

    _NODISCARD _CONSTEXPR20 void Split(const BasicString& _Delimiter, Array<BasicString>& _list) noexcept {
        Split(_Delimiter.c_str(), _list);
    }

    _NODISCARD _CONSTEXPR20 void Split(const std::string& _Delimiter, Array<BasicString>& _list) noexcept {
        Split(_Delimiter.c_str(), _list);
    }

    _NODISCARD _CONSTEXPR20 bool MidStr(_In_z_ const _Elem* _Left, _In_z_ const _Elem* _Right, BasicString& Ret) noexcept {
        auto pos = find(_Left, 0);
        if (pos == npos) {
            return false;
        }
        pos += _Traits::length(_Left);
        auto end = find(_Right, pos);
        if (end == npos || end == pos) {
            return false;
        }
        Ret.Assign(&_Myptr()[pos], &_Myptr()[end]);
        return true;
    }

    _NODISCARD _CONSTEXPR20 bool MidStr(const BasicString& _Left, const BasicString& _Right, BasicString& Ret) noexcept {
        return MidStr(_Left.c_str(), _Right.c_str(), Ret);
    }

    _NODISCARD _CONSTEXPR20 bool MidStr(const std::string& _Left, const std::string& _Right, BasicString& Ret) noexcept {
        return MidStr(_Left.c_str(), _Right.c_str(), Ret);
    }

    _NODISCARD _CONSTEXPR20 bool MoveImpl(_Elem* const _Right, const size_type _Size, const size_type _Actual_allocation_size) {
        _Traits::assign(_Right[_Size], _Elem());
        this->str = _Right;
        this->intlength = Ulong64toUint32(_Size);
        this->capacity = Ulong64toUint32(_Actual_allocation_size - 1);
        return true;
    }

    _NODISCARD _CONSTEXPR20 void shrink_to_fit() {
        size_type _Target_capacity = (_STD min)(_Mysize() | _Alloc_mask, max_size());
        if (_Target_capacity < _Myres()) {
            const pointer _New_ptr = _Allocate_for_capacity(_Target_capacity);
            _Traits::copy(_STD _Unfancy(_New_ptr), _STD _Unfancy(_Myptr()), _Mysize() + 1);
            _Deallocate_for_capacity(_Myptr());
            this->str = _New_ptr;
            this->capacity = Ulong64toUint32(_Target_capacity);
        }
    }

    BasicString& Format(_In_z_ _Printf_format_string_ const _Elem* const Format, ...) {
        va_list _ArgList;
        __crt_va_start(_ArgList, Format);
        return VFormat(Format, _ArgList);
    }

    BasicString& VFormat(_In_z_ _Printf_format_string_ const _Elem* const Format, va_list args) {
        _Elem* Buffer = 0;
        size_type _Count = 0;
        size_type _Length = 0;
        _Count = str_utils::xvscprintf(Format, args);
        if (!_Count || _Count == npos) {
            _Tidy_deallocate();
            return *this;
        }
        _Count++;
        Buffer = MemAlty.allocate<_Elem>(_Count);
        _Length = str_utils::xvsnprintf(Buffer, _Count, Format, args);
        try {
            Assign(Buffer, _Length);
        }
        catch (...) {
            MemAlty.deallocate(Buffer);
            throw;
        }
        MemAlty.deallocate(Buffer);
        return *this;
    }

    _NODISCARD _CONSTEXPR20 std::uint64_t Hash64() const {
        auto offset_basis = 14695981039346656037ull;
        for (auto c : *this) {
            offset_basis ^= c;
            offset_basis *= 1099511628211ull;
        }
        return offset_basis;
    }

    _NODISCARD _CONSTEXPR20 std::uint64_t Hash64A() {
        auto offset_basis = 14695981039346656037ull;
        for (auto _Ch : *this) {
            auto Ch = str_utils::xupper_ascii(_Ch);
            offset_basis ^= Ch;
            offset_basis *= 1099511628211ull;
        }
        return offset_basis;
    }

    _NODISCARD _CONSTEXPR20 std::uint32_t Hash32() {
        auto offset_basis = 2166136261u;
        for (auto c : *this) {
            offset_basis ^= c;
            offset_basis *= 16777619;
        }
        return offset_basis;
    }

    _NODISCARD _CONSTEXPR20 std::uint32_t Hash32A() {
        auto offset_basis = 2166136261u;
        for (auto _Ch : *this) {
            auto Ch = str_utils::xupper_ascii(_Ch);
            offset_basis ^= Ch;
            offset_basis *= 16777619;
        }
        return offset_basis;
    }

    _NODISCARD _CONSTEXPR20 void sort() noexcept {
        std::sort(pointer(_Unchecked_begin()), pointer(_Unchecked_end()));
    }

    _NODISCARD _CONSTEXPR20 void sort_desc() noexcept {
        std::sort(pointer(_Unchecked_begin()), pointer(_Unchecked_end()),
            [](const BasicString& _Left, const BasicString& _Right) {
                return _Left > _Right;
            }
        );
    }

    _NODISCARD _CONSTEXPR20 reference operator [] (const size_type _Off) noexcept {
        return _Myptr()[_Off];
    }

    _NODISCARD _CONSTEXPR20 const_reference operator [] (const size_type _Off) const noexcept {
        return _Myptr()[_Off];
    }

    operator _STD basic_string_view<_Elem, _Traits>() const noexcept {
        return _STD basic_string_view<_Elem, _Traits>{_Myptr(), _Mysize()};
    }

    BasicString& operator = (const BasicString& _Right) {
        if (this == _STD addressof(_Right)) {
            return *this;
        }
        return Assign(_Right._Myptr(), _Right._Mysize());
    }

    BasicString& operator = (const BasicString&& _Right) noexcept {
        if (this == _STD addressof(_Right)) {
            return *this;
        }
        return Assign(_Right._Myptr(), _Right._Mysize());
    }

    template <class U = _Elem, std::enable_if_t<std::is_same_v<U, char>, int> = 0>
    BasicString& operator = (const std::string& _Right) {
        return Assign(_Right);
    }

    template <class U = _Elem, std::enable_if_t<std::is_same_v<U, wchar_t>, int> = 0>
    BasicString& operator = (const std::wstring& _Right) {
        return Assign(_Right);
    }

    BasicString& operator = (_In_z_ const _Elem* const _Ptr) {
        return Assign(_Ptr);
    }

    template <class U = _Elem, std::enable_if_t<std::is_same_v<U, char>, int> = 0>
    BasicString& operator = (_In_z_ const wchar_t* const _Ptr) {
        return Assign(_Ptr);
    }

    BasicString& operator = (const _Elem _Ch) {
        return Assign(1, _Ch);
    }

    BasicString& operator = (_STD initializer_list<_Elem> _Ilist) {
        return Assign(_Ilist.begin(), _STD _Convert_size<size_type>(_Ilist.size()));
    }

    BasicString& operator + (const BasicString& _Right) {
        return Append(_Right);
    }

    BasicString& operator + (const BasicString&& _Right) {
        return Append(_Right);
    }

    BasicString& operator + (const std::string& _Right) {
        return Append(_Right);
    }

    BasicString& operator + (const std::wstring& _Right) {
        return Append(_Right);
    }

    BasicString& operator + (_In_z_ const _Elem* const _Ptr) {
        return Append(_Ptr);
    }

    template <class U = _Elem, std::enable_if_t<std::is_same_v<U, char>, int> = 0>
    BasicString& operator + (_In_z_ const wchar_t* const _Ptr) {
        return Append(_Ptr);
    }

    BasicString& operator + (_Elem _Ch) {
        push_back(_Ch);
        return *this;
    }

    BasicString& operator + (_STD initializer_list<_Elem> _Ilist) {
        return Append(_Ilist.begin(), _STD _Convert_size<size_type>(_Ilist.size()));
    }

    BasicString& operator += (const BasicString& _Right) {
        return operator +(_Right);
    }

    BasicString& operator += (const BasicString&& _Right) {
        return operator +(_Right);
    }

    BasicString& operator += (const std::string& _Right) {
        return operator +(_Right);
    }

    BasicString& operator += (const std::wstring& _Right) {
        return operator +(_Right);
    }

    BasicString& operator += (_In_z_ const _Elem* const _Ptr) {
        return operator +(_Ptr);
    }

    template <class U = _Elem, std::enable_if_t<std::is_same_v<U, char>, int> = 0>
    BasicString& operator += (_In_z_ const wchar_t* const _Ptr) {
        return operator +(_Ptr);
    }

    BasicString& operator += (_Elem _Ch) {
        return operator +(_Ch);
    }

    BasicString& operator += (_STD initializer_list<_Elem> _Ilist) {
        return operator +(_Ilist);
    }

    _NODISCARD _CONSTEXPR20 bool operator <  (const _Elem* _Right) const noexcept {
        return (Compare(_Right) < 0);
    }

    _NODISCARD _CONSTEXPR20 bool operator <  (const BasicString& _Right) const noexcept {
        return operator < (_Right._Myptr());
    }

    _NODISCARD _CONSTEXPR20 bool operator <  (const std::string& _Right) const noexcept {
        return operator < (_Right.c_str());
    }

    _NODISCARD _CONSTEXPR20 bool operator > (const _Elem* _Right)  const noexcept {
        return (Compare(_Right) > 0);
    }

    _NODISCARD _CONSTEXPR20 bool operator > (const BasicString& _Right)  const noexcept {
        return operator >(_Right._Myptr());
    }

    _NODISCARD _CONSTEXPR20 bool operator > (const std::string& _Right)  const noexcept {
        return operator >(_Right.c_str());
    }

    _NODISCARD _CONSTEXPR20 bool operator ! () const noexcept {
        return IsEmpty();
    }

    _NODISCARD _CONSTEXPR20 bool operator != (const _Elem* _Right) const noexcept {
        return !operator ==(_Right);
    }

    _NODISCARD _CONSTEXPR20 bool operator != (const BasicString& _Right) const noexcept {
        return !operator ==(_Right);
    }

    _NODISCARD _CONSTEXPR20 bool operator != (const std::string& _Right) const noexcept {
        return !operator ==(_Right);
    }

    _NODISCARD _CONSTEXPR20 bool operator == (const BasicString& _Right) const noexcept {
        if (_Right._Mysize() != _Mysize()) {
            return false;
        }
        return operator ==(_Right._Myptr());
    }

    _NODISCARD _CONSTEXPR20 bool operator == (const std::string& _Right) const noexcept {
        return operator ==(_Right.c_str());
    }

    _NODISCARD _CONSTEXPR20 bool operator == (const _Elem* _Right) const noexcept {
        return !Compare(_Right);
    }

    _NODISCARD _CONSTEXPR20 int Compare(_In_z_ const _Elem* const _Ptr) const noexcept {
        return _STD _Traits_compare<_Traits>(_Myptr(), _Mysize(), _Ptr, _Traits::length(_Ptr));
    }

    _NODISCARD _CONSTEXPR20 int Compare(_In_z_ const _Elem* const _Ptr, const size_type _Count) const noexcept {
        return _STD _Traits_compare<_Traits>(_Myptr(), _Mysize(), _Ptr, _Count);
    }

    _NODISCARD _CONSTEXPR20 int Compare(const BasicString& _Right) const noexcept {
        return _STD _Traits_compare<_Traits>(_Myptr(), _Mysize(), _Right._Myptr(), _Right._Mysize());
    }

    _NODISCARD _CONSTEXPR20 int Compare(const std::string& _Right) const noexcept {
        return _STD _Traits_compare<_Traits>(_Myptr(), _Mysize(), _Right.data(), _Right.length());
    }

    _NODISCARD _CONSTEXPR20 int Compare(size_type _Off, size_type _Nx, const BasicString& _Right) const {
        _Check_offset(_Off);
        return _STD _Traits_compare<_Traits>(_Myptr() + _Off, _Clamp_suffix_size(_Off, _Nx), _Right._Myptr(), _Right._Mysize());
    }

    _NODISCARD _CONSTEXPR20 int Compare(const size_type _Off, const size_type _Nx, const BasicString& _Right, const size_type _Roff, const size_type _Count = npos) const {
        _Check_offset(_Off);
        _Right._Check_offset(_Roff);
        return _STD _Traits_compare<_Traits>(_Myptr() + _Off, _Clamp_suffix_size(_Off, _Nx), _Right._Myptr() + _Roff, _Right._Clamp_suffix_size(_Roff, _Count));
    }

    _NODISCARD _CONSTEXPR20 int Compare(const size_type _Off, const size_type _Nx, _In_z_ const _Elem* const _Ptr) const {
        _Check_offset(_Off);
        return _STD _Traits_compare<_Traits>(_Myptr() + _Off, _Clamp_suffix_size(_Off, _Nx), _Ptr, _Traits::length(_Ptr));
    }

    _NODISCARD _CONSTEXPR20 int Compare(const size_type _Off, const size_type _Nx, _In_reads_(_Count) const _Elem* const _Ptr, const size_type _Count) const {
        _Check_offset(_Off);
        return _STD _Traits_compare<_Traits>(_Myptr() + _Off, _Clamp_suffix_size(_Off, _Nx), _Ptr, _Count);
    }

#if _HAS_CXX17
    template <class _StringViewIsh, _Is_string_view_ish<_StringViewIsh> = 0>
    _NODISCARD _CONSTEXPR20 int Compare(const _StringViewIsh& _Right) const
        noexcept(_STD _Is_nothrow_convertible_v<const _StringViewIsh&, _STD basic_string_view<_Elem, _Traits>>) {
        _STD basic_string_view<_Elem, _Traits> _As_view = _Right;
        return _STD _Traits_compare<_Traits>(_Myptr(), _Mysize(), _As_view.data(), _As_view.size());
    }

    template <class _StringViewIsh, _Is_string_view_ish<_StringViewIsh> = 0>
    _NODISCARD _CONSTEXPR20 int Compare(const size_type _Off, const size_type _Nx, const _StringViewIsh& _Right) const {
        _STD basic_string_view<_Elem, _Traits> _As_view = _Right;
        _Check_offset(_Off);
        return _STD _Traits_compare<_Traits>(_Myptr() + _Off, _Clamp_suffix_size(_Off, _Nx), _As_view.data(), _As_view.size());
    }

    template <class _StringViewIsh, _Is_string_view_ish<_StringViewIsh> = 0>
    _NODISCARD _CONSTEXPR20 int Compare(const size_type _Off, const size_type _Nx, const _StringViewIsh& _Right, const size_type _Roff, const size_type _Count = npos) const {
        _STD basic_string_view<_Elem, _Traits> _As_view = _Right;
        _Check_offset(_Off);
        const auto _With_substr = _As_view.substr(_Roff, _Count);
        return _STD _Traits_compare<_Traits>(_Myptr() + _Off, _Clamp_suffix_size(_Off, _Nx), _With_substr.data(), _With_substr.size());
    }

#endif
    _NODISCARD _CONSTEXPR20 int CompareI(_In_z_ const _Elem* const _Ptr) const noexcept {
        return str_utils::xstricmp(_Myptr(), _Ptr);
    }

    _NODISCARD _CONSTEXPR20 int CompareI(const BasicString& _Right) const noexcept {
        return CompareI(_Right._Myptr());
    }

    _NODISCARD _CONSTEXPR20 int CompareI(const std::string& _Right) const noexcept {
        return CompareI(_Right.data());
    }

    _NODISCARD _CONSTEXPR20 bool BeginsWith(const _Elem _Ch) const noexcept {
        return !IsEmpty() && _Traits::eq(front(), _Ch);
    }

    _NODISCARD _CONSTEXPR20 bool BeginsWith(const _Elem* const _Right) const noexcept {
        return BeginsWith(_Right, _Traits::length(_Right));
    }

    _NODISCARD _CONSTEXPR20 bool BeginsWith(const BasicString& _Right) const noexcept {
        return BeginsWith(_Right._Myptr(), _Right._Mysize());
    }

    _NODISCARD _CONSTEXPR20 bool BeginsWith(const std::string& _Right) const noexcept {
        return BeginsWith(_Right.data(), _Right.length());
    }

    _NODISCARD _CONSTEXPR20 bool BeginsWith(const _Elem* const _Right, const size_type _Count) const noexcept {
        if (_Count > _Mysize()) {
            return false;
        }
        return _Traits::compare(_Myptr(), _Right, _Count) == 0;
    }

    _NODISCARD _CONSTEXPR20 bool BeginsWithI(const _Elem* const _Right) const noexcept {
        return BeginsWithI(_Right, _Traits::length(_Right));
    }

    _NODISCARD _CONSTEXPR20 bool BeginsWithI(const BasicString& _Right) const noexcept {
        return BeginsWithI(_Right._Myptr(), _Right._Mysize());
    }

    _NODISCARD _CONSTEXPR20 bool BeginsWithI(const std::string& _Right) const noexcept {
        return BeginsWithI(_Right.data(), _Right.length());
    }

    _NODISCARD _CONSTEXPR20 bool BeginsWithI(const _Elem* const _Right, const size_type _Count) const noexcept {
        if (_Count > _Mysize()) {
            return false;
        }
        return str_utils::xStrCmpNI(_Myptr(), _Right, Ulong64toUint32(_Count)) == 0;
    }

    _NODISCARD _CONSTEXPR20 bool EndsWith(const _Elem _Right) const noexcept {
        return !IsEmpty() && _Traits::eq(back(), _Right);
    }

    _NODISCARD _CONSTEXPR20 bool EndsWith(const _Elem* const _Right) const noexcept {
        const auto _Rightsize = _Traits::length(_Right);
        if (_Mysize() < _Rightsize) {
            return false;
        }
        return EndsWith(_Right, _Rightsize);
    }

    _NODISCARD _CONSTEXPR20 bool EndsWith(const BasicString& _Right) const noexcept {
        return EndsWith(_Right._Myptr(), _Right._Mysize());
    }

    _NODISCARD _CONSTEXPR20 bool EndsWith(const std::string& _Right) const noexcept {
        return EndsWith(_Right.c_str(), _Right.length());
    }

    _NODISCARD _CONSTEXPR20 bool EndsWith(const _Elem* const _Right, const size_type _Count) const noexcept {
        if (_Mysize() < _Count) {
            return false;
        }
        return _Traits::compare(_Myptr() + (_Mysize() - _Count), _Right, _Count) == 0;
    }

    _NODISCARD _CONSTEXPR20 bool EndsWithI(const _Elem* const _Right) const noexcept {
        return EndsWithI(_Right, _Traits::length(_Right));
    }

    _NODISCARD _CONSTEXPR20 bool EndsWithI(const BasicString& _Right) const noexcept {
        return EndsWithI(_Right._Myptr(), _Right._Mysize());
    }

    _NODISCARD _CONSTEXPR20 bool EndsWithI(const std::string& _Right) const noexcept {
        return EndsWithI(_Right.c_str(), _Right.length());
    }

    _NODISCARD _CONSTEXPR20 bool EndsWithI(const _Elem* const _Right, const size_type _Count) const noexcept {
        if (_Mysize() < _Count) {
            return false;
        }
        return str_utils::xstricmp(_Myptr() + (_Mysize() - _Count), _Right) == 0;
    }

    _NODISCARD _CONSTEXPR20 bool Equals(_In_z_ const _Elem* const _Ptr) const noexcept {
        return !str_utils::xstrcmp(_Myptr(), _Ptr);
    }

    _NODISCARD _CONSTEXPR20 bool Equals(const BasicString& _Right) const noexcept {
        return _Mysize() == _Right._Mysize() && Equals(_Right._Myptr());
    }

    _NODISCARD _CONSTEXPR20 bool Equals(const std::string& _Right) const noexcept {
        return _Mysize() == _Right.length() && Equals(_Right.data());
    }

    _NODISCARD _CONSTEXPR20 bool EqualsI(_In_z_ const _Elem* const _Ptr) const noexcept {
        return !CompareI(_Ptr);
    }

    _NODISCARD _CONSTEXPR20 bool EqualsI(const BasicString& _Right) const noexcept {
        return EqualsI(_Right._Myptr());
    }

    _NODISCARD _CONSTEXPR20 bool EqualsI(const std::string& _Right) const noexcept {
        return EqualsI(_Right.c_str());
    }

    _CONSTEXPR20 bool _Equal(_In_z_ const _Elem* const _Ptr) const noexcept {
        return _Equal(_Ptr, _Traits::length(_Ptr));
    }

    _CONSTEXPR20 bool _Equal(const BasicString& _Right) const noexcept {
        return _Equal(_Right._Myptr(), _Right._Mysize());
    }

    _CONSTEXPR20 bool _Equal(const std::string& _Right) const noexcept {
        return _Equal(_Right.data(), _Right.length());
    }

    _CONSTEXPR20 bool _Equal(_In_z_ const _Elem* const _Ptr, const size_type _Count) const noexcept {
        return _STD _Traits_equal<_Traits>(_Myptr(), _Mysize(), _Ptr, _Count);
    }

    _NODISCARD _CONSTEXPR20 bool Matches(_In_z_ const _Elem* _Right) noexcept {
        return str_utils::xMatches(_Myptr(), _Right);
    }

    _NODISCARD _CONSTEXPR20 bool Matches(const BasicString& _Right)noexcept {
        return str_utils::xMatches(_Myptr(), _Right.c_str());
    }

    _NODISCARD _CONSTEXPR20 bool Matches(const std::string& _Right)noexcept {
        return str_utils::xMatches(_Myptr(), _Right.c_str());
    }

    _NODISCARD _CONSTEXPR20 bool MatchesI(_In_z_ const _Elem* _Right) noexcept {
        return str_utils::xMatchesI(_Myptr(), _Right);
    }

    _NODISCARD _CONSTEXPR20 bool MatchesI(const BasicString& _Right) noexcept {
        return str_utils::xMatchesI(_Myptr(), _Right.c_str());
    }

    _NODISCARD _CONSTEXPR20 bool MatchesI(const std::string& _Right) noexcept {
        return str_utils::xMatchesI(_Myptr(), _Right.c_str());
    }

    _NODISCARD _CONSTEXPR20 bool contains(const _Elem _Ch) const noexcept {
        return find(_Ch) != npos;
    }

    _NODISCARD _CONSTEXPR20 bool contains(const _Elem* _Ptr) const noexcept {
        return find(_Ptr) != npos;
    }

    _NODISCARD _CONSTEXPR20 bool contains(const BasicString& _Right) const noexcept {
        return find(_Right) != npos;
    }

    _NODISCARD _CONSTEXPR20 void Uplength() {
        if (IsEmpty() && front()) {
            this->intlength = Ulong64toUint32(_Traits::length(_Myptr()));
        }
        else if (front()) {
            auto len = _Traits::length(_Myptr());
            if (len != length()) {
                this->intlength = Ulong64toUint32(len);
            }
        }
    }
#if _HAS_CXX17
    template <class _StringViewIsh, _Is_string_view_ish<_StringViewIsh> = 0>
    _NODISCARD _CONSTEXPR20 size_type find(const _StringViewIsh& _Right, const size_type _Off = 0) const
        noexcept(_STD _Is_nothrow_convertible_v<const _StringViewIsh&, _STD basic_string_view<_Elem, _Traits>>) {
        _STD basic_string_view<_Elem, _Traits> _As_view = _Right;
        return static_cast<size_type>(_STD _Traits_find<_Traits>(_Myptr(), _Mysize(), _Off, _As_view.data(), _As_view.size()));
    }

#endif
    _NODISCARD _CONSTEXPR20 size_type find(const BasicString& _Right, const size_type _Off = 0) const noexcept {
        return static_cast<size_type>(_STD _Traits_find<_Traits>(_Myptr(), _Mysize(), _Off, _Right._Myptr(), _Right._Mysize()));
    }

    _NODISCARD _CONSTEXPR20 size_type find(_In_reads_(_Count) const _Elem* const _Ptr, const size_type _Off,const size_type _Count) const noexcept {
        return static_cast<size_type>(_STD _Traits_find<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Count));
    }

    _NODISCARD _CONSTEXPR20 size_type find(_In_z_ const _Elem* const _Ptr, const size_type _Off = 0) const noexcept {
        return static_cast<size_type>(_STD _Traits_find<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Traits::length(_Ptr)));
    }

    _NODISCARD bool Find(_In_z_ const _Elem* const _Ptr) noexcept {
        if (!_Myptr() || IsEmpty()) {
            return false;
        }
        return str_utils::xstrstr_i(_Myptr(), _Ptr) != 0;
    }

    _NODISCARD _CONSTEXPR20 size_type find(const _Elem _Ch, const size_type _Off = 0) const noexcept {
        return static_cast<size_type>(_STD _Traits_find_ch<_Traits>(_Myptr(), _Mysize(), _Off, _Ch));
    }

#if _HAS_CXX17
    template <class _StringViewIsh, _Is_string_view_ish<_StringViewIsh> = 0>
    _NODISCARD _CONSTEXPR20 size_type rfind(const _StringViewIsh& _Right, const size_type _Off = npos) const
        noexcept(_STD _Is_nothrow_convertible_v<const _StringViewIsh&, _STD basic_string_view<_Elem, _Traits>>) {
        _STD basic_string_view<_Elem, _Traits> _As_view = _Right;
        return static_cast<size_type>(_STD _Traits_rfind<_Traits>(_Myptr(), _Mysize(), _Off, _As_view.data(), _As_view.size()));
    }

#endif
    _NODISCARD _CONSTEXPR20 size_type rfind(const BasicString& _Right, const size_type _Off = npos) const noexcept {
        return static_cast<size_type>(_STD _Traits_rfind<_Traits>(_Myptr(), _Mysize(), _Off, _Right._Myptr(), _Right._Mysize()));
    }

    _NODISCARD _CONSTEXPR20 size_type rfind(_In_reads_(_Count) const _Elem* const _Ptr, const size_type _Off,const size_type _Count) const noexcept {
        return static_cast<size_type>(_STD _Traits_rfind<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Count));
    }

    _NODISCARD _CONSTEXPR20 size_type rfind(_In_z_ const _Elem* const _Ptr, const size_type _Off = npos) const noexcept{
        return static_cast<size_type>(_STD _Traits_rfind<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Traits::length(_Ptr)));
    }

    _NODISCARD _CONSTEXPR20 size_type rfind(const _Elem _Ch, const size_type _Off = npos) const noexcept {
        return static_cast<size_type>(_STD _Traits_rfind_ch<_Traits>(_Myptr(), _Mysize(), _Off, _Ch));
    }

#if _HAS_CXX17
    template <class _StringViewIsh, _Is_string_view_ish<_StringViewIsh> = 0>
    _NODISCARD constexpr size_type find_first_of(const _StringViewIsh& _Right, const size_type _Off = 0) const
        noexcept(_STD _Is_nothrow_convertible_v<const _StringViewIsh&, _STD basic_string_view<_Elem, _Traits>>) {
        _STD basic_string_view<_Elem, _Traits> _As_view = _Right;
        return static_cast<size_type>(_STD _Traits_find_first_of<_Traits>(_Myptr(), _Mysize(), _Off, _As_view.data(), _As_view.size()));
    }

#endif
    _NODISCARD constexpr size_type find_first_of(const BasicString& _Right, const size_type _Off = 0) const noexcept {
        return static_cast<size_type>(_STD _Traits_find_first_of<_Traits>(_Myptr(), _Mysize(), _Off, _Right._Myptr(), _Right._Mysize()));
    }

    _NODISCARD constexpr size_type find_first_of(_In_reads_(_Count) const _Elem* const _Ptr, const size_type _Off,const size_type _Count) const noexcept{
        return static_cast<size_type>(_STD _Traits_find_first_of<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Count));
    }

    _NODISCARD constexpr size_type find_first_of(_In_z_ const _Elem* const _Ptr, const size_type _Off = 0) const noexcept{
        return static_cast<size_type>(_STD _Traits_find_first_of<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Traits::length(_Ptr)));
    }

    _NODISCARD constexpr size_type find_first_of(const _Elem _Ch, const size_type _Off = 0) const noexcept {
        return static_cast<size_type>(_STD _Traits_find_ch<_Traits>(_Myptr(), _Mysize(), _Off, _Ch));
    }

#if _HAS_CXX17
    template <class _StringViewIsh, _Is_string_view_ish<_StringViewIsh> = 0>
    _NODISCARD constexpr size_type find_last_of(const _StringViewIsh& _Right, const size_type _Off = npos) const
        noexcept(_STD _Is_nothrow_convertible_v<const _StringViewIsh&, _STD  basic_string_view<_Elem, _Traits>>) {
        _STD basic_string_view<_Elem, _Traits> _As_view = _Right;
        return static_cast<size_type>(_STD _Traits_find_last_of<_Traits>(_Myptr(), _Mysize(), _Off, _As_view.data(), _As_view.size()));
    }

#endif
    _NODISCARD constexpr size_type find_last_of(const BasicString& _Right, size_type _Off = npos) const noexcept {
        return static_cast<size_type>(_STD _Traits_find_last_of<_Traits>(_Myptr(), _Mysize(), _Off, _Right._Myptr(), _Right._Mysize()));
    }

    _NODISCARD constexpr size_type find_last_of(_In_reads_(_Count) const _Elem* const _Ptr, const size_type _Off,
        const size_type _Count) const noexcept  {
        return static_cast<size_type>(_STD _Traits_find_last_of<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Count));
    }

    _NODISCARD constexpr size_type find_last_of(
        _In_z_ const _Elem* const _Ptr, const size_type _Off = npos) const noexcept  {
        return static_cast<size_type>(_STD _Traits_find_last_of<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Traits::length(_Ptr)));
    }

    _NODISCARD constexpr size_type find_last_of(const _Elem _Ch, const size_type _Off = npos) const noexcept {
        return static_cast<size_type>(_STD _Traits_rfind_ch<_Traits>(_Myptr(), _Mysize(), _Off, _Ch));
    }

#if _HAS_CXX17
    template <class _StringViewIsh, _Is_string_view_ish<_StringViewIsh> = 0>
    _NODISCARD constexpr size_type find_first_not_of(const _StringViewIsh& _Right, const size_type _Off = 0) const
        noexcept(_STD _Is_nothrow_convertible_v<const _StringViewIsh&, _STD basic_string_view<_Elem, _Traits>>) {
        _STD basic_string_view<_Elem, _Traits> _As_view = _Right;
        return static_cast<size_type>(_STD _Traits_find_first_not_of<_Traits>(_Myptr(), _Mysize(), _Off, _As_view.data(), _As_view.size()));
    }

#endif
    _NODISCARD constexpr size_type find_first_not_of(const BasicString& _Right, const size_type _Off = 0) const noexcept {
        return static_cast<size_type>(_STD _Traits_find_first_not_of<_Traits>(_Myptr(), _Mysize(), _Off, _Right._Myptr(), _Right._Mysize()));
    }

    _NODISCARD constexpr size_type find_first_not_of(_In_reads_(_Count) const _Elem* const _Ptr,
        const size_type _Off, const size_type _Count) const noexcept  {
        return static_cast<size_type>(_STD _Traits_find_first_not_of<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Count));
    }

    _NODISCARD constexpr size_type find_first_not_of(
        _In_z_ const _Elem* const _Ptr, size_type _Off = 0) const noexcept
         {
        return static_cast<size_type>(_STD _Traits_find_first_not_of<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Traits::length(_Ptr)));
    }

    _NODISCARD constexpr size_type find_first_not_of(const _Elem _Ch, const size_type _Off = 0) const noexcept {
        return static_cast<size_type>(_STD _Traits_find_not_ch<_Traits>(_Myptr(), _Mysize(), _Off, _Ch));
    }

#if _HAS_CXX17
    template <class _StringViewIsh, _Is_string_view_ish<_StringViewIsh> = 0>
    _NODISCARD constexpr size_type find_last_not_of(const _StringViewIsh& _Right, const size_type _Off = npos) const
        noexcept(_STD _Is_nothrow_convertible_v<const _StringViewIsh&, _STD basic_string_view<_Elem, _Traits>>) {
        _STD basic_string_view<_Elem, _Traits> _As_view = _Right;
        return static_cast<size_type>(_STD _Traits_find_last_not_of<_Traits>(_Myptr(), _Mysize(), _Off, _As_view.data(), _As_view.size()));
    }

#endif
    _NODISCARD constexpr size_type find_last_not_of(
        const BasicString& _Right, const size_type _Off = npos) const noexcept {
        return static_cast<size_type>(_STD _Traits_find_last_not_of<_Traits>(_Myptr(), _Mysize(), _Off, _Right._Myptr(), _Right._Mysize()));
    }

    _NODISCARD constexpr size_type find_last_not_of(_In_reads_(_Count) const _Elem* const _Ptr, const size_type _Off,
        const size_type _Count) const noexcept  {
        return static_cast<size_type>(_STD _Traits_find_last_not_of<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Count));
    }

    _NODISCARD constexpr size_type find_last_not_of(
        _In_z_ const _Elem* const _Ptr, const size_type _Off = npos) const noexcept {
        return static_cast<size_type>(_STD _Traits_find_last_not_of<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Traits::length(_Ptr)));
    }

    _NODISCARD constexpr size_type find_last_not_of(const _Elem _Ch, const size_type _Off = npos) const noexcept {
        return static_cast<size_type>(_STD _Traits_rfind_not_ch<_Traits>(_Myptr(), _Mysize(), _Off, _Ch));
    }

    _NODISCARD _CONSTEXPR20 size_type Capacity() const noexcept {
        return _Myres();
    }

    _NODISCARD_EMPTY_MEMBER _CONSTEXPR20 bool IsEmpty() const noexcept {
        return !_Mysize();
    }

    _NODISCARD _CONSTEXPR20 _Elem* _Unchecked_begin() noexcept {
        return _Myptr();
    }

    _NODISCARD _CONSTEXPR20 const _Elem* _Unchecked_begin() const noexcept {
        return _Myptr();
    }

    _NODISCARD _CONSTEXPR20 _Elem* _Unchecked_end() noexcept {
        return _Myptr() + _Mysize();
    }

    _NODISCARD _CONSTEXPR20 const _Elem* _Unchecked_end() const noexcept {
        return _Myptr() + _Mysize();
    }

    _NODISCARD _CONSTEXPR20 iterator begin() noexcept {
        return iterator(_STD _Refancy<pointer>(_Unchecked_begin()), 0);
    }

    _NODISCARD _CONSTEXPR20 const_iterator begin() const noexcept {
        return const_iterator(_STD _Refancy<const_pointer>(_Unchecked_begin()), 0);
    }

    _NODISCARD _CONSTEXPR20 iterator end() noexcept {
        return iterator(_STD _Refancy<pointer>(_Unchecked_end()), 0);
    }

    _NODISCARD _CONSTEXPR20 const_iterator end() const noexcept {
        return const_iterator(_STD _Refancy<const_pointer>(_Unchecked_end()), 0);
    }

    _NODISCARD _CONSTEXPR20 reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    _NODISCARD _CONSTEXPR20 const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    _NODISCARD _CONSTEXPR20 reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    _NODISCARD _CONSTEXPR20 const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    _NODISCARD _CONSTEXPR20 const_iterator cbegin() const noexcept {
        return begin();
    }

    _NODISCARD _CONSTEXPR20 const_iterator cend() const noexcept {
        return end();
    }

    _NODISCARD _CONSTEXPR20 const_reverse_iterator crbegin() const noexcept {
        return rbegin();
    }

    _NODISCARD _CONSTEXPR20 const_reverse_iterator crend() const noexcept {
        return rend();
    }

    _NODISCARD _CONSTEXPR20 reference at(const size_type _Off) {
        _Check_offset_exclusive(_Off);
        return _Myptr()[_Off];
    }

    _NODISCARD _CONSTEXPR20 const_reference at(const size_type _Off) const {
        _Check_offset_exclusive(_Off);
        return _Myptr()[_Off];
    }

    _NODISCARD _CONSTEXPR20 reference front() noexcept {
        return _Myptr()[0];
    }

    _NODISCARD _CONSTEXPR20 const_reference front() const noexcept {
        return _Myptr()[0];
    }

    _NODISCARD _CONSTEXPR20 reference back() noexcept {
        return _Myptr()[_Mysize() - 1];
    }

    _NODISCARD _CONSTEXPR20 const_reference back() const noexcept {
        return _Myptr()[_Mysize() - 1];
    }

    _NODISCARD _CONSTEXPR20 _Ret_z_ const _Elem* c_str() const noexcept {
        return _Myptr();
    }

    _NODISCARD _CONSTEXPR20 _Ret_z_ const _Elem* data() const noexcept {
        return _Myptr();
    }

    _NODISCARD _CONSTEXPR20 _Ret_z_ _Elem* data() noexcept {
        return _Myptr();
    }

    void Print() const noexcept {
        if constexpr (std::is_same_v<_Elem, char>)
            puts(_Myptr());
        else
            _putws(_Myptr());
    }

    _NODISCARD _CONSTEXPR20 size_type length() const noexcept {
        return _Mysize();
    }

    _NODISCARD _CONSTEXPR20 size_type size() const noexcept {
        return _Mysize();
    }

    _NODISCARD _CONSTEXPR20 size_type max_size() const noexcept {
        _Alty _Al;
        const size_type _Alloc_max = _Alty_traits::max_size(_Al);
        const size_type _Storage_max = (_STD max)(_Alloc_max, static_cast<size_type>(_BUF_SIZE));
        return (_STD min)(static_cast<size_type>((_STD numeric_limits<difference_type>::max)()),_Storage_max - 1);
    }

    _CONSTEXPR20 size_type copy(_Out_writes_(_Count) _Elem* const _Ptr, size_type _Count, const size_type _Off = 0) const {
        _Check_offset(_Off);
        _Count = _Clamp_suffix_size(_Off, _Count);
        _Traits::copy(_Ptr, _Myptr() + _Off, _Count);
        return _Count;
    }

    _NODISCARD _CONSTEXPR20 BasicString substr(const size_type _Off = 0, const size_type _Count = npos) const {
        return BasicString(*this, _Off, _Count);
    }
protected:
    pointer str;
    std::uint32_t intlength;
    std::uint32_t capacity;
private:
    _CONSTEXPR20 void _Construct_empty() {
        if constexpr (std::is_same_v<_Elem, char>)
            this->str = const_cast<char*>(a_EmptyString);
        else
            this->str = const_cast<wchar_t*>(w_EmptyString);
        this->intlength = 0;
        this->capacity = 0;
    }

    _NODISCARD _CONSTEXPR20 pointer _Allocate_for_capacity(size_type& _Capacity) {
        _STL_INTERNAL_CHECK(_Capacity > _Small_string_capacity);
        ++_Capacity;
        pointer _Fancy_ptr = MemAlty.allocate<_Elem>(_Capacity);
        --_Capacity;
        return _Fancy_ptr;
    }

    _CONSTEXPR20 void _Deallocate_for_capacity(const pointer _Old_ptr) noexcept {
        MemAlty.deallocate(_Old_ptr);
    }

    _NODISCARD bool _Is_Tidy_deallocate() const noexcept {
        if constexpr (std::is_same_v<_Elem, char>)
            return ((_Myres() & MEM_MUST_BE_FREED_FLAG) == MEM_MUST_BE_FREED_FLAG) && _Myptr() != 0 && _Myptr() != const_cast<char*>(a_EmptyString);
        else
            return ((_Myres() & MEM_MUST_BE_FREED_FLAG) == MEM_MUST_BE_FREED_FLAG) && _Myptr() != 0 && _Myptr() != const_cast<wchar_t*>(w_EmptyString);
    }

    _NODISCARD void _Tidy_deallocate() noexcept {
        if (_Is_Tidy_deallocate()) {
            _Deallocate_for_capacity(_Myptr());
        }
        _Construct_empty();
    }

    template <class _Fty, class... _ArgTys>
    _CONSTEXPR20 BasicString& _Reallocate_for(const size_type _New_size, _Fty _Fn, _ArgTys... _Args) {
        if (_New_size > max_size()) {
            _STD _Xlen_string();
        }
        size_type _New_capacity = _Calculate_growth(_New_size);
        const pointer _New_ptr = _Allocate_for_capacity(_New_capacity);
        this->intlength = Ulong64toUint32(_New_size);
        this->capacity = Ulong64toUint32(_New_capacity);
        _Fn(_STD _Unfancy(_New_ptr), _New_size, _Args...);
        if (_Is_Tidy_deallocate()) {
            _Deallocate_for_capacity(_Myptr());
        }
        this->str = _New_ptr;
        return *this;
    }

    template <class _Fty, class... _ArgTys>
    _CONSTEXPR20 BasicString& _Reallocate_grow_by(const size_type _Size_increase, _Fty _Fn, _ArgTys... _Args) {
        const size_type _Old_size = _Mysize();
        if (max_size() - _Old_size < _Size_increase) {
            _STD _Xlen_string();
        }
        const size_type _New_size = _Old_size + _Size_increase;
        size_type _New_capacity = _Calculate_growth(_New_size);
        const pointer _New_ptr = _Allocate_for_capacity(_New_capacity);
        this->intlength = Ulong64toUint32(_New_size);
        this->capacity = Ulong64toUint32(_New_capacity);
        const pointer _Old_ptr = _Myptr();
        _Elem* const _Raw_new = _STD _Unfancy(_New_ptr);
        _Fn(_Raw_new, _STD _Unfancy(_Old_ptr), _Old_size, _Args...);
        if (_Is_Tidy_deallocate()) {
            _Deallocate_for_capacity(_Myptr());
        }
        this->str = _New_ptr;
        return *this;
    }

    BasicString& _Erase_noexcept(const size_type _Off, size_type _Count) noexcept {
        _Count = _Clamp_suffix_size(_Off, _Count);
        const size_type _Old_size = _Mysize();
        _Elem* const _My_ptr = _Myptr();
        _Elem* const _Erase_at = _My_ptr + _Off;
        const size_type _New_size = _Old_size - _Count;
        _Traits::move(_Erase_at, _Erase_at + _Count, _New_size - _Off + 1);
        this->intlength = Ulong64toUint32(_New_size);
        return *this;
    }

    void _Eos(const size_type _New_size) noexcept {
        if (_Is_Tidy_deallocate()) {
            this->intlength = Ulong64toUint32(_New_size);
            _Traits::assign(_Myptr()[_New_size], _Elem());
        }
    }

    _CONSTEXPR20 void _Check_offset_exclusive(const size_type _Off) const {
        if (_Mysize() <= _Off) {
            _Scary_val::_Xran();
        }
    }

    _CONSTEXPR20 void _Check_offset(const size_type _Off) const {
        _Check_offset_exclusive(_Off);
    }

    _NODISCARD static _CONSTEXPR20 size_type _Calculate_growth(const size_type _Requested, const size_type _Old, const size_type _Max)
        noexcept {
        const size_type _Masked = _Requested | _Alloc_mask;
        if (_Masked > _Max) {
            return _Max;
        }
        if (_Old > _Max - _Old / 2) {
            return _Max;
        }
        return (_STD max)(_Masked, _Old + _Old / 2);
    }

    _NODISCARD _CONSTEXPR20 size_type _Calculate_growth(const size_type _Requested) const noexcept {
        return _Calculate_growth(_Requested, _Myres(), max_size());
    }

    _CONSTEXPR20 void _Swap_data(BasicString& _Right) noexcept {
        using _STD swap;
        _STD swap(this->str, _Right.str);
        _STD swap(this->intlength, _Right.intlength);
        _STD swap(this->capacity, _Right.capacity);
    }

    _CONSTEXPR20 _Pre_satisfies_(_Dest_size >= _Count) size_type _Copy_s(_Out_writes_all_(_Dest_size) _Elem* const _Dest, const size_type _Dest_size, size_type _Count, const size_type _Off = 0) const {
        _Check_offset(_Off);
        _Count = _Clamp_suffix_size(_Off, _Count);
        _Traits::_Copy_s(_Dest, _Dest_size, _Myptr() + _Off, _Count);
        return _Count;
    }

    _NODISCARD size_type _Clamp_suffix_size(const size_type _Off, const size_type _Size) noexcept {
        return (_STD min)(_Size, _Mysize() - _Off);
    }

    _NODISCARD _CONSTEXPR20 const size_type _Clamp_suffix_size(const size_type _Off, const size_type _Size) const noexcept {
        return (_STD min)(_Size, _Mysize() - _Off);
    }

    _NODISCARD _CONSTEXPR20 value_type* _Myptr() noexcept {
        return this->str;
    }

    _NODISCARD _CONSTEXPR20 const value_type* _Myptr() const noexcept {
        return this->str;
    }

    _NODISCARD _CONSTEXPR20 size_type _Mysize() noexcept {
        return this->intlength;
    }

    _NODISCARD _CONSTEXPR20 const size_type _Mysize() const noexcept {
        return this->intlength;
    }

    _NODISCARD _CONSTEXPR20 size_type _Myres() noexcept {
        return this->capacity;
    }

    _NODISCARD _CONSTEXPR20 const size_type _Myres() const noexcept {
        return this->capacity;
    }
};

template <class _Elem>
_NODISCARD BasicString<_Elem> operator+(const BasicString<_Elem>& _Left, const BasicString<_Elem>& _Right) {
    BasicString<_Elem> _Ans(_Left);
    _Ans.Append(_Right);
    return _Ans;
}

template <class _Elem>
_NODISCARD BasicString<_Elem> operator+(const BasicString<_Elem>& _Left, const _Elem* _Right) {
    BasicString<_Elem> _Ans(_Left);
    _Ans.Append(_Right);
    return _Ans;
}

template <class _Elem>
_NODISCARD BasicString<_Elem> operator+(const _Elem* _Left, const BasicString<_Elem>& _Right) {
    BasicString<_Elem> _Ans(_Left);
    _Ans.Append(_Right);
    return _Ans;
}

template <class _Elem>
_NODISCARD BasicString<_Elem> operator+(const BasicString<_Elem>& _Left, _Elem _Ch) {
    BasicString<_Elem> _Ans(_Left);
    _Ans.push_back(_Ch);
    return _Ans;
}

template <class _Elem>
_NODISCARD BasicString<_Elem> operator+(_Elem _Ch, const BasicString<_Elem>& _Right) {
    BasicString<_Elem> _Ans(1, _Ch);
    _Ans.Append(_Right);
    return _Ans;
}

void swap(AString& _Left, AString& _Right) noexcept;

void swap(WString& _Left, WString& _Right) noexcept;

_NODISCARD int stoi(const AString& _Str, std::size_t* _Idx = nullptr, int _Base = 10);

_NODISCARD long stol(const AString& _Str, size_t* _Idx = nullptr, int _Base = 10);

_NODISCARD unsigned long stoul(const AString& _Str, size_t* _Idx = nullptr, int _Base = 10);

_NODISCARD long long stoll(const AString& _Str, size_t* _Idx = nullptr, int _Base = 10);

_NODISCARD unsigned long long stoull(const AString& _Str, size_t* _Idx = nullptr, int _Base = 10);

_NODISCARD float stof(const AString& _Str, size_t* _Idx = nullptr);

_NODISCARD double stod(const AString& _Str, size_t* _Idx = nullptr);

_NODISCARD long double stold(const AString& _Str, size_t* _Idx = nullptr);

_NODISCARD int stoi(const WString& _Str, size_t* _Idx = nullptr, int _Base = 10);

_NODISCARD long stol(const WString& _Str, size_t* _Idx = nullptr, int _Base = 10);

_NODISCARD unsigned long stoul(const WString& _Str, size_t* _Idx = nullptr, int _Base = 10);

_NODISCARD long long stoll(const WString& _Str, size_t* _Idx = nullptr, int _Base = 10);

_NODISCARD unsigned long long stoull(const WString& _Str, size_t* _Idx = nullptr, int _Base = 10);

_NODISCARD float stof(const WString& _Str, size_t* _Idx = nullptr);

_NODISCARD double stod(const WString& _Str, size_t* _Idx = nullptr);

_NODISCARD long double stold(const WString& _Str, size_t* _Idx = nullptr);

template <class _Elem, class _Ty>
_NODISCARD _Elem _Integral_to_string(const _Ty _Val) {
    static_assert(_STD is_integral_v<_Ty>, "_Ty must be integral");
    using _UTy = _STD make_unsigned_t<_Ty>;
    typename _Elem::value_type _Buff[21];
    typename _Elem::value_type* const _Buff_end = _STD end(_Buff);
    typename _Elem::value_type* _RNext = _Buff_end;
    const auto _UVal = static_cast<_UTy>(_Val);
    if (_Val < 0) {
        _RNext = _STD _UIntegral_to_buff(_RNext, 0 - _UVal);
        *--_RNext = '-';
    }
    else {
        _RNext = _STD _UIntegral_to_buff(_RNext, _UVal);
    }
    return _Elem(_RNext, _Buff_end);
}

template <class _Elem, class _Ty>
_NODISCARD _Elem _UIntegral_to_string(const _Ty _Val) {
    static_assert(_STD is_integral_v<_Ty>, "_Ty must be integral");
    static_assert(_STD is_unsigned_v<_Ty>, "_Ty must be unsigned");
    typename _Elem::value_type _Buff[21];
    typename _Elem::value_type* const _Buff_end = _STD end(_Buff);
    typename _Elem::value_type* const _RNext = _STD _UIntegral_to_buff(_Buff_end, _Val);
    return _Elem(_RNext, _Buff_end);
}

_NODISCARD AString to_AString(int _Val);

_NODISCARD AString to_AString(unsigned int _Val);

_NODISCARD AString to_AString(long _Val);

_NODISCARD AString to_AString(unsigned long _Val);

_NODISCARD AString to_AString(long long _Val);

_NODISCARD AString to_AString(unsigned long long _Val);

_NODISCARD AString to_AString(double _Val);

_NODISCARD AString to_AString(float _Val);

_NODISCARD AString to_AString(long double _Val);

_NODISCARD WString to_WString(int _Val);

_NODISCARD WString to_WString(unsigned int _Val);

_NODISCARD WString to_WString(long _Val);

_NODISCARD WString to_WString(unsigned long _Val);

_NODISCARD WString to_WString(long long _Val);

_NODISCARD WString to_WString(unsigned long long _Val);

_NODISCARD WString to_WString(double _Val);

_NODISCARD WString to_WString(float _Val);

_NODISCARD WString to_WString(long double _Val);

AString& operator<<(AString& _Left, const char* _Right);

AString& operator<<(AString& _Left, const AString& _Right);

AString& operator<<(AString& _Left, int _Val);

AString& operator<<(AString& _Left, unsigned int _Val);

AString& operator<<(AString& _Left, long _Val);

AString& operator<<(AString& _Left, unsigned long _Val);

AString& operator<<(AString& _Left, long long _Val);

AString& operator<<(AString& _Left, unsigned long long _Val);

AString& operator<<(AString& _Left, double _Val);

AString& operator<<(AString& _Left, float _Val);

AString& operator<<(AString& _Left, long double _Val);

WString& operator<<(WString& _Left, const WString& _Right);

WString& operator<<(WString& _Left, const wchar_t* _Right);

WString& operator<<(WString& _Left, int _Val);

WString& operator<<(WString& _Left, unsigned int _Val);

WString& operator<<(WString& _Left, long _Val);

WString& operator<<(WString& _Left, unsigned long _Val);

WString& operator<<(WString& _Left, long long _Val);

WString& operator<<(WString& _Left, unsigned long long _Val);

WString& operator<<(WString& _Left, double _Val);

WString& operator<<(WString& _Left, float _Val);

WString& operator<<(WString& _Left, long double _Val);

AString& operator+(AString& _Left, const wchar_t* const _Right);

_NODISCARD std::size_t PreNUm(std::uint8_t byte);

_NODISCARD bool AStringIsUTF8(std::uint8_t* Data, std::size_t _Count);

AString& Utf8_to_AString(AString& _Right);

AString& Utf8_to_AString(AString& _Left, WString& _Right);

template <std::uint32_t Count = 256, class _Elem = char>
class StackString : public BasicString<_Elem> {
public:
    explicit StackString() {
        CheckStack;
        _Construct_empty_();
    }

    explicit StackString(const _Elem* _Right) {
        CheckStack;
        _Construct_empty_();
        BasicString<_Elem>::Assign(_Right);
    }

    explicit StackString(const BasicString<_Elem>& _Right) {
        CheckStack;
        _Construct_empty_();
        BasicString<_Elem>::Assign(_Right);
    }

    explicit StackString(const BasicString<_Elem>&& _Right) {
        CheckStack;
        _Construct_empty_();
        BasicString<_Elem>::Assign(_Right);
    }

    explicit StackString(const std::string& _Right) {
        CheckStack;
        _Construct_empty_();
        BasicString<_Elem>::Assign(_Right);
    }

    explicit StackString(const _Elem* _Right, const _Elem* _RightEnd) {
        CheckStack;
        _Construct_empty_();
        BasicString<_Elem>::Assign(_Right, _RightEnd);
    }

    explicit StackString(const StackString& _Right) {
        CheckStack;
        _Construct_empty_();
        BasicString<_Elem>::Assign(static_cast<const BasicString<_Elem>&>(_Right));
    }

    inline ~StackString() = default;

    StackString& operator = (const _Elem* _Right) {
        BasicString<_Elem>::Assign(_Right);
        return *this;
    }

    template <class U = _Elem, std::enable_if_t<std::is_same_v<U, char>, int> = 0>
    StackString& operator = (const wchar_t* _Right) {
        BasicString<_Elem>::Assign(_Right);
        return *this;
    }

    template <class U = _Elem, std::enable_if_t<std::is_same_v<U, wchar_t>, int> = 0>
    StackString& operator = (const char* _Right) {
        BasicString<_Elem>::Assign(_Right);
        return *this;
    }

    StackString& operator = (const BasicString<_Elem>& _Right) {
        BasicString<_Elem>::Assign(_Right);
        return *this;
    }

    template <class U = _Elem, std::enable_if_t<std::is_same_v<U, char>, int> = 0>
    StackString& operator = (const BasicString<wchar_t>& _Right) {
        BasicString<_Elem>::Assign(_Right);
        return *this;
    }

    StackString& operator = (const std::string& _Right) {
        BasicString<_Elem>::Assign(_Right);
        return *this;
    }

    StackString& operator = (const std::wstring& _Right) {
        BasicString<_Elem>::Assign(_Right);
        return *this;
    }
private:
    _Elem Buffer[Count] = { 0 };
private:
    void _Construct_empty_() {
        this->str = Buffer;
        this->capacity = Count;
        this->intlength = 0;
    }
};

template <std::uint32_t Count = 256>
using AStackString = StackString<Count, char>;

template <std::uint32_t Count = 256>
using WStackString = StackString<Count, wchar_t>;

template <class _Elem, class _Traits, class _Alloc = std::allocator<_Elem>>
std::basic_ostream<_Elem, _Traits>& operator<<(
    std::basic_ostream<_Elem, _Traits>& _Ostr, const BasicString<_Elem>& _Str) {
    _Ostr << _Str.c_str();
    return _Ostr;
}

template <class _Elem, class _Traits, class _Alloc = std::allocator<_Elem>>
std::basic_istream<_Elem, _Traits>& operator>>(
    std::basic_istream<_Elem, _Traits>& _Istr, BasicString<_Elem>& _Str) {
    _Str.Clear();
    typename std::basic_istream<_Elem, _Traits>::sentry _Ok(_Istr);
    if (_Ok) {
        _Elem _Ch;
        while (_Istr.get(_Ch)) {
            if (_Ch == _Elem(' ') || _Ch == _Elem('\t') || _Ch == _Elem('\n') || _Ch == _Elem('\r'))
                break;
            _Str.push_back(_Ch);
        }
    }
    return _Istr;
}

template <class _Elem, class _Traits, class _Alloc = std::allocator<_Elem>>
std::basic_istream<_Elem, _Traits>& getline(
    std::basic_istream<_Elem, _Traits>& _Istr, BasicString<_Elem>& _Str, _Elem _Delim) {
    _Str.Clear();
    typename std::basic_istream<_Elem, _Traits>::sentry _Ok(_Istr, true);
    if (_Ok) {
        while (_Istr) {
            const auto _Meta = _Istr.rdbuf()->sbumpc();
            if (_Traits::eq_int_type(_Traits::eof(), _Meta)) {
                _Istr.setstate(std::ios_base::eofbit);
                break;
            }
            const auto _Ch = _Traits::to_char_type(_Meta);
            if (_Traits::eq(_Ch, _Delim)) break;
            if (_Str.max_size() <= _Str.size()) {
                _Istr.setstate(std::ios_base::failbit);
                break;
            }
            _Str.push_back(_Ch);
        }
    }
    if (!_Str.size()) _Istr.setstate(std::ios_base::failbit);
    return _Istr;
}

template <class _Elem, class _Traits, class _Alloc = std::allocator<_Elem>>
std::basic_istream<_Elem, _Traits>& getline(
    std::basic_istream<_Elem, _Traits>& _Istr, BasicString<_Elem>& _Str) {
    return getline(_Istr, _Str, _Istr.widen('\n'));
}

_STD_BEGIN

template <>
struct hash<AString> {
    _NODISCARD size_t operator()(const AString& _Keyval) const noexcept {
        return _Keyval.Hash64();
    }
};

template <>
struct hash<WString> {
    _NODISCARD size_t operator()(const WString& _Keyval) const noexcept {
        return _Keyval.Hash64();
    }
};

_STD_END


#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif
