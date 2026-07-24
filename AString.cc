#include "AString.hpp"

const char* const a_EmptyString = "";

const wchar_t* const w_EmptyString = L"";

MemContainer MemAlty;

MemContainer::~MemContainer() noexcept {
    std::lock_guard<std::mutex> lock(Mutex);
    for (auto& [ptr, size] : _Mypair) {
        if (size) {
            std::memset(ptr, 0, size);
        }
        ::operator delete[](ptr);
    }
    _Mypair.clear();
}

void MemContainer::deallocate(std::uint8_t* _Ptr) noexcept {
    if (!_Ptr) {
        return;
    }
    std::lock_guard<std::mutex> lock(Mutex);
    auto it = _Mypair.find(_Ptr);
    if (it != _Mypair.end()) {
        if (it->second) {
            std::memset(_Ptr, 0, it->second);
        }
        ::operator delete[](_Ptr);
        _Mypair.erase(it);
    }
}

_NODISCARD std::size_t MemContainer::size(std::uint8_t* _Ptr) noexcept {
    std::lock_guard<std::mutex> lock(Mutex);
    auto it = _Mypair.find(_Ptr);
    return (it != _Mypair.end()) ? it->second : -1;
}

void swap(AString& _Left, AString& _Right) noexcept {
    _Left.swap(_Right);
}

void swap(WString& _Left, WString& _Right) noexcept {
    _Left.swap(_Right);
}

