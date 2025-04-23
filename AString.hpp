#ifndef _AString__  
#define _AString__  


#include <iostream>  
#include <string>  
#include <cstdint>  
#include <vector>  
#include <iterator>  
#include <algorithm>  
#include <unordered_map>
#include <map>
#include <memory>  
#include <mutex>
#include <Shlwapi.h>

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

#undef _CONSTEXPR20

#if _HAS_CXX20
#ifndef _CONSTEXPR20
#define _CONSTEXPR20 
#endif 
#else
#ifndef _CONSTEXPR20
#define _CONSTEXPR20 
#endif
#endif

#define CheckStack static_assert((Count % 2) == 0, "Capacity must be multiple of 2")

static const char* const a_EmptyString{ "" };

static const wchar_t* const w_EmptyString{ L"" };

class MemContainer {
public:
    MemContainer() noexcept  { std::lock_guard<std::mutex> lock(Mutex); }
    MemContainer(const MemContainer&) noexcept = default;
    ~MemContainer() noexcept { 
        std::lock_guard<std::mutex> lock(Mutex);
        for (auto& [ptr, size] : _Mypair) {
            if (size) {
                std::memset(ptr, 0, size);
            }
            ::operator delete(ptr);
        }
        _Mypair.clear();
    }

    template <class _Elem>
    _NODISCARD_RAW_PTR_ALLOC __declspec(allocator) _Elem* allocate(const std::size_t _Count) {
#undef max
        if (_Count == 0 || _Count > (std::numeric_limits<size_t>::max() / sizeof(_Elem)) - 1) {
            throw std::bad_array_new_length();
        }
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif 
        _Elem* _Ptr = nullptr;
        std::lock_guard<std::mutex> lock(Mutex);

        try {
            auto _Memory = std::make_unique_for_overwrite<_Elem[]>(_Count);
            _Ptr = _Memory.release();
            _Mypair[reinterpret_cast<std::uint8_t*>(_Ptr)] = _Count * sizeof(_Elem);
        }
        catch (...) {
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

    void deallocate(std::uint8_t* _Ptr) noexcept {
        if (!_Ptr) { 
            return; 
        }

        std::lock_guard<std::mutex> lock(Mutex);
        auto it = _Mypair.find(_Ptr);
        if (it != _Mypair.end()) { 
            if (it->second) {
                std::memset(_Ptr, 0, it->second);
            }

            ::operator delete(_Ptr);
            _Mypair.erase(it);           
        }
    }

    _NODISCARD std::size_t size(const wchar_t* _Ptr) noexcept {
        auto Size = size(reinterpret_cast<std::uint8_t*>(const_cast<wchar_t*>(_Ptr)));
        return (Size != -1) ? Size / 2 : -1;
    }

    _NODISCARD std::size_t size(const char* _Ptr) noexcept {
        auto Size = size(reinterpret_cast<std::uint8_t*>(const_cast<char*>(_Ptr)));
        return (Size != -1) ? Size / 1 : -1;
    }

    _NODISCARD std::size_t size(std::uint8_t* _Ptr) noexcept {
        std::lock_guard<std::mutex> lock(Mutex);
        auto it = _Mypair.find(_Ptr);
        return (it != _Mypair.end()) ? it->second : -1;
    }

protected:
    std::mutex Mutex;
    std::map<std::uint8_t*, std::size_t> _Mypair;
};

static MemContainer MemAlty;
//static std::unique_ptr<MemContainer> MemAlty = std::make_unique<MemContainer>();

template <class _Ty>
constexpr std::uint32_t Ulong64toUint32(_Ty Value) {
    if constexpr (sizeof(_Ty) > 4) {
        return static_cast<std::uint32_t>((Value << 32) >> 32);
    }
    else {
        return static_cast<std::uint32_t>(Value);
    }
}

template <class _Ty>
_CONSTEXPR20 bool _Is_nullptr(const _Ty* const _Ptr) {
    static_assert(std::is_same_v<_Ty, char> || std::is_same_v<_Ty, wchar_t>, "Template argument must be either char or wchar_t");

    try {
        if (_Ptr) {
            return _Ptr[0] != static_cast<_Ty>(0);
        }
        return false;
    }
    catch (...) {
        return false;
    }
}

enum : std::uint32_t {
    MEM_MUST_BE_FREED_FLAG = 0x00000001,
    MEM_VECTOR_FLAG = 0x80000020,
    RESERVED_MASK = 0xFFFFFFFE
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
        std::uint32_t  _Count = 0;
        std::uint32_t  _Length = 0;

        _Length = Ulong64toUint32(wcslen(_Right));
        if (!_Length) {
            return false;
        }

        _Count = WideCharToMultiByte(CP_ACP, 0, (const wchar_t*)(_Right), _Length, 0, 0, 0, 0);
        if (!_Count) {
            return false;
        }

        _Count++;
        Myres = _Count;
        Myptr = MemAlty.allocate<value_type>(Myres);
        if (!Myptr) {
            return false;
        }

        _Count--;

        Mysize = WideCharToMultiByte(CP_ACP, 0, (const wchar_t*)(_Right), _Length, (char*)(Myptr), _Count, 0, 0);
        return Mysize != 0;
    }

    _NODISCARD bool ToAnsi(const wchar_t* _Right, std::uint32_t _Length) {
        std::uint32_t  _Count = 0;

        if (!_Length) {
            return false;
        }

        _Count = WideCharToMultiByte(CP_ACP, 0, (const wchar_t*)(_Right), _Length, 0, 0, 0, 0);

        if (!_Count) {
            return false;
        }

        _Count++;
        Myres = _Count;
        Myptr = MemAlty.allocate<value_type>(Myres);
        if (!Myptr) {
            return false;
        }

        _Count--;

        Mysize = WideCharToMultiByte(CP_ACP, 0, (const wchar_t*)(_Right), _Length, (char*)(Myptr), _Count, 0, 0);
        return Mysize != 0;
    }

    _NODISCARD bool ToUtf8(const char* _Right) {
        std::uint32_t  _Count = 0;
        std::uint32_t  _Length = 0;

        _Length = Ulong64toUint32(strlen(_Right));
        if (!_Length) {
            return false;
        }

        _Count = MultiByteToWideChar(CP_ACP, 0, (const char*)_Right, _Length, 0, 0);
        if (!_Count) {
            return false;
        }

        _Count++;
        Myres = _Count;
        Myptr = MemAlty.allocate<value_type>(Myres);

        if (!Myptr) {
            return false;
        }
        _Count--;


        Mysize = MultiByteToWideChar(CP_ACP, 0, (const char*)_Right, _Length, (wchar_t*)(Myptr), _Count);

        return Mysize != 0;
    }
    
    _NODISCARD bool ToUtf8(const char* _Right, std::uint32_t _Length) {
        std::uint32_t  _Count = 0;

        if (!_Length) {
            return false;
        }

        _Count = MultiByteToWideChar(CP_ACP, 0, (const char*)_Right, _Length, 0, 0);
        if (!_Count) {
            return false;
        }

        _Count++;
        Myres = _Count;
        Myptr = MemAlty.allocate<value_type>(Myres);

        if (!Myptr) {
            return false;
        }
        _Count--;


        Mysize = MultiByteToWideChar(CP_ACP, 0, (const char*)_Right, _Length, (wchar_t*)(Myptr), _Count);

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

class WString;

class AString {
public:
    class Vector {
    public:
        AString* str = 0;
        std::uint32_t Size = 0;
        std::uint32_t Capacity = MEM_VECTOR_FLAG;
    };
public:
    using _Elem = char;
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
    AString() noexcept {
        _Construct_empty();
    }

    AString(const AString& _Right) {
        _Construct_empty();
        Assign(_Right);
    }

    AString(const AString&& _Right) noexcept {
        _Construct_empty();
        Assign(_Right);
    }

    AString(const std::string& _Right) {
        _Construct_empty();
        Assign(_Right);
    }

    AString(const std::wstring& _Right) {
        _Construct_empty();
        Assign(_Right);
    }

    AString(const size_type _Count) {
        _Construct_empty();
        Assign(_Count);
    }
    
    AString(_STD initializer_list<_Elem> _Ilist) {
        _Construct_empty();
        Assign(_Ilist.begin(), _STD _Convert_size<size_type>(_Ilist.size()));
    }

    AString(_In_z_ const _Elem* const _Ptr) {
        _Construct_empty();
        Assign(_Ptr, _STD _Convert_size<size_type>(_Traits::length(_Ptr)));
    }

    AString(const AString& _Right, const size_type _Roff) {
        _Construct_empty();
        _Right._Check_offset(_Roff);
        Assign(_Right._Myptr() + _Roff, _Right._Clamp_suffix_size(_Roff, npos));
    }

    AString(const AString&& _Right, const size_type _Roff) {
        _Construct_empty();
        _Right._Check_offset(_Roff);
        Assign(_Right._Myptr() + _Roff, _Right._Clamp_suffix_size(_Roff, npos));
    }

    AString(const AString& _Right, const size_type _Roff, const size_type _Count) {
        _Construct_empty();
        _Right._Check_offset(_Roff);
        Assign(_Right._Myptr() + _Roff, _Right._Clamp_suffix_size(_Roff, _Count));
    }

    AString(const _Elem* const _Ptr, const _Elem* const _PtrEnd) {
        _Construct_empty();
        Assign(_Ptr, _PtrEnd);
    }

    AString(_In_reads_(_Count) const _Elem* const _Ptr, _CRT_GUARDOVERFLOW const size_type _Count) {
        _Construct_empty();
        Assign(_Ptr, _Count);
    }

    AString(_CRT_GUARDOVERFLOW const size_type _Count, const _Elem _Ch) {
        _Construct_empty();
        Assign(_Count, _Ch);
    }

    ~AString() {
        _Tidy_deallocate();
    }

    AString& Assign(const AString& _Right) {
        return Assign(_Right._Myptr());
    }

    AString& Assign(const AString&& _Right) noexcept {
        return Assign(_Right);
    }
    
    AString& Assign(const WString& _Right);

    AString& Assign(const std::string& _Right) {
        return Assign(_Right.c_str());
    }

    AString& Assign(const std::wstring& _Right) {
        return Assign(_Right.c_str());
    }

    AString& Assign(const size_type _Count) noexcept {
        _Tidy_deallocate();

        size_type _New_capacity = _Count;
        this->str = _Allocate_for_capacity(_New_capacity);
        this->capacity = Ulong64toUint32(_New_capacity);

        return *this;
    }

    AString& Assign(_STD initializer_list<_Elem> _Ilist) {
        return Assign(_Ilist.begin(), _STD _Convert_size<size_type>(_Ilist.size()));
    }

    AString& Assign(_In_z_ const _Elem* const _Ptr) {
        if (!_Is_nullptr(_Ptr)) {
            return *this;
        }

        return Assign(_Ptr, _STD _Convert_size<size_type>(_Traits::length(_Ptr)));
    }

    AString& Assign(const wchar_t* const _Right) {
        EncodeCodecvt<_Elem> Convert;

        if (!Convert.ToAnsi(_Right)) {
            return *this;
        }

        return Assign(Convert._Myptr(), Convert._Mysize());
    }

    AString& Assign(const _Elem* const _Ptr, const _Elem* const _PtrEnd) {
        if (!_Is_nullptr(_Ptr)) {
            return *this;
        }

        return Assign(_Ptr, _STD _Convert_size<size_type>(_PtrEnd - _Ptr));
    }

    AString& Assign(_In_reads_(_Count) const _Elem* const _Ptr, _CRT_GUARDOVERFLOW const size_type _Count) {
        if (!_Is_nullptr(_Ptr)) {
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

    AString& Assign(_CRT_GUARDOVERFLOW const size_type _Count, const _Elem _Ch) {
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

    AString& Assign(const AString& _Right, const size_type _Roff, size_type _Count = npos) {
        _Right._Check_offset(_Roff);
        _Count = _Right._Clamp_suffix_size(_Roff, _Count);
        return Assign(_Right._Myptr() + _Roff, _Count);
    }

    AString& Append(const AString& _Right) {
        return Append(_Right._Myptr(), _Right._Mysize());
    }
    
    AString& Append(const AString&& _Right) {
        return Append(_Right._Myptr(), _Right._Mysize());
    }

    AString& Append(const WString& _Right);

    AString& Append(const std::string& _Right) {
        return Append(_Right.data(), _Right.length());
    }

    AString& Append(const std::wstring& _Right) {
        return Append(_Right.data());
    }

    AString& Append(_STD initializer_list<_Elem> _Ilist) {
        return Append(_Ilist.begin(), _STD _Convert_size<size_type>(_Ilist.size()));
    }

    AString& Append(_In_z_ const _Elem* const _Ptr) {
        if (!_Is_nullptr(_Ptr)) {
            return *this;
        }

        return Append(_Ptr, _STD _Convert_size<size_type>(_Traits::length(_Ptr)));
    }

    AString& Append(const wchar_t* const _Right) {
        EncodeCodecvt<_Elem> Convert;

        if (!Convert.ToAnsi(_Right)) {
            return *this;
        }

        return Append(Convert._Myptr(), Convert._Mysize());
    }

    AString& Append(_In_reads_(_Count) const _Elem* const _Ptr, _CRT_GUARDOVERFLOW const size_type _Count) {
        if (!_Is_nullptr(_Ptr)) {
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

    AString& Append(_CRT_GUARDOVERFLOW const size_type _Count, const _Elem _Ch) {
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

    AString& Append(const AString& _Right, const size_type _Roff, size_type _Count = npos) {
        _Right._Check_offset(_Roff);
        _Count = _Right._Clamp_suffix_size(_Roff, _Count);
        return Append(_Right._Myptr() + _Roff, _Count);
    }

    AString& AppendFormat(_In_z_ _Printf_format_string_ const _Elem* Format, ...) noexcept {
        va_list args;

        va_start(args, Format);
        return AppendVFormat(Format, args);
    }

    AString& AppendVFormat(_In_z_ _Printf_format_string_ const _Elem* Format, va_list args) noexcept {
        AString Buffer;

        Buffer.VFormat(Format, args);
        return Append(Buffer);
    }

    AString& Insert(const size_type _Off, const AString& _Right) {
        return Insert(_Off, _Right._Myptr(), _Right._Mysize());
    }

    AString& Insert(const size_type _Off, const std::string& _Right) {
        return Insert(_Off, _Right.data(), _Right.length());
    }

    AString& Insert(const size_type _Off, _In_z_ const _Elem* const _Ptr) {
        return Insert(_Off, _Ptr, _STD _Convert_size<size_type>(_Traits::length(_Ptr)));
    }

    AString& Insert(const size_type _Off, _CRT_GUARDOVERFLOW const size_type _Count, const _Elem _Ch) {
        _Check_offset(_Off);
        const size_type _Old_size = _Mysize();
        if (_Count <= _Myres() - _Old_size) {
            this->intlength = Ulong64toUint32(_Old_size + _Count);
            _Elem* const _Old_ptr = _Myptr();
            _Elem* const _Insert_at = _Old_ptr + _Off;
            _Traits::move(_Insert_at + _Count, _Insert_at, _Old_size - _Off + 1); // move suffix + null down
            _Traits::assign(_Insert_at, _Count, _Ch); // fill hole
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

    AString& Insert(const size_type _Off, _In_reads_(_Count) const _Elem* const _Ptr, _CRT_GUARDOVERFLOW const size_type _Count) {
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

    AString& Insert(const size_type _Off, const AString& _Right, const size_type _Roff, size_type _Count = npos) {
        _Right._Check_offset(_Roff);
        _Count = _Right._Clamp_suffix_size(_Roff, _Count);
        return Insert(_Off, _Right._Myptr() + _Roff, _Count);
    }

    AString& Erase(const size_type _Off = 0) {
        _Check_offset(_Off);
        _Eos(_Off);
        return *this;
    }

    AString& Erase(const size_type _Off, const size_type _Count) {
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
            _Myptr()[i] = tolower(_Myptr()[i]);
        }
    }

    _CONSTEXPR20 void ToUpper() noexcept {
        for (size_t i = 0; i < length(); i++) {
            _Myptr()[i] = _Elem(toupper(_Myptr()[i]));
        }
    }

    AString& Replace(const AString& old_value, const AString& new_value) {
        while (true) {
            AString::size_type pos(0);
            if ((pos = find(old_value, 0)) != AString::npos) {
                Replace(pos, old_value.length(), new_value);
            }
            else {
                break;
            }
        }
        return *this;
    }

    AString& Replace(const _Elem* const old_value, const _Elem* const new_value) {
        while (true) {
            AString::size_type pos(0);
            if ((pos = find(old_value, 0)) != AString::npos) {
                Replace(pos, _Traits::length(old_value), new_value);
            }
            else {
                break;
            }
        }
        return *this;
    }

    AString& Replace(const size_type _Off, const size_type _Nx, _In_z_ const _Elem* const _Ptr) {
        return Replace(_Off, _Nx, _Ptr, _STD _Convert_size<size_type>(_Traits::length(_Ptr)));
    }

    AString& Replace(const size_type _Off, size_type _Nx, const size_type _Count, const _Elem _Ch) {
        // replace [_Off, _Off + _Nx) with _Count * _Ch
        _Check_offset(_Off);
        _Nx = _Clamp_suffix_size(_Off, _Nx);
        if (_Count == _Nx) {
            _Traits::assign(_Myptr() + _Off, _Count, _Ch);
            return *this;
        }

        const size_type _Old_size = _Mysize();
        if (_Count < _Nx || _Count - _Nx <= _Myres() - _Old_size) {
            // either we are shrinking, or the growth fits
            // may temporarily overflow; OK because size_type must be unsigned
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

    AString& Replace(const size_type _Off, size_type _Nx, _In_reads_(_Count) const _Elem* const _Ptr, const size_type _Count) {
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
#endif // _HAS_CXX20
        {
            if (_Growth <= _Myres() - _Old_size) {
                this->intlength = Ulong64toUint32(_Old_size + _Growth);
                _Elem* const _Old_ptr = _Myptr();
                _Elem* const _Insert_at = _Old_ptr + _Off;
                _Elem* const _Suffix_at = _Insert_at + _Nx;

                size_type _Ptr_shifted_after; // see rationale in insert
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

    AString& Replace(const size_type _Off, const size_type _Nx, const AString& _Right) {
        return Replace(_Off, _Nx, _Right._Myptr(), _Right._Mysize());
    }

    AString& Replace(const size_type _Off, const size_type _Nx, const std::string& _Right) {
        return Replace(_Off, _Nx, _Right.data(), _Right.length());
    }

    AString& Replace(const size_type _Off, size_type _Nx, const AString& _Right, const size_type _Roff, size_type _Count = npos) {
        _Right._Check_offset(_Roff);
        _Count = _Right._Clamp_suffix_size(_Roff, _Count);
        return Replace(_Off, _Nx, _Right._Myptr() + _Roff, _Count);
    }

    AString& Replace(const const_iterator _First, const const_iterator _Last, _In_z_ const _Elem* const _Ptr) {
        _Adl_verify_range(_First, _Last);
        return Replace(static_cast<size_type>(_STD _Unfancy(_First._Ptr) - _Myptr()), static_cast<size_type>(_Last._Ptr - _First._Ptr), _Ptr);
    }

    AString& Replace(const const_iterator _First, const const_iterator _Last, const AString& _Right) {
        _Adl_verify_range(_First, _Last);
        return Replace(static_cast<size_type>(_STD _Unfancy(_First._Ptr) - _Myptr()), static_cast<size_type>(_Last._Ptr - _First._Ptr), _Right);
    }

    AString& Replace(const const_iterator _First, const const_iterator _Last, const size_type _Count, const _Elem _Ch) {
        _Adl_verify_range(_First, _Last);
        return Replace(static_cast<size_type>(_STD _Unfancy(_First._Ptr) - _Myptr()), static_cast<size_type>(_Last._Ptr - _First._Ptr), _Count, _Ch);
    }

    AString& Replace(const const_iterator _First, const const_iterator _Last, _In_reads_(_Count) const _Elem* const _Ptr, const size_type _Count) {
        _Adl_verify_range(_First, _Last);
        return Replace(static_cast<size_type>(_STD _Unfancy(_First._Ptr) - _Myptr()), static_cast<size_type>(_Last._Ptr - _First._Ptr), _Ptr, _Count);
    }

    template <class _StringViewIsh, _Is_string_view_ish<_StringViewIsh> = 0>
    AString& Replace(const size_type _Off, const size_type _Nx, const _StringViewIsh& _Right) {
        _STD basic_string_view<_Elem, _Traits> _As_view = _Right;
        return Replace(_Off, _Nx, _As_view.data(), _Convert_size<size_type>(_As_view.size()));
    }

    template <class _StringViewIsh, _Is_string_view_ish<_StringViewIsh> = 0>
    AString& Replace(const size_type _Off, const size_type _Nx, const _StringViewIsh& _Right, const size_type _Roff, const size_type _Count = npos) {
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
            size_type _New_capacity = _Newcap;
            this->str = _Allocate_for_capacity(_New_capacity);
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

    void swap(AString& _Right) noexcept {
        if (this != _STD addressof(_Right)) {
            _Swap_data(_Right);
        }
    }

    void Split(const _Elem* _Delimiter, std::vector<AString>& _list) {
        _Elem* Safeptr = 0;
        _Elem* split = 0;

        if (IsEmpty()) {
            return;
        }

        split = strtok_s(_Myptr(),_Delimiter,&Safeptr);
        while (split) {
            _list.push_back(split);
            split = (_Elem*)strtok_s(0,_Delimiter, &Safeptr);
        }
    }

    _NODISCARD _CONSTEXPR20 void Split(const AString& _Delimiter, std::vector<AString>& _list) noexcept {
        Split(_Delimiter.c_str(), _list);
    }

    _NODISCARD _CONSTEXPR20 void Split(const std::string& _Delimiter, std::vector<AString>& _list) noexcept {
        Split(_Delimiter.c_str(), _list);
    }

    _NODISCARD _CONSTEXPR20 bool MidStr(_In_z_ const _Elem* _Left, _In_z_ const _Elem* _Right, AString& Ret) noexcept {

        auto pos = find(_Left, 0);
        if (pos == npos) {
            return false;
        }

        pos += _Traits::length(_Left);

        auto end = find(_Right, pos);
        if (end == npos || end == pos) {
            return false;
        }


        Ret.Assign(_Myptr() + pos, _Myptr() + end);
        return true;
    }

    _NODISCARD _CONSTEXPR20 bool MidStr(const AString& _Left, const AString& _Right, AString& Ret) noexcept {
        return MidStr(_Left.c_str(), _Right.c_str(), Ret);
    }

    _NODISCARD _CONSTEXPR20 bool MidStr(const std::string& _Left, const std::string& _Right, AString& Ret) noexcept {
        return MidStr(_Left.c_str(), _Right.c_str(), Ret);
    }

    _NODISCARD _CONSTEXPR20 bool MoveImpl(_Elem* const _Right, const size_type _Size, const size_type _Actual_allocation_size) {

        _Traits::assign(_Right[_Size], _Elem());
        this->str = _Right;
        this->intlength = Ulong64toUint32(_Size);
        this->capacity = Ulong64toUint32(_Actual_allocation_size - 1);
        return true;
    }

    _NODISCARD _CONSTEXPR20 void shrink_to_fit() { // reduce capacity
        size_type _Target_capacity = (_STD min)(_Mysize() | _Alloc_mask, max_size());
        if (_Target_capacity < _Myres()) { // worth shrinking, do it

            const pointer _New_ptr = _Allocate_for_capacity(_Target_capacity); // throws

            _Traits::copy(_STD _Unfancy(_New_ptr), _STD _Unfancy(_Myptr()), _Mysize() + 1);
            _Deallocate_for_capacity(_Myptr());

            this->str = _New_ptr;
            this->capacity = Ulong64toUint32(_Target_capacity);
        }
    }

    AString& Format(_In_z_ _Printf_format_string_ const _Elem* const Format, ...) noexcept {
        va_list _ArgList;

        __crt_va_start(_ArgList, Format);
        return VFormat(Format, _ArgList);
    }

    AString& VFormat(_In_z_ _Printf_format_string_ const _Elem* const Format, va_list args) noexcept {
        _Elem* Buffer = 0;
        size_type _Count = 0;
        size_type _Length = 0;

        _Count = _vscprintf(Format, args);

        if (!_Count || _Count == npos) {
            _Tidy_deallocate();
            return *this;
        }

        _Count++;

        Buffer = MemAlty.allocate<_Elem>(_Count);
        _Length = vsnprintf(Buffer, _Count, Format, args);
        Assign(Buffer, _Length);

        MemAlty.deallocate(Buffer);
        return *this;
    }

    _NODISCARD _CONSTEXPR20 std::uint64_t Hash64() {
        auto offset_basis = 0xcbf29ce484222325ull;
        for (auto c : *this) {
            offset_basis ^= c;
            offset_basis *= 1099511628211ull;
        }

        return offset_basis;
    }

    _NODISCARD _CONSTEXPR20 std::uint64_t Hash64A() {
        auto offset_basis = 0xcbf29ce484222325ull;
        for (auto c : *this) {
            offset_basis ^= tolower(c);
            offset_basis *= 1099511628211ull;
        }

        return offset_basis;
    }

    _NODISCARD _CONSTEXPR20 std::uint32_t Hash32() {

        auto offset_basis = 0x811C9DC5u;
        for (auto c : *this) {
            offset_basis ^= c;
            offset_basis *= 0x1000193;
        }

        return offset_basis;
    }

    _NODISCARD _CONSTEXPR20 std::uint32_t Hash32A() {
        auto offset_basis = 0x811C9DC5u;
        for (auto c : *this) {
            offset_basis ^= tolower(c);
            offset_basis *= 0x1000193;
        }

        return offset_basis;
    }

    _NODISCARD _CONSTEXPR20 void sort() const noexcept {
        std::sort(pointer(_Unchecked_begin()), pointer(_Unchecked_end()));
    }

    _NODISCARD _CONSTEXPR20 void sort_desc() const noexcept {
        std::sort(pointer(_Unchecked_begin()), pointer(_Unchecked_end()),
            [](const AString& _Left, const AString& _Right) {
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

    AString& operator = (const AString& _Right) {
        if (this == _STD addressof(_Right)) {
            return *this;
        }
        return Assign(_Right._Myptr(), _Right._Mysize());
    }

    AString& operator = (const AString&& _Right) noexcept {
        if (this == _STD addressof(_Right)) {
            return *this;
        }
        return Assign(_Right._Myptr(), _Right._Mysize());
    }

    AString& operator = (const std::string& _Right) {
        return Assign(_Right);
    }

    AString& operator = (const std::wstring& _Right) {
        return Assign(_Right);
    }

    AString& operator = (_In_z_ const _Elem* const _Ptr) {
        return Assign(_Ptr);
    }

    AString& operator = (_In_z_ const wchar_t* const _Ptr) {
        return Assign(_Ptr);
    }

    AString& operator = (const _Elem _Ch) {
        return Assign(1, _Ch);
    }

    AString& operator = (_STD initializer_list<_Elem> _Ilist) {
        return Assign(_Ilist.begin(), _STD _Convert_size<size_type>(_Ilist.size()));
    }

    AString& operator = (const size_type Val) {
        if (Val) {
            intlength = Ulong64toUint32(Val);
        }
        return *this;
    }

    AString& operator = (const int Val) {
        if (Val) {
            intlength = Val;
        }
        return *this;
    }

    AString& operator = (const DWORD Val) {
        if (Val) {
            intlength = Val;
        }
        return *this;
    }

    AString& operator + (const AString& _Right) {
        return Append(_Right);
    }

    AString& operator + (const AString&& _Right) {
        return Append(_Right);
    }

    AString& operator + (const std::string& _Right) {
        return Append(_Right);
    }

    AString& operator + (const std::wstring& _Right) {
        return Append(_Right);
    }

    AString& operator + (_In_z_ const _Elem* const _Ptr) {
        return Append(_Ptr);
    }

    AString& operator + (_In_z_ const wchar_t* const _Ptr) {
        return Append(_Ptr);
    }

    AString& operator + (_Elem _Ch) {
        push_back(_Ch);
        return *this;
    }

    AString& operator + (_STD initializer_list<_Elem> _Ilist) {
        return Append(_Ilist.begin(), _STD _Convert_size<size_type>(_Ilist.size()));
    }

    AString& operator += (const AString& _Right) {
        return operator +(_Right);
    }

    AString& operator += (const AString&& _Right) {
        return operator +(_Right);
    }

    AString& operator += (const std::string& _Right) {
        return operator +(_Right);
    }
    
    AString& operator += (const std::wstring& _Right) {
        return operator +(_Right);
    }

    AString& operator += (_In_z_ const _Elem* const _Ptr) {
        return operator +(_Ptr);
    }

    AString& operator += (_In_z_ const wchar_t* const _Ptr) {
        return operator +(_Ptr);
    }

    AString& operator += (_Elem _Ch) {
        return operator +(_Ch);
    }

    AString& operator += (_STD initializer_list<_Elem> _Ilist) {
        return operator +(_Ilist);
    }

    _NODISCARD _CONSTEXPR20 bool operator <  (const _Elem* _Right) const noexcept {
        return (Compare(_Right) < 0);
    }

    _NODISCARD _CONSTEXPR20 bool operator <  (const AString& _Right) const noexcept {
        return operator < (_Right._Myptr());
    }

    _NODISCARD _CONSTEXPR20 bool operator <  (const std::string& _Right) const noexcept {
        return operator < (_Right.c_str());
    }

    _NODISCARD _CONSTEXPR20 bool operator > (const _Elem* _Right)  const noexcept {
        return (Compare(_Right) > 0);
    }

    _NODISCARD _CONSTEXPR20 bool operator > (const AString& _Right)  const noexcept {
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

    _NODISCARD _CONSTEXPR20 bool operator != (const AString& _Right) const noexcept {
        return !operator ==(_Right);
    }

    _NODISCARD _CONSTEXPR20 bool operator != (const std::string& _Right) const noexcept {
        return !operator ==(_Right);
    }

    _NODISCARD _CONSTEXPR20 bool operator == (const AString& _Right) const noexcept {
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

    _NODISCARD _CONSTEXPR20 int Compare(const AString& _Right) const noexcept {
        return _STD _Traits_compare<_Traits>(_Myptr(), _Mysize(), _Right._Myptr(), _Right._Mysize());
    }

    _NODISCARD _CONSTEXPR20 int Compare(const std::string& _Right) const noexcept {
        return _STD _Traits_compare<_Traits>(_Myptr(), _Mysize(), _Right.data(), _Right.length());
    }

    _NODISCARD _CONSTEXPR20 int Compare(size_type _Off, size_type _Nx, const AString& _Right) const {
        _Check_offset(_Off);
        return _STD _Traits_compare<_Traits>(_Myptr() + _Off, _Clamp_suffix_size(_Off, _Nx), _Right._Myptr(), _Right._Mysize());
    }

    _NODISCARD _CONSTEXPR20 int Compare(const size_type _Off, const size_type _Nx, const AString& _Right, const size_type _Roff, const size_type _Count = npos) const {
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
#endif // _HAS_CXX17

    _NODISCARD _CONSTEXPR20 int CompareI(_In_z_ const _Elem* const _Ptr) const noexcept {
        return _stricmp(_Myptr(), _Ptr);
    }

    _NODISCARD _CONSTEXPR20 int CompareI(const AString& _Right) const noexcept {
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

    _NODISCARD _CONSTEXPR20 bool BeginsWith(const AString& _Right) const noexcept {
        return BeginsWith(_Right._Myptr(), _Right._Mysize());
    }

    _NODISCARD _CONSTEXPR20 bool BeginsWith(const std::string& _Right) const noexcept {
        return BeginsWith(_Right.data(), _Right.length());
    }

    _NODISCARD _CONSTEXPR20 bool BeginsWith(const _Elem* const _Right, const size_type _Count) const noexcept {
        if (_Count > _Mysize()) {
            return false;
        }

        return !Compare(_Right, _Count);
    }

    _NODISCARD _CONSTEXPR20 bool BeginsWithI(const _Elem* const _Right) const noexcept {
        return BeginsWithI(_Right, _Traits::length(_Right));
    }

    _NODISCARD _CONSTEXPR20 bool BeginsWithI(const AString& _Right) const noexcept {
        return BeginsWithI(_Right._Myptr(), _Right._Mysize());
    }

    _NODISCARD _CONSTEXPR20 bool BeginsWithI(const std::string& _Right) const noexcept {
        return BeginsWithI(_Right.data(), _Right.length());
    }

    _NODISCARD _CONSTEXPR20 bool BeginsWithI(const _Elem* const _Right, const size_type _Count) const noexcept {
        if (_Count > _Mysize()) {
            return false;
        }

        return StrCmpNIA(_Myptr(), _Right, Ulong64toUint32(_Count)) == 0;
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

    _NODISCARD _CONSTEXPR20 bool EndsWith(const AString& _Right) const noexcept {
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

    _NODISCARD _CONSTEXPR20 bool EndsWithI(const AString& _Right) const noexcept {
        return EndsWithI(_Right._Myptr(), _Right._Mysize());
    }

    _NODISCARD _CONSTEXPR20 bool EndsWithI(const std::string& _Right) const noexcept {
        return EndsWithI(_Right.c_str(), _Right.length());
    }

    _NODISCARD _CONSTEXPR20 bool EndsWithI(const _Elem* const _Right, const size_type _Count) const noexcept {
        if (_Mysize() < _Count) {
            return false;
        }

        return _stricmp((const char*)(_Myptr() + (_Mysize() - _Count)), (const char*)(_Right)) == 0;
    }

    _NODISCARD _CONSTEXPR20 bool Equals(_In_z_ const _Elem* const _Ptr) const noexcept {
        return !strcmp((const char*)(_Myptr()), (const char*)(_Ptr));
    }

    _NODISCARD _CONSTEXPR20 bool Equals(const AString& _Right) const noexcept {
        return _Mysize() == _Right._Mysize() && Equals(_Right._Myptr());
    }

    _NODISCARD _CONSTEXPR20 bool Equals(const std::string& _Right) const noexcept {
        return _Mysize() == _Right.length() && Equals(_Right.data());
    }

    _NODISCARD _CONSTEXPR20 bool EqualsI(_In_z_ const _Elem* const _Ptr) const noexcept {
        return !CompareI(_Ptr);
    }

    _NODISCARD _CONSTEXPR20 bool EqualsI(const AString& _Right) const noexcept {
        return EqualsI(_Right._Myptr());
    }

    _NODISCARD _CONSTEXPR20 bool EqualsI(const std::string& _Right) const noexcept {
        return EqualsI(_Right.c_str());
    }

    _CONSTEXPR20 bool _Equal(_In_z_ const _Elem* const _Ptr) const noexcept {
        return _Equal(_Ptr, _Traits::length(_Ptr));
    }

    _CONSTEXPR20 bool _Equal(const AString& _Right) const noexcept {
        return _Equal(_Right._Myptr(), _Right._Mysize());
    }

    _CONSTEXPR20 bool _Equal(const std::string& _Right) const noexcept {
        return _Equal(_Right.data(), _Right.length());
    }

    _CONSTEXPR20 bool _Equal(_In_z_ const _Elem* const _Ptr, const size_type _Count) const noexcept {
        return _STD _Traits_equal<_Traits>(_Myptr(), _Mysize(), _Ptr, _Count);
    }

    _NODISCARD bool Matches(_In_z_ const _Elem* _Right) noexcept {
        _Elem* _Ptr = _Myptr();
        bool star;

    new_segment:
        star = false;
        if (*_Right == '*') {
            star = true;
            do { _Right++; } while (*_Right == '*');
        }

    test_match:
        int i;
        for (i = 0; _Right[i] && (_Right[i] != '*'); i++) {
            if (_Ptr[i] != _Right[i]) {
                if (!_Ptr[i]) {
                    return false;
                }
                if ((_Right[i] == '?') && (_Ptr[i] != '.')) {
                    continue;
                }
                if (!star) {
                    return false;
                }
                _Ptr++;
                goto test_match;
            }
        }
        if (_Right[i] == '*') {
            _Ptr += i;
            _Right += i;
            goto new_segment;
        }
        if (!_Ptr[i]) {
            return true;
        }
        if (i && _Right[i - 1] == '*') {
            return true;
        }
        if (!star) {
            return false;
        }
        _Ptr++;
        goto test_match;
        return false;
    }

    _NODISCARD _CONSTEXPR20 bool Matches(const AString& _Right)noexcept {
        return Matches(_Right.c_str());
    }

    _NODISCARD _CONSTEXPR20 bool Matches(const std::string& _Right)noexcept {
        return Matches(_Right.c_str());
    }

    _NODISCARD bool MatchesI(_In_z_ const _Elem* _Right) noexcept {
        bool star;
        const _Elem* _Ptr = _Myptr();

    new_segment:
        star = false;
        if (*_Right == '*') {
            star = true;
            do { _Right++; } while (*_Right == '*');
        }

    test_match:
        int i;
        for (i = 0; _Right[i] && (_Right[i] != '*'); i++) {
            char a = _Ptr[i]; a = ((a >= 'A') && (a <= 'Z')) ? 'a' + (a - 'A') : a;
            char b = _Right[i]; b = ((b >= 'A') && (b <= 'Z')) ? 'a' + (b - 'A') : b;
            if (a != b) {
                if (!_Ptr[i]) {
                    return false;
                }
                if ((_Right[i] == '?') && (_Ptr[i] != '.')) continue;
                if (!star) {
                    return false;
                }
                _Ptr++;
                goto test_match;
            }
        }
        if (_Right[i] == '*')
        {
            _Ptr += i;
            _Right += i;
            goto new_segment;
        }
        if (!_Ptr[i]) {
            return true;
        }
        if (i && _Right[i - 1] == '*') {
            return true;
        }
        if (!star) {
            return false;
        }
        _Ptr++;
        goto test_match;
        return false;
    }

    _NODISCARD _CONSTEXPR20 bool MatchesI(const AString& _Right) noexcept {
        return MatchesI(_Right.c_str());
    }

    _NODISCARD _CONSTEXPR20 bool MatchesI(const std::string& _Right) noexcept {
        return MatchesI(_Right.c_str());
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
#endif // _HAS_CXX17

    _NODISCARD _CONSTEXPR20 size_type find(const AString& _Right, const size_type _Off = 0) const noexcept {
        return static_cast<size_type>(_STD _Traits_find<_Traits>(_Myptr(), _Mysize(), _Off, _Right._Myptr(), _Right._Mysize()));
    }

    _NODISCARD _CONSTEXPR20 size_type find(_In_reads_(_Count) const _Elem* const _Ptr, const size_type _Off,const size_type _Count) const noexcept {
        return static_cast<size_type>(_STD _Traits_find<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Count));
    }

    _NODISCARD _CONSTEXPR20 size_type find(_In_z_ const _Elem* const _Ptr, const size_type _Off) const noexcept {
        return static_cast<size_type>(_STD _Traits_find<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Traits::length(_Ptr)));
    }

    _NODISCARD bool find(_In_z_ const _Elem* const _Ptr) noexcept {
        AString src;
        AString startPos;

        src = _Myptr();
        startPos = _Ptr;

        if (!_Mysize()) {
            return false;
        }

        src.ToLower();
        startPos.ToLower();

        return std::strstr((const char*)(src._Myptr()), (const char*)(startPos._Myptr())) != 0;
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
#endif // _HAS_CXX17
    _NODISCARD _CONSTEXPR20 size_type rfind(const AString& _Right, const size_type _Off = npos) const noexcept {
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
#endif // _HAS_CXX17

    _NODISCARD constexpr size_type find_first_of(const AString& _Right, const size_type _Off = 0) const noexcept {
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
        // look for one of _Right before _Off
        _STD basic_string_view<_Elem, _Traits> _As_view = _Right;
        return static_cast<size_type>(_STD _Traits_find_last_of<_Traits>(_Myptr(), _Mysize(), _Off, _As_view.data(), _As_view.size()));
    }
#endif // _HAS_CXX17

    _NODISCARD constexpr size_type find_last_of(const AString& _Right, size_type _Off = npos) const noexcept {
        // look for one of _Right before _Off
        return static_cast<size_type>(_STD _Traits_find_last_of<_Traits>(_Myptr(), _Mysize(), _Off, _Right._Myptr(), _Right._Mysize()));
    }

    _NODISCARD constexpr size_type find_last_of(_In_reads_(_Count) const _Elem* const _Ptr, const size_type _Off,
        const size_type _Count) const noexcept /* strengthened */ {
        // look for one of [_Ptr, _Ptr + _Count) before _Off
        return static_cast<size_type>(_STD _Traits_find_last_of<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Count));
    }

    _NODISCARD constexpr size_type find_last_of(
        _In_z_ const _Elem* const _Ptr, const size_type _Off = npos) const noexcept /* strengthened */ {
        // look for one of [_Ptr, <null>) before _Off
        return static_cast<size_type>(_STD _Traits_find_last_of<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Traits::length(_Ptr)));
    }

    _NODISCARD constexpr size_type find_last_of(const _Elem _Ch, const size_type _Off = npos) const noexcept {
        // look for _Ch before _Off
        return static_cast<size_type>(_STD _Traits_rfind_ch<_Traits>(_Myptr(), _Mysize(), _Off, _Ch));
    }

#if _HAS_CXX17
    template <class _StringViewIsh, _Is_string_view_ish<_StringViewIsh> = 0>
    _NODISCARD constexpr size_type find_first_not_of(const _StringViewIsh& _Right, const size_type _Off = 0) const
        noexcept(_STD _Is_nothrow_convertible_v<const _StringViewIsh&, _STD basic_string_view<_Elem, _Traits>>) {
        // look for none of _Right at or after _Off
        _STD basic_string_view<_Elem, _Traits> _As_view = _Right;
        return static_cast<size_type>(_STD _Traits_find_first_not_of<_Traits>(_Myptr(), _Mysize(), _Off, _As_view.data(), _As_view.size()));
    }
#endif // _HAS_CXX17

    _NODISCARD constexpr size_type find_first_not_of(const AString& _Right, const size_type _Off = 0) const noexcept {
        // look for none of _Right at or after _Off
        return static_cast<size_type>(_STD _Traits_find_first_not_of<_Traits>(_Myptr(), _Mysize(), _Off, _Right._Myptr(), _Right._Mysize()));
    }

    _NODISCARD constexpr size_type find_first_not_of(_In_reads_(_Count) const _Elem* const _Ptr,
        const size_type _Off, const size_type _Count) const noexcept /* strengthened */ {
        // look for none of [_Ptr, _Ptr + _Count) at or after _Off
        return static_cast<size_type>(_STD _Traits_find_first_not_of<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Count));
    }

    _NODISCARD constexpr size_type find_first_not_of(
        _In_z_ const _Elem* const _Ptr, size_type _Off = 0) const noexcept
        /* strengthened */ {
        // look for one of [_Ptr, <null>) at or after _Off
        return static_cast<size_type>(_STD _Traits_find_first_not_of<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Traits::length(_Ptr)));
    }

    _NODISCARD constexpr size_type find_first_not_of(const _Elem _Ch, const size_type _Off = 0) const noexcept {
        // look for non-_Ch at or after _Off
        return static_cast<size_type>(_STD _Traits_find_not_ch<_Traits>(_Myptr(), _Mysize(), _Off, _Ch));
    }

#if _HAS_CXX17
    template <class _StringViewIsh, _Is_string_view_ish<_StringViewIsh> = 0>
    _NODISCARD constexpr size_type find_last_not_of(const _StringViewIsh& _Right, const size_type _Off = npos) const
        noexcept(_STD _Is_nothrow_convertible_v<const _StringViewIsh&, _STD basic_string_view<_Elem, _Traits>>) {
        // look for none of _Right before _Off
        _STD basic_string_view<_Elem, _Traits> _As_view = _Right;
        return static_cast<size_type>(_STD _Traits_find_last_not_of<_Traits>(_Myptr(), _Mysize(), _Off, _As_view.data(), _As_view.size()));
    }
#endif // _HAS_CXX17

    _NODISCARD constexpr size_type find_last_not_of(
        const AString& _Right, const size_type _Off = npos) const noexcept {
        // look for none of _Right before _Off
        return static_cast<size_type>(_STD _Traits_find_last_not_of<_Traits>(_Myptr(), _Mysize(), _Off, _Right._Myptr(), _Right._Mysize()));
    }

    _NODISCARD constexpr size_type find_last_not_of(_In_reads_(_Count) const _Elem* const _Ptr, const size_type _Off,
        const size_type _Count) const noexcept /* strengthened */ {
        // look for none of [_Ptr, _Ptr + _Count) before _Off
        return static_cast<size_type>(_STD _Traits_find_last_not_of<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Count));
    }

    _NODISCARD constexpr size_type find_last_not_of(
        _In_z_ const _Elem* const _Ptr, const size_type _Off = npos) const noexcept {
        // look for none of [_Ptr, <null>) before _Off
        return static_cast<size_type>(_STD _Traits_find_last_not_of<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Traits::length(_Ptr)));
    }

    _NODISCARD constexpr size_type find_last_not_of(const _Elem _Ch, const size_type _Off = npos) const noexcept {
        // look for non-_Ch before _Off
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

protected:
    pointer str;
    std::uint32_t intlength;
    std::uint32_t capacity;
private:

    _CONSTEXPR20 void _Construct_empty() {
        this->str = const_cast<char*>(a_EmptyString);
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
        return ((_Myres() & MEM_MUST_BE_FREED_FLAG) == MEM_MUST_BE_FREED_FLAG) && _Myptr() != 0 && _Myptr() != const_cast<char*>(a_EmptyString);
    }
    
    _NODISCARD void _Tidy_deallocate() noexcept {

        if (_Is_Tidy_deallocate()) {
            _Deallocate_for_capacity(_Myptr());
        }

        _Construct_empty();
    }

    template <class _Fty, class... _ArgTys>
    _CONSTEXPR20 AString& _Reallocate_for(const size_type _New_size, _Fty _Fn, _ArgTys... _Args) {
        
        if (_New_size > max_size()) {
            _STD _Xlen_string();
        }

        const size_type _Old_capacity = _Myres();
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
    _CONSTEXPR20 AString& _Reallocate_grow_by(const size_type _Size_increase, _Fty _Fn, _ArgTys... _Args) {

        const size_type _Old_size = _Mysize();
        if (max_size() - _Old_size < _Size_increase) {
            _STD _Xlen_string(); // result too long
        }

        const size_type _New_size = _Old_size + _Size_increase;
        const size_type _Old_capacity = _Myres();
        size_type _New_capacity = _Calculate_growth(_New_size);
        const pointer _New_ptr = _Allocate_for_capacity(_New_capacity); // throws

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

    AString& _Erase_noexcept(const size_type _Off, size_type _Count) noexcept {
        _Count = _Clamp_suffix_size(_Off, _Count);
        const size_type _Old_size = _Mysize();
        _Elem* const _My_ptr = _Myptr();
        _Elem* const _Erase_at = _My_ptr + _Off;
        const size_type _New_size = _Old_size - _Count;
        _Traits::move(_Erase_at, _Erase_at + _Count, _New_size - _Off + 1); // move suffix + null up
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
        if (_Masked > _Max) { // the mask overflows, settle for max_size()
            return _Max;
        }

        if (_Old > _Max - _Old / 2) { // similarly, geometric overflows
            return _Max;
        }

        return (_STD max)(_Masked, _Old + _Old / 2);
    }

    _NODISCARD _CONSTEXPR20 size_type _Calculate_growth(const size_type _Requested) const noexcept {
        return _Calculate_growth(_Requested, _Myres(), max_size());
    }

    _CONSTEXPR20 void _Swap_data(AString& _Right) noexcept {
        using _STD swap;

        _STD swap(this->str, _Right.str);
        _STD swap(this->intlength, _Right.intlength);
        _STD swap(this->capacity, _Right.capacity);
    }

    _CONSTEXPR20 size_type copy(_Out_writes_(_Count) _Elem* const _Ptr, size_type _Count, const size_type _Off = 0) const {
        _Check_offset(_Off);
        _Count = _Clamp_suffix_size(_Off, _Count);
        _Traits::copy(_Ptr, _Myptr() + _Off, _Count);
        return _Count;
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

class WString {
public:
    class Vector {
    public:
        WString* str = 0;
        std::uint32_t Size = 0;
        std::uint32_t Capacity = MEM_VECTOR_FLAG;
    };
public:
    using _Elem = wchar_t;
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
    WString() noexcept {
        _Construct_empty();
    }

    WString(const WString& _Right) {
        _Construct_empty();
        Assign(_Right);
    }

    WString(const AString& _Right) {
        _Construct_empty();
        Assign(_Right);
    }

    WString(const WString&& _Right) noexcept {
        _Construct_empty();
        Assign(_Right);
    }

    WString(const std::wstring& _Right) {
        _Construct_empty();
        Assign(_Right);
    }

    WString(const std::string& _Right) {
        _Construct_empty();
        Assign(_Right);
    }

    WString(const size_type _Count) {
        _Construct_empty();
        Assign(_Count);
    }

    WString(_STD initializer_list<_Elem> _Ilist) {
        _Construct_empty();
        Assign(_Ilist.begin(), _STD _Convert_size<size_type>(_Ilist.size()));
    }

    WString(_In_z_ const _Elem* const _Ptr) {
        _Construct_empty();
        Assign(_Ptr, _STD _Convert_size<size_type>(_Traits::length(_Ptr)));
    }

    WString(_In_z_ const char* const _Ptr) {
        _Construct_empty();
        Assign(_Ptr);
    }

    WString(const WString& _Right, const size_type _Roff) {
        _Construct_empty();
        _Right._Check_offset(_Roff);
        Assign(_Right._Myptr() + _Roff, _Right._Clamp_suffix_size(_Roff, npos));
    }

    WString(const WString&& _Right, const size_type _Roff) {
        _Construct_empty();
        _Right._Check_offset(_Roff);
        Assign(_Right._Myptr() + _Roff, _Right._Clamp_suffix_size(_Roff, npos));
    }

    WString(const WString& _Right, const size_type _Roff, const size_type _Count) {
        _Construct_empty();
        _Right._Check_offset(_Roff);
        Assign(_Right._Myptr() + _Roff, _Right._Clamp_suffix_size(_Roff, _Count));
    }

    WString(const _Elem* const _Ptr, const _Elem* const _PtrEnd) {
        _Construct_empty();
        Assign(_Ptr, _PtrEnd);
    }

    WString(_In_reads_(_Count) const _Elem* const _Ptr, _CRT_GUARDOVERFLOW const size_type _Count) {
        _Construct_empty();
        Assign(_Ptr, _Count);
    }

    WString(_CRT_GUARDOVERFLOW const size_type _Count, const _Elem _Ch) {
        _Construct_empty();
        Assign(_Count, _Ch);
    }

    ~WString() {
        _Tidy_deallocate();
    }

    WString& Assign(const WString& _Right) {
        return Assign(_Right._Myptr());
    }

    WString& Assign(const WString&& _Right) noexcept {
        return Assign(_Right);
    }

    WString& Assign(const AString& _Right) {
        return Assign(_Right.data());
    }

    WString& Assign(const std::string& _Right) {
        return Assign(_Right.c_str());
    }

    WString& Assign(const std::wstring& _Right) {
        return Assign(_Right.c_str());
    }

    WString& Assign(const size_type _Count) noexcept {
        _Tidy_deallocate();

        size_type _New_capacity = _Count;
        this->str = _Allocate_for_capacity(_New_capacity);
        this->capacity = Ulong64toUint32(_New_capacity);

        return *this;
    }

    WString& Assign(_STD initializer_list<_Elem> _Ilist) {
        return Assign(_Ilist.begin(), _STD _Convert_size<size_type>(_Ilist.size()));
    }

    WString& Assign(_In_z_ const _Elem* const _Ptr) {
        if (!_Is_nullptr(_Ptr)) {
            return *this;
        }

        return Assign(_Ptr, _STD _Convert_size<size_type>(_Traits::length(_Ptr)));
    }

    WString& Assign(const char* const _Right) {
        EncodeCodecvt<wchar_t> Convert;

        if (!Convert.ToUtf8(_Right)) {
            return *this;
        }

        return Assign(Convert._Myptr(), Convert._Mysize());
    }

    WString& Assign(const _Elem* const _Ptr, const _Elem* const _PtrEnd) {
        if (!_Is_nullptr(_Ptr)) {
            return *this;
        }

        return Assign(_Ptr, _STD _Convert_size<size_type>(_PtrEnd - _Ptr));
    }

    WString& Assign(_In_reads_(_Count) const _Elem* const _Ptr, _CRT_GUARDOVERFLOW const size_type _Count) {
        if (!_Is_nullptr(_Ptr)) {
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

    WString& Assign(_CRT_GUARDOVERFLOW const size_type _Count, const _Elem _Ch) {
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

    WString& Assign(const WString& _Right, const size_type _Roff, size_type _Count = npos) {
        _Right._Check_offset(_Roff);
        _Count = _Right._Clamp_suffix_size(_Roff, _Count);
        return Assign(_Right._Myptr() + _Roff, _Count);
    }

    WString& Append(const WString& _Right) {
        return Append(_Right._Myptr(), _Right._Mysize());
    }

    WString& Append(const AString& _Right) {
        return Append(_Right.c_str());
    }

    WString& Append(const WString&& _Right) {
        return Append(_Right._Myptr(), _Right._Mysize());
    }

    WString& Append(const std::wstring& _Right) {
        return Append(_Right.data(), _Right.length());
    }

    WString& Append(const std::string& _Right) {
        return Append(_Right.data());
    }

    WString& Append(_STD initializer_list<_Elem> _Ilist) {
        return Append(_Ilist.begin(), _STD _Convert_size<size_type>(_Ilist.size()));
    }

    WString& Append(_In_z_ const char* const _Ptr) {
        EncodeCodecvt<wchar_t> Convert;
        if (!_Is_nullptr(_Ptr)) {
            return *this;
        }
        
        if (!Convert.ToUtf8(_Ptr)) {
            return *this;
        }

        return Append(Convert._Myptr(), Convert._Mysize());
    }

    WString& Append(_In_z_ const _Elem* const _Ptr) {
        if (!_Is_nullptr(_Ptr)) {
            return *this;
        }

        return Append(_Ptr, _STD _Convert_size<size_type>(_Traits::length(_Ptr)));
    }

    WString& Append(_In_reads_(_Count) const _Elem* const _Ptr, _CRT_GUARDOVERFLOW const size_type _Count) {
        if (!_Is_nullptr(_Ptr)) {
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

    WString& Append(_CRT_GUARDOVERFLOW const size_type _Count, const _Elem _Ch) {
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

    WString& Append(const WString& _Right, const size_type _Roff, size_type _Count = npos) {
        _Right._Check_offset(_Roff);
        _Count = _Right._Clamp_suffix_size(_Roff, _Count);
        return Append(_Right._Myptr() + _Roff, _Count);
    }

    WString& AppendFormat(_In_z_ _Printf_format_string_ const _Elem* Format, ...) noexcept {
        va_list args;

        va_start(args, Format);
        return AppendVFormat(Format, args);
    }

    WString& AppendVFormat(_In_z_ _Printf_format_string_ const _Elem* Format, va_list args) noexcept {
        WString Buffer;

        Buffer.VFormat(Format, args);
        return Append(Buffer);
    }

    WString& Insert(const size_type _Off, const WString& _Right) {
        return Insert(_Off, _Right._Myptr(), _Right._Mysize());
    }

    WString& Insert(const size_type _Off, const std::wstring& _Right) {
        return Insert(_Off, _Right.data(), _Right.length());
    }

    WString& Insert(const size_type _Off, _In_z_ const _Elem* const _Ptr) {
        return Insert(_Off, _Ptr, _STD _Convert_size<size_type>(_Traits::length(_Ptr)));
    }

    WString& Insert(const size_type _Off, _CRT_GUARDOVERFLOW const size_type _Count, const _Elem _Ch) {
        _Check_offset(_Off);
        const size_type _Old_size = _Mysize();
        if (_Count <= _Myres() - _Old_size) {
            this->intlength = Ulong64toUint32(_Old_size + _Count);
            _Elem* const _Old_ptr = _Myptr();
            _Elem* const _Insert_at = _Old_ptr + _Off;
            _Traits::move(_Insert_at + _Count, _Insert_at, _Old_size - _Off + 1); // move suffix + null down
            _Traits::assign(_Insert_at, _Count, _Ch); // fill hole
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

    WString& Insert(const size_type _Off, _In_reads_(_Count) const _Elem* const _Ptr, _CRT_GUARDOVERFLOW const size_type _Count) {
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

    WString& Insert(const size_type _Off, const WString& _Right, const size_type _Roff, size_type _Count = npos) {
        _Right._Check_offset(_Roff);
        _Count = _Right._Clamp_suffix_size(_Roff, _Count);
        return Insert(_Off, _Right._Myptr() + _Roff, _Count);
    }

    WString& Erase(const size_type _Off = 0) {
        _Check_offset(_Off);
        _Eos(_Off);
        return *this;
    }

    WString& Erase(const size_type _Off, const size_type _Count) {
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
            _Myptr()[i] = tolower(_Myptr()[i]);
        }
    }

    _CONSTEXPR20 void ToUpper() noexcept {
        for (size_t i = 0; i < length(); i++) {
            _Myptr()[i] = _Elem(toupper(_Myptr()[i]));
        }
    }

    WString& Replace(const WString& old_value, const WString& new_value) {
        while (true) {
            WString::size_type pos(0);
            if ((pos = find(old_value, 0)) != WString::npos) {
                Replace(pos, old_value.length(), new_value);
            }
            else {
                break;
            }
        }
        return *this;
    }

    WString& Replace(const _Elem* const old_value, const _Elem* const new_value) {
        while (true) {
            WString::size_type pos(0);
            if ((pos = find(old_value, 0)) != WString::npos) {
                Replace(pos, _Traits::length(old_value), new_value);
            }
            else {
                break;
            }
        }
        return *this;
    }

    WString& Replace(const size_type _Off, const size_type _Nx, _In_z_ const _Elem* const _Ptr) {
        return Replace(_Off, _Nx, _Ptr, _STD _Convert_size<size_type>(_Traits::length(_Ptr)));
    }

    WString& Replace(const size_type _Off, size_type _Nx, const size_type _Count, const _Elem _Ch) {
        // replace [_Off, _Off + _Nx) with _Count * _Ch
        _Check_offset(_Off);
        _Nx = _Clamp_suffix_size(_Off, _Nx);
        if (_Count == _Nx) {
            _Traits::assign(_Myptr() + _Off, _Count, _Ch);
            return *this;
        }

        const size_type _Old_size = _Mysize();
        if (_Count < _Nx || _Count - _Nx <= _Myres() - _Old_size) {
            // either we are shrinking, or the growth fits
            // may temporarily overflow; OK because size_type must be unsigned
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

    WString& Replace(const size_type _Off, size_type _Nx, _In_reads_(_Count) const _Elem* const _Ptr, const size_type _Count) {
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
#endif // _HAS_CXX20
        {
            if (_Growth <= _Myres() - _Old_size) {
                this->intlength = Ulong64toUint32(_Old_size + _Growth);
                _Elem* const _Old_ptr = _Myptr();
                _Elem* const _Insert_at = _Old_ptr + _Off;
                _Elem* const _Suffix_at = _Insert_at + _Nx;

                size_type _Ptr_shifted_after; // see rationale in insert
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

    WString& Replace(const size_type _Off, const size_type _Nx, const WString& _Right) {
        return Replace(_Off, _Nx, _Right._Myptr(), _Right._Mysize());
    }

    WString& Replace(const size_type _Off, const size_type _Nx, const std::wstring& _Right) {
        return Replace(_Off, _Nx, _Right.data(), _Right.length());
    }

    WString& Replace(const size_type _Off, size_type _Nx, const WString& _Right, const size_type _Roff, size_type _Count = npos) {
        _Right._Check_offset(_Roff);
        _Count = _Right._Clamp_suffix_size(_Roff, _Count);
        return Replace(_Off, _Nx, _Right._Myptr() + _Roff, _Count);
    }

    WString& Replace(const const_iterator _First, const const_iterator _Last, _In_z_ const _Elem* const _Ptr) {
        _Adl_verify_range(_First, _Last);
        return Replace(static_cast<size_type>(_STD _Unfancy(_First._Ptr) - _Myptr()), static_cast<size_type>(_Last._Ptr - _First._Ptr), _Ptr);
    }

    WString& Replace(const const_iterator _First, const const_iterator _Last, const WString& _Right) {
        _Adl_verify_range(_First, _Last);
        return Replace(static_cast<size_type>(_STD _Unfancy(_First._Ptr) - _Myptr()), static_cast<size_type>(_Last._Ptr - _First._Ptr), _Right);
    }

    WString& Replace(const const_iterator _First, const const_iterator _Last, const size_type _Count, const _Elem _Ch) {
        _Adl_verify_range(_First, _Last);
        return Replace(static_cast<size_type>(_STD _Unfancy(_First._Ptr) - _Myptr()), static_cast<size_type>(_Last._Ptr - _First._Ptr), _Count, _Ch);
    }

    WString& Replace(const const_iterator _First, const const_iterator _Last, _In_reads_(_Count) const _Elem* const _Ptr, const size_type _Count) {
        _Adl_verify_range(_First, _Last);
        return Replace(static_cast<size_type>(_STD _Unfancy(_First._Ptr) - _Myptr()), static_cast<size_type>(_Last._Ptr - _First._Ptr), _Ptr, _Count);
    }

    template <class _StringViewIsh, _Is_string_view_ish<_StringViewIsh> = 0>
    WString& Replace(const size_type _Off, const size_type _Nx, const _StringViewIsh& _Right) {
        _STD basic_string_view<_Elem, _Traits> _As_view = _Right;
        return Replace(_Off, _Nx, _As_view.data(), _Convert_size<size_type>(_As_view.size()));
    }

    template <class _StringViewIsh, _Is_string_view_ish<_StringViewIsh> = 0>
    WString& Replace(const size_type _Off, const size_type _Nx, const _StringViewIsh& _Right, const size_type _Roff, const size_type _Count = npos) {
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

    void pop_back() noexcept {
        const size_type _Old_size = _Mysize();
        _Eos(_Old_size - 1);
    }

    void resize(_CRT_GUARDOVERFLOW const size_type _New_size, const _Elem _Ch = _Elem()) {
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
            size_type _New_capacity = _Newcap;
            this->str = _Allocate_for_capacity(_New_capacity);
            this->capacity = Ulong64toUint32(_New_capacity);
            return;
        }

        if (_Myres() >= _Newcap) {
            return;
        }

        const size_type _Old_size = _Mysize();
        _Reallocate_grow_by(_Newcap - _Old_size,
            [](_Elem* const _New_ptr, const _Elem* const _Old_ptr, const size_type _Old_size) {
                _Traits::copy(_New_ptr, _Old_ptr, _Old_size + 1);
            });

        this->intlength = Ulong64toUint32(_Old_size);
    }

    _CONSTEXPR20 void swap(WString& _Right) noexcept {
        if (this != _STD addressof(_Right)) {
            _Swap_data(_Right);
        }
    }

    _CONSTEXPR20 void Split(const _Elem* _Delimiter, std::vector<WString>& _list) {
        _Elem* Safeptr = 0;
        _Elem* split = 0;

        if (IsEmpty()) {
            return;
        }

        split = wcstok_s(_Myptr(), _Delimiter, &Safeptr);
        while (split) {
            _list.push_back(split);
            split = wcstok_s(0,_Delimiter, &Safeptr);
        }
    }

    _NODISCARD _CONSTEXPR20 void Split(const WString& _Delimiter, std::vector<WString>& _list) noexcept {
        Split(_Delimiter.c_str(), _list);
    }

    _NODISCARD _CONSTEXPR20 void Split(const std::wstring& _Delimiter, std::vector<WString>& _list) noexcept {
        Split(_Delimiter.c_str(), _list);
    }

    _NODISCARD _CONSTEXPR20 bool MidStr(_In_z_ const _Elem* _Left, _In_z_ const _Elem* _Right, WString& Ret) noexcept {

        auto pos = find(_Left, 0);
        if (pos == npos) {
            return false;
        }

        pos += _Traits::length(_Left);

        auto end = find(_Right, pos);
        if (end == npos || end == pos) {
            return false;
        }


        Ret.Assign(_Myptr() + pos, _Myptr() + end);
        return true;
    }

    _NODISCARD _CONSTEXPR20 bool MidStr(const WString& _Left, const WString& _Right, WString& Ret) noexcept {
        return MidStr(_Left.c_str(), _Right.c_str(), Ret);
    }

    _NODISCARD _CONSTEXPR20 bool MidStr(const std::wstring& _Left, const std::wstring& _Right, WString& Ret) noexcept {
        return MidStr(_Left.c_str(), _Right.c_str(), Ret);
    }

    _NODISCARD _CONSTEXPR20 bool MoveImpl(_Elem* const _Right, const size_type _Size, const size_type _Actual_allocation_size) {

        _Traits::assign(_Right[_Size], _Elem());
        this->str = _Right;
        this->intlength = Ulong64toUint32(_Size);
        this->capacity = Ulong64toUint32(_Actual_allocation_size - 1);
        return true;
    }

    _NODISCARD _CONSTEXPR20 void shrink_to_fit() { // reduce capacity
        size_type _Target_capacity = (_STD min)(_Mysize() | _Alloc_mask, max_size());
        if (_Target_capacity < _Myres()) { // worth shrinking, do it

            const pointer _New_ptr = _Allocate_for_capacity(_Target_capacity); // throws

            _Traits::copy(_STD _Unfancy(_New_ptr), _STD _Unfancy(_Myptr()), _Mysize() + 1);
            _Deallocate_for_capacity(_Myptr());

            this->str = _New_ptr;
            this->capacity = Ulong64toUint32(_Target_capacity);
        }
    }

    WString& Format(_In_z_ _Printf_format_string_ const _Elem* const Format, ...) noexcept {
        va_list _ArgList;

        __crt_va_start(_ArgList, Format);
        return VFormat(Format, _ArgList);
    }

    WString& VFormat(_In_z_ _Printf_format_string_ const _Elem* const Format, va_list args) noexcept {
        _Elem* Buffer = 0;
        size_type _Count = 0;
        size_type _Length = 0;

        _Count = _vscwprintf(Format, args);

        if (!_Count || _Count == npos) {
            _Tidy_deallocate();
            return *this;
        }

        _Count++;

        Buffer = MemAlty.allocate<_Elem>(_Count);
        _Length = _vsnwprintf_s(Buffer, _Count, -1, Format, args);
        Assign(Buffer, _Length);

        MemAlty.deallocate(Buffer);
        return *this;
    }

    _NODISCARD _CONSTEXPR20 std::uint64_t Hash64() {
        auto offset_basis = 0xcbf29ce484222325ull;
        for (auto c : *this) {
            offset_basis ^= c;
            offset_basis *= 1099511628211ull;
        }

        return offset_basis;
    }

    _NODISCARD _CONSTEXPR20 std::uint64_t Hash64A() {
        auto offset_basis = 0xcbf29ce484222325ull;
        for (auto c : *this) {
            offset_basis ^= tolower(c);
            offset_basis *= 1099511628211ull;
        }

        return offset_basis;
    }

    _NODISCARD _CONSTEXPR20 std::uint32_t Hash32() {

        auto offset_basis = 0x811C9DC5u;
        for (auto c : *this) {
            offset_basis ^= c;
            offset_basis *= 0x1000193;
        }

        return offset_basis;
    }

    _NODISCARD _CONSTEXPR20 std::uint32_t Hash32A() {
        auto offset_basis = 0x811C9DC5u;
        for (auto c : *this) {
            offset_basis ^= tolower(c);
            offset_basis *= 0x1000193;
        }

        return offset_basis;
    }

    _NODISCARD void sort() const noexcept {
        std::sort(pointer(_Unchecked_begin()), pointer(_Unchecked_end()));
    }

    _NODISCARD void sort_desc() const noexcept {
        std::sort(pointer(_Unchecked_begin()), pointer(_Unchecked_end()),
            [](const WString& _Left, const WString& _Right) {
                return _Left > _Right;
            }
        );
    }

    _NODISCARD reference operator [] (const size_type _Off) noexcept {
        return _Myptr()[_Off];
    }

    _NODISCARD const_reference operator [] (const size_type _Off) const noexcept {
        return _Myptr()[_Off];
    }

    operator _STD basic_string_view<_Elem, _Traits>() const noexcept {
        return _STD basic_string_view<_Elem, _Traits>{_Myptr(), _Mysize()};
    }

    WString& operator = (const WString& _Right) {
        if (this == _STD addressof(_Right)) {
            return *this;
        }
        return Assign(_Right._Myptr(), _Right._Mysize());
    }

    WString& operator = (const WString&& _Right) noexcept {
        if (this == _STD addressof(_Right)) {
            return *this;
        }
        return Assign(_Right._Myptr(), _Right._Mysize());
    }

    WString& operator = (const std::wstring& _Right) {
        return Assign(_Right);
    }

    WString& operator = (_In_z_ const _Elem* const _Ptr) {
        return Assign(_Ptr);
    }

    WString& operator = (const _Elem _Ch) {
        return Assign(1, _Ch);
    }

    WString& operator = (_STD initializer_list<_Elem> _Ilist) {
        return Assign(_Ilist.begin(), _STD _Convert_size<size_type>(_Ilist.size()));
    }

    WString& operator = (const size_type Val) {
        if (Val) {
            intlength = Ulong64toUint32(Val);
        }
        return *this;
    }

    WString& operator = (const int Val) {
        if (Val) {
            intlength = Val;
        }
        return *this;
    }

    WString& operator = (const DWORD Val) {
        if (Val) {
            intlength = Val;
        }
        return *this;
    }

    WString& operator + (const WString& _Right) {
        return Append(_Right);
    }

    WString& operator + (const AString& _Right) {
        return Append(_Right);
    }

    WString& operator + (const WString&& _Right) {
        return Append(_Right);
    }

    WString& operator + (const std::wstring& _Right) {
        return Append(_Right);
    }

    WString& operator + (const std::string& _Right) {
        return Append(_Right);
    }

    WString& operator + (_In_z_ const _Elem* const _Ptr) {
        return Append(_Ptr);
    }

    WString& operator + (_In_z_ const char* const _Ptr) {
        return Append(_Ptr);
    }

    WString& operator + (_Elem _Ch) {
        push_back(_Ch);
        return *this;
    }

    WString& operator + (_STD initializer_list<_Elem> _Ilist) {
        return Append(_Ilist.begin(), _STD _Convert_size<size_type>(_Ilist.size()));
    }

    WString& operator += (const WString& _Right) {
        return operator +(_Right);
    }

    WString& operator += (const AString& _Right) {
        return operator +(_Right);
    }

    WString& operator += (const WString&& _Right) {
        return operator +(_Right);
    }

    WString& operator += (const std::wstring& _Right) {
        return operator +(_Right);
    }

    WString& operator += (const std::string& _Right) {
        return operator +(_Right);
    }

    WString& operator += (_In_z_ const _Elem* const _Ptr) {
        return operator +(_Ptr);
    }

    WString& operator += (_In_z_ const char* const _Ptr) {
        return operator +(_Ptr);
    }

    WString& operator += (_Elem _Ch) {
        return operator +(_Ch);
    }

    WString& operator += (_STD initializer_list<_Elem> _Ilist) {
        return operator +(_Ilist);
    }

    _NODISCARD _CONSTEXPR20 bool operator <  (const _Elem* _Right) const noexcept {
        return (Compare(_Right) < 0);
    }

    _NODISCARD _CONSTEXPR20 bool operator <  (const WString& _Right) const noexcept {
        return operator < (_Right._Myptr());
    }

    _NODISCARD _CONSTEXPR20 bool operator <  (const std::wstring& _Right) const noexcept {
        return operator < (_Right.c_str());
    }

    _NODISCARD _CONSTEXPR20 bool operator > (const _Elem* _Right)  const noexcept {
        return (Compare(_Right) > 0);
    }

    _NODISCARD _CONSTEXPR20 bool operator > (const WString& _Right)  const noexcept {
        return operator >(_Right._Myptr());
    }

    _NODISCARD _CONSTEXPR20 bool operator > (const std::wstring& _Right)  const noexcept {
        return operator >(_Right.c_str());
    }

    _NODISCARD _CONSTEXPR20 bool operator ! () const noexcept {
        return IsEmpty();
    }

    _NODISCARD _CONSTEXPR20 bool operator != (const _Elem* _Right) const noexcept {
        return !operator ==(_Right);
    }

    _NODISCARD _CONSTEXPR20 bool operator != (const WString& _Right) const noexcept {
        return !operator ==(_Right);
    }

    _NODISCARD _CONSTEXPR20 bool operator != (const std::wstring& _Right) const noexcept {
        return !operator ==(_Right);
    }

    _NODISCARD _CONSTEXPR20 bool operator == (const WString& _Right) const noexcept {
        if (_Right._Mysize() != _Mysize()) {
            return false;
        }
        return operator ==(_Right._Myptr());
    }

    _NODISCARD _CONSTEXPR20 bool operator == (const std::wstring& _Right) const noexcept {
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

    _NODISCARD _CONSTEXPR20 int Compare(const WString& _Right) const noexcept {
        return _STD _Traits_compare<_Traits>(_Myptr(), _Mysize(), _Right._Myptr(), _Right._Mysize());
    }

    _NODISCARD _CONSTEXPR20 int Compare(const std::wstring& _Right) const noexcept {
        return _STD _Traits_compare<_Traits>(_Myptr(), _Mysize(), _Right.data(), _Right.length());
    }

    _NODISCARD _CONSTEXPR20 int Compare(size_type _Off, size_type _Nx, const WString& _Right) const {
        _Check_offset(_Off);
        return _STD _Traits_compare<_Traits>(_Myptr() + _Off, _Clamp_suffix_size(_Off, _Nx), _Right._Myptr(), _Right._Mysize());
    }

    _NODISCARD _CONSTEXPR20 int Compare(const size_type _Off, const size_type _Nx, const WString& _Right, const size_type _Roff, const size_type _Count = npos) const {
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
#endif // _HAS_CXX17

    _NODISCARD int CompareI(_In_z_ const _Elem* const _Ptr) const noexcept {
        return _wcsicmp(_Myptr(), _Ptr);
    }

    _NODISCARD int CompareI(const WString& _Right) const noexcept {
        return CompareI(_Right._Myptr());
    }

    _NODISCARD int CompareI(const std::wstring& _Right) const noexcept {
        return CompareI(_Right.data());
    }

    _NODISCARD bool BeginsWith(const _Elem _Ch) const noexcept {
        return !IsEmpty() && _Traits::eq(front(), _Ch);
    }

    _NODISCARD bool BeginsWith(const _Elem* const _Right) const noexcept {
        return BeginsWith(_Right, _Traits::length(_Right));
    }

    _NODISCARD bool BeginsWith(const WString& _Right) const noexcept {
        return BeginsWith(_Right._Myptr(), _Right._Mysize());
    }

    _NODISCARD bool BeginsWith(const std::wstring& _Right) const noexcept {
        return BeginsWith(_Right.data(), _Right.length());
    }

    _NODISCARD bool BeginsWith(const _Elem* const _Right, const size_type _Count) const noexcept {
        if (_Count > _Mysize()) {
            return false;
        }

        return !Compare(_Right, _Count);
    }

    _NODISCARD bool BeginsWithI(const _Elem* const _Right) const noexcept {
        return BeginsWithI(_Right, _Traits::length(_Right));
    }

    _NODISCARD bool BeginsWithI(const WString& _Right) const noexcept {
        return BeginsWithI(_Right._Myptr(), _Right._Mysize());
    }

    _NODISCARD bool BeginsWithI(const std::wstring& _Right) const noexcept {
        return BeginsWithI(_Right.data(), _Right.length());
    }

    _NODISCARD bool BeginsWithI(const _Elem* const _Right, const size_type _Count) const noexcept {
        if (_Count > _Mysize()) {
            return false;
        }
        return StrCmpNIW(_Myptr(), _Right, Ulong64toUint32(_Count)) == 0;
    }

    _NODISCARD bool EndsWith(const _Elem _Right) const noexcept {
        return !IsEmpty() && _Traits::eq(back(), _Right);
    }

    _NODISCARD bool EndsWith(const _Elem* const _Right) const noexcept {
        const auto _Rightsize = _Traits::length(_Right);
        if (_Mysize() < _Rightsize) {
            return false;
        }
        return EndsWith(_Right, _Rightsize);
    }

    _NODISCARD bool EndsWith(const WString& _Right) const noexcept {
        return EndsWith(_Right._Myptr(), _Right._Mysize());
    }

    _NODISCARD bool EndsWith(const std::wstring& _Right) const noexcept {
        return EndsWith(_Right.c_str(), _Right.length());
    }

    _NODISCARD bool EndsWith(const _Elem* const _Right, const size_type _Count) const noexcept {
        if (_Mysize() < _Count) {
            return false;
        }
        return _Traits::compare(_Myptr() + (_Mysize() - _Count), _Right, _Count) == 0;
    }

    _NODISCARD bool EndsWithI(const _Elem* const _Right) const noexcept {
        return EndsWithI(_Right, _Traits::length(_Right));
    }

    _NODISCARD bool EndsWithI(const WString& _Right) const noexcept {
        return EndsWithI(_Right._Myptr(), _Right._Mysize());
    }

    _NODISCARD bool EndsWithI(const std::wstring& _Right) const noexcept {
        return EndsWithI(_Right.c_str(), _Right.length());
    }

    _NODISCARD bool EndsWithI(const _Elem* const _Right, const size_type _Count) const noexcept {
        if (_Mysize() < _Count) {
            return false;
        }

        return _wcsicmp((_Myptr() + (_Mysize() - _Count)), (_Right)) == 0;
    }

    _NODISCARD bool Equals(_In_z_ const _Elem* const _Ptr) const noexcept {
        return !wcscmp(_Myptr(), _Ptr);
    }

    _NODISCARD bool Equals(const WString& _Right) const noexcept {
        return _Mysize() == _Right._Mysize() && Equals(_Right._Myptr());
    }

    _NODISCARD bool Equals(const std::wstring& _Right) const noexcept {
        return _Mysize() == _Right.length() && Equals(_Right.data());
    }

    _NODISCARD bool EqualsI(_In_z_ const _Elem* const _Ptr) const noexcept {
        return !CompareI(_Ptr);
    }

    _NODISCARD bool EqualsI(const WString& _Right) const noexcept {
        return EqualsI(_Right._Myptr());
    }

    _NODISCARD bool EqualsI(const std::wstring& _Right) const noexcept {
        return EqualsI(_Right.c_str());
    }

    bool _Equal(_In_z_ const _Elem* const _Ptr) const noexcept {
        return _Equal(_Ptr, _Traits::length(_Ptr));
    }

    bool _Equal(const WString& _Right) const noexcept {
        return _Equal(_Right._Myptr(), _Right._Mysize());
    }

    bool _Equal(const std::wstring& _Right) const noexcept {
        return _Equal(_Right.data(), _Right.length());
    }

    bool _Equal(_In_z_ const _Elem* const _Ptr, const size_type _Count) const noexcept {
        return _STD _Traits_equal<_Traits>(_Myptr(), _Mysize(), _Ptr, _Count);
    }

    _NODISCARD bool Matches(_In_z_ const _Elem* _Right) noexcept {
        _Elem* _Ptr = _Myptr();
        bool star;


    new_segment:
        star = false;
        if (*_Right == L'*') {
            star = true;
            do { _Right++; } while (*_Right == L'*');
        }

    test_match:
        int i;
        for (i = 0; _Right[i] && (_Right[i] != L'*'); i++) {
            if (_Ptr[i] != _Right[i]) {
                if (!_Ptr[i]) {
                    return false;
                }
                if ((_Right[i] == L'?') && (_Ptr[i] != L'.')) {
                    continue;
                }
                if (!star) {
                    return false;
                }
                _Ptr++;
                goto test_match;
            }
        }
        if (_Right[i] == L'*') {
            _Ptr += i;
            _Right += i;
            goto new_segment;
        }
        if (!_Ptr[i]) {
            return true;
        }
        if (i && _Right[i - 1] == L'*') {
            return true;
        }
        if (!star) {
            return false;
        }
        _Ptr++;
        goto test_match;
        return false;
    }

    _NODISCARD bool Matches(const WString& _Right)noexcept {
        return Matches(_Right.c_str());
    }

    _NODISCARD bool Matches(const std::wstring& _Right)noexcept {
        return Matches(_Right.c_str());
    }

    _NODISCARD bool MatchesI(_In_z_ const _Elem* _Right) noexcept {
        bool star;
        const _Elem* _Ptr = _Myptr();

    new_segment:
        star = false;
        if (*_Right == L'*') {
            star = true;
            do { _Right++; } while (*_Right == L'*');
        }

    test_match:
        int i;
        for (i = 0; _Right[i] && (_Right[i] != L'*'); i++) {
            wchar_t a = _Ptr[i]; a = ((a >= L'A') && (a <= L'Z')) ? L'a' + (a - L'A') : a;
            wchar_t b = _Right[i]; b = ((b >= L'A') && (b <= L'Z')) ? L'a' + (b - L'A') : b;
            if (a != b) {
                if (!_Ptr[i]) {
                    return false;
                }
                if ((_Right[i] == L'?') && (_Ptr[i] != L'.')) continue;
                if (!star) {
                    return false;
                }
                _Ptr++;
                goto test_match;
            }
        }
        if (_Right[i] == L'*')
        {
            _Ptr += i;
            _Right += i;
            goto new_segment;
        }
        if (!_Ptr[i]) {
            return true;
        }
        if (i && _Right[i - 1] == L'*') {
            return true;
        }
        if (!star) {
            return false;
        }
        _Ptr++;
        goto test_match;
        return false;
    }

    _NODISCARD bool MatchesI(const WString& _Right) noexcept {
        return MatchesI(_Right.c_str());
    }

    _NODISCARD bool MatchesI(const std::wstring& _Right) noexcept {
        return MatchesI(_Right.c_str());
    }

    _NODISCARD void Uplength() {
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
#endif // _HAS_CXX17

    _NODISCARD _CONSTEXPR20 size_type find(const WString& _Right, const size_type _Off = 0) const noexcept {
        return static_cast<size_type>(_STD _Traits_find<_Traits>(_Myptr(), _Mysize(), _Off, _Right._Myptr(), _Right._Mysize()));
    }

    _NODISCARD _CONSTEXPR20 size_type find(_In_reads_(_Count) const _Elem* const _Ptr, const size_type _Off, const size_type _Count) const noexcept {
        return static_cast<size_type>(_STD _Traits_find<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Count));
    }

    _NODISCARD _CONSTEXPR20 size_type find(_In_z_ const _Elem* const _Ptr, const size_type _Off) const noexcept {
        return static_cast<size_type>(_STD _Traits_find<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Traits::length(_Ptr)));
    }

    _NODISCARD bool find(_In_z_ const _Elem* const _Ptr) noexcept {
        WString src;
        WString startPos;

        src = _Myptr();
        startPos = _Ptr;

        if (!_Mysize()) {
            return false;
        }

        src.ToLower();
        startPos.ToLower();

        return std::wcsstr(src._Myptr(), startPos._Myptr()) != 0;
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
#endif // _HAS_CXX17
    _NODISCARD _CONSTEXPR20 size_type rfind(const WString& _Right, const size_type _Off = npos) const noexcept {
        return static_cast<size_type>(_STD _Traits_rfind<_Traits>(_Myptr(), _Mysize(), _Off, _Right._Myptr(), _Right._Mysize()));
    }

    _NODISCARD _CONSTEXPR20 size_type rfind(_In_reads_(_Count) const _Elem* const _Ptr, const size_type _Off, const size_type _Count) const noexcept {
        return static_cast<size_type>(_STD _Traits_rfind<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Count));
    }

    _NODISCARD _CONSTEXPR20 size_type rfind(_In_z_ const _Elem* const _Ptr, const size_type _Off = npos) const noexcept {
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
#endif // _HAS_CXX17

    _NODISCARD constexpr size_type find_first_of(const WString& _Right, const size_type _Off = 0) const noexcept {
        return static_cast<size_type>(_STD _Traits_find_first_of<_Traits>(_Myptr(), _Mysize(), _Off, _Right._Myptr(), _Right._Mysize()));
    }

    _NODISCARD constexpr size_type find_first_of(_In_reads_(_Count) const _Elem* const _Ptr, const size_type _Off, const size_type _Count) const noexcept {
        return static_cast<size_type>(_STD _Traits_find_first_of<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Count));
    }

    _NODISCARD constexpr size_type find_first_of(_In_z_ const _Elem* const _Ptr, const size_type _Off = 0) const noexcept {
        return static_cast<size_type>(_STD _Traits_find_first_of<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Traits::length(_Ptr)));
    }

    _NODISCARD constexpr size_type find_first_of(const _Elem _Ch, const size_type _Off = 0) const noexcept {
        return static_cast<size_type>(_STD _Traits_find_ch<_Traits>(_Myptr(), _Mysize(), _Off, _Ch));
    }

#if _HAS_CXX17
    template <class _StringViewIsh, _Is_string_view_ish<_StringViewIsh> = 0>
    _NODISCARD constexpr size_type find_last_of(const _StringViewIsh& _Right, const size_type _Off = npos) const
        noexcept(_STD _Is_nothrow_convertible_v<const _StringViewIsh&, _STD  basic_string_view<_Elem, _Traits>>) {
        // look for one of _Right before _Off
        _STD basic_string_view<_Elem, _Traits> _As_view = _Right;
        return static_cast<size_type>(_STD _Traits_find_last_of<_Traits>(_Myptr(), _Mysize(), _Off, _As_view.data(), _As_view.size()));
    }
#endif // _HAS_CXX17

    _NODISCARD constexpr size_type find_last_of(const WString& _Right, size_type _Off = npos) const noexcept {
        // look for one of _Right before _Off
        return static_cast<size_type>(_STD _Traits_find_last_of<_Traits>(_Myptr(), _Mysize(), _Off, _Right._Myptr(), _Right._Mysize()));
    }

    _NODISCARD constexpr size_type find_last_of(_In_reads_(_Count) const _Elem* const _Ptr, const size_type _Off,
        const size_type _Count) const noexcept /* strengthened */ {
        // look for one of [_Ptr, _Ptr + _Count) before _Off
        return static_cast<size_type>(_STD _Traits_find_last_of<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Count));
    }

    _NODISCARD constexpr size_type find_last_of(
        _In_z_ const _Elem* const _Ptr, const size_type _Off = npos) const noexcept /* strengthened */ {
        // look for one of [_Ptr, <null>) before _Off
        return static_cast<size_type>(_STD _Traits_find_last_of<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Traits::length(_Ptr)));
    }

    _NODISCARD constexpr size_type find_last_of(const _Elem _Ch, const size_type _Off = npos) const noexcept {
        // look for _Ch before _Off
        return static_cast<size_type>(_STD _Traits_rfind_ch<_Traits>(_Myptr(), _Mysize(), _Off, _Ch));
    }

#if _HAS_CXX17
    template <class _StringViewIsh, _Is_string_view_ish<_StringViewIsh> = 0>
    _NODISCARD constexpr size_type find_first_not_of(const _StringViewIsh& _Right, const size_type _Off = 0) const
        noexcept(_STD _Is_nothrow_convertible_v<const _StringViewIsh&, _STD basic_string_view<_Elem, _Traits>>) {
        _STD basic_string_view<_Elem, _Traits> _As_view = _Right;
        return static_cast<size_type>(_STD _Traits_find_first_not_of<_Traits>(_Myptr(), _Mysize(), _Off, _As_view.data(), _As_view.size()));
    }
#endif // _HAS_CXX17

    _NODISCARD constexpr size_type find_first_not_of(const WString& _Right, const size_type _Off = 0) const noexcept {
        return static_cast<size_type>(_STD _Traits_find_first_not_of<_Traits>(_Myptr(), _Mysize(), _Off, _Right._Myptr(), _Right._Mysize()));
    }

    _NODISCARD constexpr size_type find_first_not_of(_In_reads_(_Count) const _Elem* const _Ptr, const size_type _Off, const size_type _Count) const noexcept {
        return static_cast<size_type>(_STD _Traits_find_first_not_of<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Count));
    }

    _NODISCARD constexpr size_type find_first_not_of(_In_z_ const _Elem* const _Ptr, size_type _Off = 0) const noexcept {
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
#endif // _HAS_CXX17

    _NODISCARD constexpr size_type find_last_not_of(const WString& _Right, const size_type _Off = npos) const noexcept {
        return static_cast<size_type>(_STD _Traits_find_last_not_of<_Traits>(_Myptr(), _Mysize(), _Off, _Right._Myptr(), _Right._Mysize()));
    }

    _NODISCARD constexpr size_type find_last_not_of(_In_reads_(_Count) const _Elem* const _Ptr, const size_type _Off, const size_type _Count) const noexcept {
        return static_cast<size_type>(_STD _Traits_find_last_not_of<_Traits>(_Myptr(), _Mysize(), _Off, _Ptr, _Count));
    }

    _NODISCARD constexpr size_type find_last_not_of(_In_z_ const _Elem* const _Ptr, const size_type _Off = npos) const noexcept {
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
        return (_STD min)(static_cast<size_type>((_STD numeric_limits<difference_type>::max)()), _Storage_max - 1);
    }

protected:
    pointer str;
    std::uint32_t intlength;
    std::uint32_t capacity;
private:

    _CONSTEXPR20 void _Construct_empty() {
        this->str = const_cast<wchar_t*>(w_EmptyString);
        this->intlength = 0;
        this->capacity = 0;
    }

    _NODISCARD pointer _Allocate_for_capacity(size_type& _Capacity) {
        _STL_INTERNAL_CHECK(_Capacity > _Small_string_capacity);

        ++_Capacity;
        pointer _Fancy_ptr = MemAlty.allocate<_Elem>(_Capacity);
        --_Capacity;
        return _Fancy_ptr;
    }

    void _Deallocate_for_capacity(const pointer _Old_ptr) noexcept {
        MemAlty.deallocate(_Old_ptr);
    }

    _NODISCARD bool _Is_Tidy_deallocate() const noexcept {
        return ((_Myres() & MEM_MUST_BE_FREED_FLAG) == MEM_MUST_BE_FREED_FLAG) && _Myptr() != 0 && _Myptr() != const_cast<wchar_t*>(w_EmptyString);
    }

    _NODISCARD void _Tidy_deallocate() noexcept {

        if (_Is_Tidy_deallocate()) {
            _Deallocate_for_capacity(_Myptr());
        }

        _Construct_empty();
    }

    template <class _Fty, class... _ArgTys>
    _CONSTEXPR20 WString& _Reallocate_for(const size_type _New_size, _Fty _Fn, _ArgTys... _Args) {

        if (_New_size > max_size()) {
            _STD _Xlen_string();
        }

        const size_type _Old_capacity = _Myres();
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
    _CONSTEXPR20 WString& _Reallocate_grow_by(const size_type _Size_increase, _Fty _Fn, _ArgTys... _Args) {

        const size_type _Old_size = _Mysize();
        if (max_size() - _Old_size < _Size_increase) {
            _STD _Xlen_string(); // result too long
        }

        const size_type _New_size = _Old_size + _Size_increase;
        const size_type _Old_capacity = _Myres();
        size_type _New_capacity = _Calculate_growth(_New_size);
        const pointer _New_ptr = _Allocate_for_capacity(_New_capacity); // throws

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

    WString& _Erase_noexcept(const size_type _Off, size_type _Count) noexcept {
        _Count = _Clamp_suffix_size(_Off, _Count);
        const size_type _Old_size = _Mysize();
        _Elem* const _My_ptr = _Myptr();
        _Elem* const _Erase_at = _My_ptr + _Off;
        const size_type _New_size = _Old_size - _Count;
        _Traits::move(_Erase_at, _Erase_at + _Count, _New_size - _Off + 1); // move suffix + null up
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
        if (_Masked > _Max) { // the mask overflows, settle for max_size()
            return _Max;
        }

        if (_Old > _Max - _Old / 2) { // similarly, geometric overflows
            return _Max;
        }

        return (_STD max)(_Masked, _Old + _Old / 2);
    }

    _NODISCARD _CONSTEXPR20 size_type _Calculate_growth(const size_type _Requested) const noexcept {
        return _Calculate_growth(_Requested, _Myres(), max_size());
    }

    _CONSTEXPR20 void _Swap_data(WString& _Right) noexcept {
        using _STD swap;

        _STD swap(this->str, _Right.str);
        _STD swap(this->intlength, _Right.intlength);
        _STD swap(this->capacity, _Right.capacity);
    }

    _CONSTEXPR20 size_type copy(_Out_writes_(_Count) _Elem* const _Ptr, size_type _Count, const size_type _Off = 0) const {
        _Check_offset(_Off);
        _Count = _Clamp_suffix_size(_Off, _Count);
        _Traits::copy(_Ptr, _Myptr() + _Off, _Count);
        return _Count;
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

inline AString& AString::Assign(const WString& _Right) {
    return Assign(_Right.data());
}

inline AString& AString::Append(const WString& _Right) {
    return Append(_Right.data());
}

inline void swap(AString& _Left, AString& _Right) noexcept {
    _Left.swap(_Right);
}

inline void swap(WString& _Left, WString& _Right) noexcept {
    _Left.swap(_Right);
}

_NODISCARD inline int stoi(const AString& _Str, std::size_t* _Idx = nullptr, int _Base = 10) {
    int& _Errno_ref = errno; // Nonzero cost, pay it once
    const char* _Ptr = _Str.c_str();
    char* _Eptr;
    _Errno_ref = 0;
    const long _Ans = _CSTD strtol(_Ptr, &_Eptr, _Base);

    if (_Ptr == _Eptr) {
        _STD _Xinvalid_argument("invalid stoi argument");
    }

    if (_Errno_ref == ERANGE) {
        _STD _Xout_of_range("stoi argument out of range");
    }

    if (_Idx) {
        *_Idx = static_cast<std::size_t>(_Eptr - _Ptr);
    }

    return static_cast<int>(_Ans);
}

_NODISCARD inline long stol(const AString& _Str, size_t* _Idx = nullptr, int _Base = 10) {
    int& _Errno_ref = errno; // Nonzero cost, pay it once
    const char* _Ptr = _Str.c_str();
    char* _Eptr;
    _Errno_ref = 0;
    const long _Ans = _CSTD strtol(_Ptr, &_Eptr, _Base);

    if (_Ptr == _Eptr) {
        _STD _Xinvalid_argument("invalid stol argument");
    }

    if (_Errno_ref == ERANGE) {
        _STD _Xout_of_range("stol argument out of range");
    }

    if (_Idx) {
        *_Idx = static_cast<size_t>(_Eptr - _Ptr);
    }

    return _Ans;
}

_NODISCARD inline unsigned long stoul(const AString& _Str, size_t* _Idx = nullptr, int _Base = 10) {
    int& _Errno_ref = errno; // Nonzero cost, pay it once
    const char* _Ptr = _Str.c_str();
    char* _Eptr;
    _Errno_ref = 0;
    const unsigned long _Ans = _CSTD strtoul(_Ptr, &_Eptr, _Base);

    if (_Ptr == _Eptr) {
        _STD _Xinvalid_argument("invalid stoul argument");
    }

    if (_Errno_ref == ERANGE) {
        _STD _Xout_of_range("stoul argument out of range");
    }

    if (_Idx) {
        *_Idx = static_cast<size_t>(_Eptr - _Ptr);
    }

    return _Ans;
}

_NODISCARD inline long long stoll(const AString& _Str, size_t* _Idx = nullptr, int _Base = 10) {
    int& _Errno_ref = errno; // Nonzero cost, pay it once
    const char* _Ptr = _Str.c_str();
    char* _Eptr;
    _Errno_ref = 0;
    const long long _Ans = _CSTD strtoll(_Ptr, &_Eptr, _Base);

    if (_Ptr == _Eptr) {
        _STD _Xinvalid_argument("invalid stoll argument");
    }

    if (_Errno_ref == ERANGE) {
        _STD _Xout_of_range("stoll argument out of range");
    }

    if (_Idx) {
        *_Idx = static_cast<size_t>(_Eptr - _Ptr);
    }

    return _Ans;
}

_NODISCARD inline unsigned long long stoull(const AString& _Str, size_t* _Idx = nullptr, int _Base = 10) {
    int& _Errno_ref = errno; // Nonzero cost, pay it once
    const char* _Ptr = _Str.c_str();
    char* _Eptr;
    _Errno_ref = 0;
    const unsigned long long _Ans = _CSTD strtoull(_Ptr, &_Eptr, _Base);

    if (_Ptr == _Eptr) {
        _STD _Xinvalid_argument("invalid stoull argument");
    }

    if (_Errno_ref == ERANGE) {
        _STD _Xout_of_range("stoull argument out of range");
    }

    if (_Idx) {
        *_Idx = static_cast<size_t>(_Eptr - _Ptr);
    }

    return _Ans;
}

_NODISCARD inline float stof(const AString& _Str, size_t* _Idx = nullptr) {
    int& _Errno_ref = errno; // Nonzero cost, pay it once
    const char* _Ptr = _Str.c_str();
    char* _Eptr;
    _Errno_ref = 0;
    const float _Ans = _CSTD strtof(_Ptr, &_Eptr);

    if (_Ptr == _Eptr) {
        _STD _Xinvalid_argument("invalid stof argument");
    }

    if (_Errno_ref == ERANGE) {
        _STD _Xout_of_range("stof argument out of range");
    }

    if (_Idx) {
        *_Idx = static_cast<size_t>(_Eptr - _Ptr);
    }

    return _Ans;
}

_NODISCARD inline double stod(const AString& _Str, size_t* _Idx = nullptr) {
    int& _Errno_ref = errno; // Nonzero cost, pay it once
    const char* _Ptr = _Str.c_str();
    char* _Eptr;
    _Errno_ref = 0;
    const double _Ans = _CSTD strtod(_Ptr, &_Eptr);

    if (_Ptr == _Eptr) {
        _STD _Xinvalid_argument("invalid stod argument");
    }

    if (_Errno_ref == ERANGE) {
        _STD _Xout_of_range("stod argument out of range");
    }

    if (_Idx) {
        *_Idx = static_cast<size_t>(_Eptr - _Ptr);
    }

    return _Ans;
}

_NODISCARD inline long double stold(const AString& _Str, size_t* _Idx = nullptr) {
    int& _Errno_ref = errno; // Nonzero cost, pay it once
    const char* _Ptr = _Str.c_str();
    char* _Eptr;
    _Errno_ref = 0;
    const long double _Ans = _CSTD strtold(_Ptr, &_Eptr);

    if (_Ptr == _Eptr) {
        _STD _Xinvalid_argument("invalid stold argument");
    }

    if (_Errno_ref == ERANGE) {
        _STD _Xout_of_range("stold argument out of range");
    }

    if (_Idx) {
        *_Idx = static_cast<size_t>(_Eptr - _Ptr);
    }

    return _Ans;
}

_NODISCARD inline int stoi(const WString& _Str, size_t* _Idx = nullptr, int _Base = 10) {
    int& _Errno_ref = errno; // Nonzero cost, pay it once
    const wchar_t* _Ptr = _Str.c_str();
    wchar_t* _Eptr;
    _Errno_ref = 0;
    const long _Ans = _CSTD wcstol(_Ptr, &_Eptr, _Base);

    if (_Ptr == _Eptr) {
        _STD _Xinvalid_argument("invalid stoi argument");
    }

    if (_Errno_ref == ERANGE) {
        _STD _Xout_of_range("stoi argument out of range");
    }

    if (_Idx) {
        *_Idx = static_cast<size_t>(_Eptr - _Ptr);
    }

    return static_cast<int>(_Ans);
}

_NODISCARD inline long stol(const WString& _Str, size_t* _Idx = nullptr, int _Base = 10) {
    int& _Errno_ref = errno; // Nonzero cost, pay it once
    const wchar_t* _Ptr = _Str.c_str();
    wchar_t* _Eptr;
    _Errno_ref = 0;
    const long _Ans = _CSTD wcstol(_Ptr, &_Eptr, _Base);

    if (_Ptr == _Eptr) {
        _STD _Xinvalid_argument("invalid stol argument");
    }

    if (_Errno_ref == ERANGE) {
        _STD _Xout_of_range("stol argument out of range");
    }

    if (_Idx) {
        *_Idx = static_cast<size_t>(_Eptr - _Ptr);
    }

    return _Ans;
}

_NODISCARD inline unsigned long stoul(const WString& _Str, size_t* _Idx = nullptr, int _Base = 10) {
    int& _Errno_ref = errno; // Nonzero cost, pay it once
    const wchar_t* _Ptr = _Str.c_str();
    wchar_t* _Eptr;
    _Errno_ref = 0;
    const unsigned long _Ans = _CSTD wcstoul(_Ptr, &_Eptr, _Base);

    if (_Ptr == _Eptr) {
        _STD _Xinvalid_argument("invalid stoul argument");
    }

    if (_Errno_ref == ERANGE) {
        _STD _Xout_of_range("stoul argument out of range");
    }

    if (_Idx) {
        *_Idx = static_cast<size_t>(_Eptr - _Ptr);
    }

    return _Ans;
}

_NODISCARD inline long long stoll(const WString& _Str, size_t* _Idx = nullptr, int _Base = 10) {
    int& _Errno_ref = errno; // Nonzero cost, pay it once
    const wchar_t* _Ptr = _Str.c_str();
    wchar_t* _Eptr;
    _Errno_ref = 0;
    const long long _Ans = _CSTD wcstoll(_Ptr, &_Eptr, _Base);

    if (_Ptr == _Eptr) {
        _STD _Xinvalid_argument("invalid stoll argument");
    }

    if (_Errno_ref == ERANGE) {
        _STD _Xout_of_range("stoll argument out of range");
    }

    if (_Idx) {
        *_Idx = static_cast<size_t>(_Eptr - _Ptr);
    }

    return _Ans;
}

_NODISCARD inline unsigned long long stoull(const WString& _Str, size_t* _Idx = nullptr, int _Base = 10) {
    int& _Errno_ref = errno; // Nonzero cost, pay it once
    const wchar_t* _Ptr = _Str.c_str();
    wchar_t* _Eptr;
    _Errno_ref = 0;
    const unsigned long long _Ans = _CSTD wcstoull(_Ptr, &_Eptr, _Base);

    if (_Ptr == _Eptr) {
        _STD _Xinvalid_argument("invalid stoull argument");
    }

    if (_Errno_ref == ERANGE) {
        _STD _Xout_of_range("stoull argument out of range");
    }

    if (_Idx) {
        *_Idx = static_cast<size_t>(_Eptr - _Ptr);
    }

    return _Ans;
}

_NODISCARD inline float stof(const WString& _Str, size_t* _Idx = nullptr) {
    int& _Errno_ref = errno; // Nonzero cost, pay it once
    const wchar_t* _Ptr = _Str.c_str();
    wchar_t* _Eptr;
    _Errno_ref = 0;
    const float _Ans = _CSTD wcstof(_Ptr, &_Eptr);

    if (_Ptr == _Eptr) {
        _STD _Xinvalid_argument("invalid stof argument");
    }

    if (_Errno_ref == ERANGE) {
        _STD _Xout_of_range("stof argument out of range");
    }

    if (_Idx) {
        *_Idx = static_cast<size_t>(_Eptr - _Ptr);
    }

    return _Ans;
}

_NODISCARD inline double stod(const WString& _Str, size_t* _Idx = nullptr) {
    int& _Errno_ref = errno; // Nonzero cost, pay it once
    const wchar_t* _Ptr = _Str.c_str();
    wchar_t* _Eptr;
    _Errno_ref = 0;
    const double _Ans = _CSTD wcstod(_Ptr, &_Eptr);

    if (_Ptr == _Eptr) {
        _STD _Xinvalid_argument("invalid stod argument");
    }

    if (_Errno_ref == ERANGE) {
        _STD _Xout_of_range("stod argument out of range");
    }

    if (_Idx) {
        *_Idx = static_cast<size_t>(_Eptr - _Ptr);
    }

    return _Ans;
}

_NODISCARD inline long double stold(const WString& _Str, size_t* _Idx = nullptr) {
    int& _Errno_ref = errno; // Nonzero cost, pay it once
    const wchar_t* _Ptr = _Str.c_str();
    wchar_t* _Eptr;
    _Errno_ref = 0;
    const long double _Ans = _CSTD wcstold(_Ptr, &_Eptr);

    if (_Ptr == _Eptr) {
        _STD _Xinvalid_argument("invalid stold argument");
    }

    if (_Errno_ref == ERANGE) {
        _STD _Xout_of_range("stold argument out of range");
    }

    if (_Idx) {
        *_Idx = static_cast<size_t>(_Eptr - _Ptr);
    }

    return _Ans;
}

template <class _Elem, class _Ty>
_NODISCARD _Elem _Integral_to_string(const _Ty _Val) {
    // convert _Val to string
    static_assert(_STD is_integral_v<_Ty>, "_Ty must be integral");
    using _UTy = _STD make_unsigned_t<_Ty>;
    typename _Elem::value_type _Buff[21]; // can hold -2^63 and 2^64 - 1, plus NUL
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
    // convert _Val to string
    static_assert(_STD is_integral_v<_Ty>, "_Ty must be integral");
    static_assert(_STD is_unsigned_v<_Ty>, "_Ty must be unsigned");
    typename _Elem::value_type _Buff[21]; // can hold 2^64 - 1, plus NUL
    typename _Elem::value_type* const _Buff_end = _STD end(_Buff);
    typename _Elem::value_type* const _RNext = _STD _UIntegral_to_buff(_Buff_end, _Val);
    return _Elem(_RNext, _Buff_end);
}

_NODISCARD inline AString to_AString(int _Val) {
    return _Integral_to_string<AString, int>(_Val);
}

_NODISCARD inline AString to_AString(unsigned int _Val) {
    return _UIntegral_to_string<AString, unsigned int>(_Val);
}

_NODISCARD inline AString to_AString(long _Val) {
    return _Integral_to_string<AString, long>(_Val);
}

_NODISCARD inline AString to_AString(unsigned long _Val) {
    return _UIntegral_to_string<AString, unsigned long>(_Val);
}

_NODISCARD inline AString to_AString(long long _Val) {
    return _Integral_to_string<AString, long long>(_Val);
}

_NODISCARD inline AString to_AString(unsigned long long _Val) {
    return _UIntegral_to_string<AString, unsigned long long>(_Val);
}

_NODISCARD inline AString to_AString(double _Val) {
    const auto _Len = static_cast<size_t>(_CSTD _scprintf("%f", _Val));
    AString _Str(_Len, '\0');
    _CSTD sprintf_s(&_Str[0], _Len + 1, "%f", _Val);
    return _Str;
}

_NODISCARD inline AString to_AString(float _Val) {
    return to_AString(static_cast<double>(_Val));
}

_NODISCARD inline AString to_AString(long double _Val) {
    return to_AString(static_cast<double>(_Val));
}

_NODISCARD inline WString to_WString(int _Val) {
    return _Integral_to_string<WString, int>(_Val);
}

_NODISCARD inline WString to_WString(unsigned int _Val) {
    return _UIntegral_to_string<WString, unsigned int>(_Val);
}

_NODISCARD inline WString to_WString(long _Val) {
    return _Integral_to_string<WString, long>(_Val);
}

_NODISCARD inline WString to_WString(unsigned long _Val) {
    return _UIntegral_to_string<WString, unsigned long>(_Val);
}

_NODISCARD inline WString to_WString(long long _Val) {
    return _Integral_to_string<WString, long long>(_Val);
}

_NODISCARD inline WString to_WString(unsigned long long _Val) {
    return _UIntegral_to_string<WString, unsigned long long>(_Val);
}

_NODISCARD inline WString to_WString(double _Val) {
    const auto _Len = static_cast<size_t>(_CSTD _scwprintf(L"%f", _Val));
    WString _Str(_Len, L'\0');
    _CSTD swprintf_s(&_Str[0], _Len + 1, L"%f", _Val);
    return _Str;
}

_NODISCARD inline WString to_WString(float _Val) {
    return to_WString(static_cast<double>(_Val));
}

_NODISCARD inline WString to_WString(long double _Val) {
    return to_WString(static_cast<double>(_Val));
}

inline AString& operator<<(AString& _Left, const char* _Right) {
    return _Left.operator+(_Right);
}

inline AString& operator<<(AString& _Left, const AString& _Right) {
    return _Left.operator+(_Right);
}

inline AString& operator<<(AString& _Left, int _Val) {
    auto _Str = to_AString(_Val);
    if (_Str.IsEmpty()) {
        return _Left;
    }

    return _Left.operator+(_Str);
}

inline AString& operator<<(AString& _Left, unsigned int _Val) {
    auto _Str = to_AString(_Val);
    if (_Str.IsEmpty()) {
        return _Left;
    }

    return _Left.operator+(_Str);
}

inline AString& operator<<(AString& _Left, long _Val) {
    auto _Str = to_AString(_Val);
    if (_Str.IsEmpty()) {
        return _Left;
    }

    return _Left.operator+(_Str);
}

inline AString& operator<<(AString& _Left, unsigned long _Val) {
    auto _Str = to_AString(_Val);
    if (_Str.IsEmpty()) {
        return _Left;
    }

    return _Left.operator+(_Str);
}

inline AString& operator<<(AString& _Left, long long _Val) {
    auto _Str = to_AString(_Val);
    if (_Str.IsEmpty()) {
        return _Left;
    }

    return _Left.operator+(_Str);
}

inline AString& operator<<(AString& _Left, unsigned long long _Val) {
    auto _Str = to_AString(_Val);
    if (_Str.IsEmpty()) {
        return _Left;
    }

    return _Left.operator+(_Str);
}

inline AString& operator<<(AString& _Left, double _Val) {
    auto _Str = to_AString(_Val);
    if (_Str.IsEmpty()) {
        return _Left;
    }

    return _Left.operator+(_Str);
}

inline AString& operator<<(AString& _Left, float _Val) {
    auto _Str = to_AString(_Val);
    if (_Str.IsEmpty()) {
        return _Left;
    }

    return _Left.operator+(_Str);
}

inline AString& operator<<(AString& _Left, long double _Val) {
    auto _Str = to_AString(_Val);
    if (_Str.IsEmpty()) {
        return _Left;
    }

    return _Left.operator+(_Str);
}

inline WString& operator<<(WString& _Left, const WString& _Right) {
    return _Left.operator+(_Right);
}

inline WString& operator<<(WString& _Left, const wchar_t* _Right) {
    return _Left.operator+(_Right);
}

inline WString& operator<<(WString& _Left, int _Val) {
    auto _Str = to_WString(_Val);
    if (_Str.IsEmpty()) {
        return _Left;
    }

    return _Left.operator+(_Str);
}

inline WString& operator<<(WString& _Left, unsigned int _Val) {
    auto _Str = to_WString(_Val);
    if (_Str.IsEmpty()) {
        return _Left;
    }

    return _Left.operator+(_Str);
}

inline WString& operator<<(WString& _Left, long _Val) {
    auto _Str = to_WString(_Val);
    if (_Str.IsEmpty()) {
        return _Left;
    }

    return _Left.operator+(_Str);
}

inline WString& operator<<(WString& _Left, unsigned long _Val) {
    auto _Str = to_WString(_Val);
    if (_Str.IsEmpty()) {
        return _Left;
    }

    return _Left.operator+(_Str);
}

inline WString& operator<<(WString& _Left, long long _Val) {
    auto _Str = to_WString(_Val);
    if (_Str.IsEmpty()) {
        return _Left;
    }

    return _Left.operator+(_Str);
}

inline WString& operator<<(WString& _Left, unsigned long long _Val) {
    auto _Str = to_WString(_Val);
    if (_Str.IsEmpty()) {
        return _Left;
    }

    return _Left.operator+(_Str);
}

inline WString& operator<<(WString& _Left, double _Val) {
    auto _Str = to_WString(_Val);
    if (_Str.IsEmpty()) {
        return _Left;
    }

    return _Left.operator+(_Str);
}

inline WString& operator<<(WString& _Left, float _Val) {
    auto _Str = to_WString(_Val);
    if (_Str.IsEmpty()) {
        return _Left;
    }

    return _Left.operator+(_Str);
}

inline WString& operator<<(WString& _Left, long double _Val) {
    auto _Str = to_WString(_Val);
    if (_Str.IsEmpty()) {
        return _Left;
    }

    return _Left.operator+(_Str);
}

inline AString& operator+(AString& _Left, _In_z_ const wchar_t* const _Right) {
    EncodeCodecvt<char> _convert;

    if (!_convert.ToAnsi(_Right)) {
        return _Left;
    }

    return _Left.operator+(_convert._Myptr());
}

_NODISCARD inline std::size_t PreNUm(std::uint8_t byte) {
    std::uint8_t Mask = 0x80;
    std::size_t num = 0;

    for (std::size_t i = 0; i < 8; i++) {
        if ((byte & Mask) == Mask) {
            Mask = Mask >> 1;
            num++;
        }
        else {
            break;
        }
    }
    return num;
}

_NODISCARD inline bool AStringIsUTF8(std::uint8_t* Data, std::size_t _Count) {
    std::size_t num = 0;
    std::size_t i = 0;

    while (i < _Count) {
        if ((Data[i] & 0x80) == 0x00) {
            i++;
            continue;
        }
        else if ((num = PreNUm(Data[i])) > 2) {
            i++;
            for (size_t j = 0; j < num - 1; j++) {
                if ((Data[i] & 0xC0) != 0x80) {
                    return false;
                }
                i++;
            }
        }
        else {
            return false;
        }
    }
    return true;
}

inline AString& Utf8_to_AString(AString& _Right) {
    std::uint32_t length = 0;
    wchar_t* Utf8 = 0;
    char* Ansi = 0;

    if (_Right.IsEmpty()) {
        return _Right;
    }

    if (!AStringIsUTF8((std::uint8_t*)_Right.data(), _Right.length())) {
        return _Right;
    }


    length = MultiByteToWideChar(CP_UTF8, 0, _Right.data(), Ulong64toUint32(_Right.length()), 0, 0);
    if (!length) {
        return _Right;
    }

    Utf8 = new wchar_t[length + 1] { 0 };
    if (!Utf8) {
        return _Right;
    }

    MultiByteToWideChar(CP_UTF8, 0, _Right.data(), Ulong64toUint32(_Right.length()), Utf8, length);
    length = WideCharToMultiByte(CP_ACP, 0, Utf8, -1, 0, 0, 0, 0);
    if (!length) {
        delete[] Utf8;
        return _Right;
    }

    Ansi = new char[length + 1] { 0 };

    WideCharToMultiByte(CP_ACP, 0, Utf8, -1, Ansi, length, 0, 0);
    _Right.Assign(Ansi);

    if (!Ansi) {
        delete[] Ansi;
    }

    if (!Utf8) {
        delete[] Utf8;
    }

    return _Right;
}

inline AString& Utf8_to_AString(AString& _Left, WString& _Right) {
    char    Buffer[4096] = { 0 };
    std::uint32_t Count = 0;

    if (_Right.IsEmpty()) {
        return _Left;
    }

    Count = WideCharToMultiByte(CP_UTF8, 0, _Right.c_str(), Ulong64toUint32(_Right.length()), 0, 0, 0, 0);
    WideCharToMultiByte(CP_UTF8, 0, _Right.c_str(), Ulong64toUint32(_Right.length()), Buffer, Count, 0, 0);
    _Left.Assign(Buffer);
    return _Left;
}

template <std::uint32_t Count = 256 >
class AStackString : public AString {
public:
    explicit AStackString() {
        CheckStack;
        _Construct_empty_();
    }

    explicit AStackString(const char* _Right) {
        CheckStack;
        _Construct_empty_();
        Assign(_Right);
    }

    explicit AStackString(const AString& _Right) {
        CheckStack;
        _Construct_empty_();
        Assign(_Right);
    }

    explicit AStackString(const AString&& _Right) {
        CheckStack;
        _Construct_empty_();
        Assign(_Right);
    }

    explicit AStackString(const std::string& _Right) {
        CheckStack;
        _Construct_empty_();
        Assign(_Right);
    }

    explicit AStackString(const char* _Right, const char* _RightEnd) {
        CheckStack;
        _Construct_empty_();
        Assign(_Right, _RightEnd);
    }

    explicit AStackString(const AStackString& _Right) {
        CheckStack;
        _Construct_empty_();
        this->str = _Right.str;
        this->capacity = _Right.capacity;
        this->intlength = _Right.intlength;
    }

    inline ~AStackString() = default;

    AStackString<Count>& operator = (const char* _Right) {
        Assign(_Right);
        return *this;
    }

    AStackString<Count>& operator = (const AString& _Right) {
        Assign(_Right);
        return *this;
    }

    AStackString<Count>& operator = (const std::string& _Right) {
        Assign(_Right);
        return *this;
    }


    AStackString<Count>& operator = (const size_type Val) {
        if (Val) {
            intlength = Ulong64toUint32(Val);
        }
        return *this;
    }

    AStackString<Count>& operator = (const int Val) {
        if (Val) {
            intlength = Val;
        }
        return *this;
    }

    AStackString<Count>& operator = (const DWORD Val) {
        if (Val) {
            intlength = Val;
        }
        return *this;
    }
private:
    char Buffer[Count] = { 0 };
private:
    void _Construct_empty_() {
        this->str = Buffer;
        this->capacity = Count;
        this->intlength = 0;
    }
};

template <std::uint32_t Count = 256 >
class WStackString : public WString {
public:
    explicit WStackString() {
        CheckStack;
        _Construct_empty_();
    }

    explicit WStackString(const wchar_t* _Right) {
        CheckStack;
        _Construct_empty_();
        Assign(_Right);
    }

    explicit WStackString(const AString& _Right) {
        CheckStack;
        _Construct_empty_();
        Assign(_Right);
    }

    explicit WStackString(const AString&& _Right) {
        CheckStack;
        _Construct_empty_();
        Assign(_Right);
    }

    explicit WStackString(const std::string& _Right) {
        CheckStack;
        _Construct_empty_();
        Assign(_Right);
    }

    explicit WStackString(const wchar_t* _Right, const wchar_t* _RightEnd) {
        CheckStack;
        _Construct_empty_();
        Assign(_Right, _RightEnd);
    }

    explicit WStackString(const WStackString& _Right) {
        CheckStack;
        _Construct_empty_();
        this->str = _Right.str;
        this->capacity = _Right.capacity;
        this->intlength = _Right.intlength;
    }

    inline ~WStackString() = default;

    WStackString<Count>& operator = (const wchar_t* _Right) {
        Assign(_Right);
        return *this;
    }

    WStackString<Count>& operator = (const AString& _Right) {
        Assign(_Right);
        return *this;
    }

    WStackString<Count>& operator = (const std::string& _Right) {
        Assign(_Right);
        return *this;
    }

    WStackString<Count>& operator = (const size_type Val) {
        if (Val) {
            intlength = Ulong64toUint32(Val);
        }
        return *this;
    }

    WStackString<Count>& operator = (const int Val) {
        if (Val) {
            intlength = Val;
        }
        return *this;
    }

    WStackString<Count>& operator = (const DWORD Val) {
        if (Val) {
            intlength = Val;
        }
        return *this;
    }


private:
    wchar_t Buffer[Count] = { 0 };
private:
    void _Construct_empty_() {
        this->str = Buffer;
        this->capacity = Count;
        this->intlength = 0;
    }
};

inline AString& GetModuleName(HMODULE hModule, AString& _Right) {

    if (_Right.Capacity() < 260) {
        _Right.Assign(260);
    }

    _Right = GetModuleFileNameA(hModule, _Right.data(), 260);
    if (!_Right) {
        _Right.Uplength();
    }
    return _Right;
}

inline WString& GetModuleName(HMODULE hModule, WString& _Right) {

    if (_Right.Capacity() < 260) {
        _Right.Assign(260);
    }

    _Right = GetModuleFileNameW(hModule, _Right.data(), 260);
    if (!_Right) {
        _Right.Uplength();
    }
    return _Right;
}

inline HMODULE GetModuleBase(AString& _Right) {

    if (_Right.Capacity() < 1) {
        return 0;
    }

    return GetModuleHandleA(_Right.data());
}

inline HMODULE GetModuleBase(WString& _Right) {

    if (_Right.Capacity() < 1) {
        return 0;
    }

    return GetModuleHandleW(_Right.data());
}

#undef _CONSTEXPR20

#if _HAS_CXX20
#ifndef _CONSTEXPR20
#define _CONSTEXPR20 constexpr
#endif 
#else
#ifndef _CONSTEXPR20
#define _CONSTEXPR20 inline
#endif
#endif

#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)

#endif