_NODISCARD int stoi(const AString& _Str, size_t* _Idx, int _Base) {
    int& _Errno_ref = errno;
    const char* _Ptr = _Str.c_str();
    char* _Eptr;
    _Errno_ref = 0;
    const long _Ans = strtol(_Ptr, &_Eptr, _Base);
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

_NODISCARD long stol(const AString& _Str, size_t* _Idx, int _Base) {
    int& _Errno_ref = errno;
    const char* _Ptr = _Str.c_str();
    char* _Eptr;
    _Errno_ref = 0;
    const long _Ans = strtol(_Ptr, &_Eptr, _Base);
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

_NODISCARD unsigned long stoul(const AString& _Str, size_t* _Idx, int _Base) {
    int& _Errno_ref = errno;
    const char* _Ptr = _Str.c_str();
    char* _Eptr;
    _Errno_ref = 0;
    const unsigned long _Ans = strtoul(_Ptr, &_Eptr, _Base);
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

_NODISCARD long long stoll(const AString& _Str, size_t* _Idx, int _Base) {
    int& _Errno_ref = errno;
    const char* _Ptr = _Str.c_str();
    char* _Eptr;
    _Errno_ref = 0;
    const long long _Ans = strtoll(_Ptr, &_Eptr, _Base);
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

_NODISCARD unsigned long long stoull(const AString& _Str, size_t* _Idx, int _Base) {
    int& _Errno_ref = errno;
    const char* _Ptr = _Str.c_str();
    char* _Eptr;
    _Errno_ref = 0;
    const unsigned long long _Ans = strtoull(_Ptr, &_Eptr, _Base);
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

_NODISCARD float stof(const AString& _Str, size_t* _Idx) {
    int& _Errno_ref = errno;
    const char* _Ptr = _Str.c_str();
    char* _Eptr;
    _Errno_ref = 0;
    const float _Ans = strtof(_Ptr, &_Eptr);
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

_NODISCARD double stod(const AString& _Str, size_t* _Idx) {
    int& _Errno_ref = errno;
    const char* _Ptr = _Str.c_str();
    char* _Eptr;
    _Errno_ref = 0;
    const double _Ans = strtod(_Ptr, &_Eptr);
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

_NODISCARD long double stold(const AString& _Str, size_t* _Idx) {
    int& _Errno_ref = errno;
    const char* _Ptr = _Str.c_str();
    char* _Eptr;
    _Errno_ref = 0;
    const long double _Ans = strtold(_Ptr, &_Eptr);
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

_NODISCARD int stoi(const WString& _Str, size_t* _Idx, int _Base) {
    int& _Errno_ref = errno;
    const wchar_t* _Ptr = _Str.c_str();
    wchar_t* _Eptr;
    _Errno_ref = 0;
    const long _Ans = wcstol(_Ptr, &_Eptr, _Base);
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

_NODISCARD long stol(const WString& _Str, size_t* _Idx, int _Base) {
    int& _Errno_ref = errno;
    const wchar_t* _Ptr = _Str.c_str();
    wchar_t* _Eptr;
    _Errno_ref = 0;
    const long _Ans = wcstol(_Ptr, &_Eptr, _Base);
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

_NODISCARD unsigned long stoul(const WString& _Str, size_t* _Idx, int _Base) {
    int& _Errno_ref = errno;
    const wchar_t* _Ptr = _Str.c_str();
    wchar_t* _Eptr;
    _Errno_ref = 0;
    const unsigned long _Ans = wcstoul(_Ptr, &_Eptr, _Base);
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

_NODISCARD long long stoll(const WString& _Str, size_t* _Idx, int _Base) {
    int& _Errno_ref = errno;
    const wchar_t* _Ptr = _Str.c_str();
    wchar_t* _Eptr;
    _Errno_ref = 0;
    const long long _Ans = wcstoll(_Ptr, &_Eptr, _Base);
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

_NODISCARD unsigned long long stoull(const WString& _Str, size_t* _Idx, int _Base) {
    int& _Errno_ref = errno;
    const wchar_t* _Ptr = _Str.c_str();
    wchar_t* _Eptr;
    _Errno_ref = 0;
    const unsigned long long _Ans = wcstoull(_Ptr, &_Eptr, _Base);
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

_NODISCARD float stof(const WString& _Str, size_t* _Idx) {
    int& _Errno_ref = errno;
    const wchar_t* _Ptr = _Str.c_str();
    wchar_t* _Eptr;
    _Errno_ref = 0;
    const float _Ans = wcstof(_Ptr, &_Eptr);
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

_NODISCARD double stod(const WString& _Str, size_t* _Idx) {
    int& _Errno_ref = errno;
    const wchar_t* _Ptr = _Str.c_str();
    wchar_t* _Eptr;
    _Errno_ref = 0;
    const double _Ans = wcstod(_Ptr, &_Eptr);
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

_NODISCARD long double stold(const WString& _Str, size_t* _Idx) {
    int& _Errno_ref = errno;
    const wchar_t* _Ptr = _Str.c_str();
    wchar_t* _Eptr;
    _Errno_ref = 0;
    const long double _Ans = wcstold(_Ptr, &_Eptr);
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

_NODISCARD AString to_AString(int _Val) {
    return _Integral_to_string<AString, int>(_Val);
}

_NODISCARD AString to_AString(unsigned int _Val) {
    return _UIntegral_to_string<AString, unsigned int>(_Val);
}

_NODISCARD AString to_AString(long _Val) {
    return _Integral_to_string<AString, long>(_Val);
}

_NODISCARD AString to_AString(unsigned long _Val) {
    return _UIntegral_to_string<AString, unsigned long>(_Val);
}

_NODISCARD AString to_AString(long long _Val) {
    return _Integral_to_string<AString, long long>(_Val);
}

_NODISCARD AString to_AString(unsigned long long _Val) {
    return _UIntegral_to_string<AString, unsigned long long>(_Val);
}

_NODISCARD AString to_AString(double _Val) {
    const auto _Len = static_cast<size_t>(_scprintf("%f", _Val));
    AString _Str(_Len, '\0');
    sprintf_s(&_Str[0], _Len + 1, "%f", _Val);
    return _Str;
}

_NODISCARD AString to_AString(float _Val) {
    return to_AString(static_cast<double>(_Val));
}

_NODISCARD AString to_AString(long double _Val) {
    return to_AString(static_cast<double>(_Val));
}

_NODISCARD WString to_WString(int _Val) {
    return _Integral_to_string<WString, int>(_Val);
}

_NODISCARD WString to_WString(unsigned int _Val) {
    return _UIntegral_to_string<WString, unsigned int>(_Val);
}

_NODISCARD WString to_WString(long _Val) {
    return _Integral_to_string<WString, long>(_Val);
}

_NODISCARD WString to_WString(unsigned long _Val) {
    return _UIntegral_to_string<WString, unsigned long>(_Val);
}

_NODISCARD WString to_WString(long long _Val) {
    return _Integral_to_string<WString, long long>(_Val);
}

_NODISCARD WString to_WString(unsigned long long _Val) {
    return _UIntegral_to_string<WString, unsigned long long>(_Val);
}

_NODISCARD WString to_WString(double _Val) {
    const auto _Len = static_cast<size_t>(_scwprintf(L"%f", _Val));
    WString _Str(_Len, L'\0');
    swprintf_s(&_Str[0], _Len + 1, L"%f", _Val);
    return _Str;
}

_NODISCARD WString to_WString(float _Val) {
    return to_WString(static_cast<double>(_Val));
}

_NODISCARD WString to_WString(long double _Val) {
    return to_WString(static_cast<double>(_Val));
}

AString& operator<<(AString& _Left, const char* _Right) {
    return _Left.operator+(_Right);
}

AString& operator<<(AString& _Left, const AString& _Right) {
    return _Left.operator+(_Right);
}

AString& operator<<(AString& _Left, int _Val) {
    auto _Str = to_AString(_Val);
    if (_Str.IsEmpty()) return _Left;
    return _Left.operator+(_Str);
}

AString& operator<<(AString& _Left, unsigned int _Val) {
    auto _Str = to_AString(_Val);
    if (_Str.IsEmpty()) return _Left;
    return _Left.operator+(_Str);
}

AString& operator<<(AString& _Left, long _Val) {
    auto _Str = to_AString(_Val);
    if (_Str.IsEmpty()) return _Left;
    return _Left.operator+(_Str);
}

AString& operator<<(AString& _Left, unsigned long _Val) {
    auto _Str = to_AString(_Val);
    if (_Str.IsEmpty()) return _Left;
    return _Left.operator+(_Str);
}

AString& operator<<(AString& _Left, long long _Val) {
    auto _Str = to_AString(_Val);
    if (_Str.IsEmpty()) return _Left;
    return _Left.operator+(_Str);
}

AString& operator<<(AString& _Left, unsigned long long _Val) {
    auto _Str = to_AString(_Val);
    if (_Str.IsEmpty()) return _Left;
    return _Left.operator+(_Str);
}

AString& operator<<(AString& _Left, double _Val) {
    auto _Str = to_AString(_Val);
    if (_Str.IsEmpty()) return _Left;
    return _Left.operator+(_Str);
}

AString& operator<<(AString& _Left, float _Val) {
    auto _Str = to_AString(_Val);
    if (_Str.IsEmpty()) return _Left;
    return _Left.operator+(_Str);
}

AString& operator<<(AString& _Left, long double _Val) {
    auto _Str = to_AString(_Val);
    if (_Str.IsEmpty()) return _Left;
    return _Left.operator+(_Str);
}

WString& operator<<(WString& _Left, const WString& _Right) {
    return _Left.operator+(_Right);
}

WString& operator<<(WString& _Left, const wchar_t* _Right) {
    return _Left.operator+(_Right);
}

WString& operator<<(WString& _Left, int _Val) {
    auto _Str = to_WString(_Val);
    if (_Str.IsEmpty()) return _Left;
    return _Left.operator+(_Str);
}

WString& operator<<(WString& _Left, unsigned int _Val) {
    auto _Str = to_WString(_Val);
    if (_Str.IsEmpty()) return _Left;
    return _Left.operator+(_Str);
}

WString& operator<<(WString& _Left, long _Val) {
    auto _Str = to_WString(_Val);
    if (_Str.IsEmpty()) return _Left;
    return _Left.operator+(_Str);
}

WString& operator<<(WString& _Left, unsigned long _Val) {
    auto _Str = to_WString(_Val);
    if (_Str.IsEmpty()) return _Left;
    return _Left.operator+(_Str);
}

WString& operator<<(WString& _Left, long long _Val) {
    auto _Str = to_WString(_Val);
    if (_Str.IsEmpty()) return _Left;
    return _Left.operator+(_Str);
}

WString& operator<<(WString& _Left, unsigned long long _Val) {
    auto _Str = to_WString(_Val);
    if (_Str.IsEmpty()) return _Left;
    return _Left.operator+(_Str);
}

WString& operator<<(WString& _Left, double _Val) {
    auto _Str = to_WString(_Val);
    if (_Str.IsEmpty()) return _Left;
    return _Left.operator+(_Str);
}

WString& operator<<(WString& _Left, float _Val) {
    auto _Str = to_WString(_Val);
    if (_Str.IsEmpty()) return _Left;
    return _Left.operator+(_Str);
}

WString& operator<<(WString& _Left, long double _Val) {
    auto _Str = to_WString(_Val);
    if (_Str.IsEmpty()) return _Left;
    return _Left.operator+(_Str);
}

AString& operator+(AString& _Left, const wchar_t* const _Right) {
    EncodeCodecvt<char> _convert;
    if (!_convert.ToAnsi(_Right)) {
        return _Left;
    }
    return _Left.operator+(_convert._Myptr());
}

_NODISCARD std::size_t PreNUm(std::uint8_t byte) {
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

_NODISCARD bool AStringIsUTF8(std::uint8_t* Data, std::size_t _Count) {
    std::size_t num = 0;
    std::size_t i = 0;
    while (i < _Count) {
        if ((Data[i] & 0x80) == 0x00) {
            i++;
            continue;
        }
        else if ((num = PreNUm(Data[i])) > 2) {
            if (i + num > _Count) return false;
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

AString& Utf8_to_AString(AString& _Right) {
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
    delete[] Ansi;
    delete[] Utf8;
    return _Right;
}

AString& Utf8_to_AString(AString& _Left, WString& _Right) {
    if (_Right.IsEmpty()) {
        return _Left;
    }
    std::uint32_t Count = WideCharToMultiByte(CP_UTF8, 0, _Right.c_str(), Ulong64toUint32(_Right.length()), 0, 0, 0, 0);
    if (!Count) return _Left;
    Array<char> Buffer(Count + 1, 0);
    WideCharToMultiByte(CP_UTF8, 0, _Right.c_str(), Ulong64toUint32(_Right.length()), Buffer.data(), Count, 0, 0);
    _Left.Assign(Buffer.data());
    return _Left;
}
