#include "AString.hpp"
#include <algorithm>
#include <cstdarg>

AString::AString() {
	this->str = 0;
	this->intlength = 0;
	this->capacity = 0;
}

AString::AString(std::uint32_t reserve) {
	std::uint32_t _size = 0;

	if (reserve) {
		_size = reserve + 1 | MEM_MUST_BE_FREED_FLAG;
		this->str = new char[_size] { 0 };
		this->intlength = 0;
		this->capacity = _size;
	}
	else {
		this->str = 0;
		this->intlength = 0;
		this->capacity = 0;
	}
}

AString::AString(AString& other) {
	this->str = 0;
	this->intlength = 0;
	this->capacity = 0;
	this->Assign(other.str);
}

AString::AString(char* __s) {
	this->str = 0;
	this->intlength = 0;
	this->capacity = 0;
	this->Assign(__s);
}

AString::AString(std::string& __s) {
	this->str = 0;
	this->intlength = 0;
	this->capacity = 0;
	this->Assign(__s.c_str(), __s.c_str() + __s.length());
}

AString::AString(const char* __s) {
	this->str = 0;
	this->intlength = 0;
	this->capacity = 0;
	this->Assign(__s);
}

AString::~AString() throw() {
	if ((this->capacity & MEM_MUST_BE_FREED_FLAG) != 0) {
		if (this->str) {
			delete[]this->str;
		}
	}
}

AString& AString::Assign(char byte) {
	std::uint32_t _size = 1;
	_size += 1 | MEM_MUST_BE_FREED_FLAG;
	if ((this->capacity & RESERVED_MASK) < _size) {
		this->Grow(_size);
	}
	this->str[0] = byte;
	this->intlength = 1;
	return *this;
}

AString& AString::Assign(AString& __s) {
	return this->Assign(__s.str);
}

AString& AString::Assign(const AString& __s) {
	return this->Assign(__s.str);
}

AString& AString::Assign(char* __s) {
	return this->Assign(__s, __s + static_cast<std::uint32_t>(strlen(__s)));
};

AString& AString::Assign(const char* __s) {
	return this->Assign(__s, __s + static_cast<std::uint32_t>(strlen(__s)));
};

AString& AString::Assign(std::string& __s) {
	return this->Assign(__s.c_str(), __s.c_str() + __s.length());
}

AString& AString::Assign(const char* __s, const char* __e) {
	std::uint32_t _size = std::uint32_t(__s - __e) + 1;

	_size += 1 | MEM_MUST_BE_FREED_FLAG;
	if ((this->capacity & RESERVED_MASK) < _size) {
		this->Grow(_size);
	}
	strcat_s(this->str, this->capacity, __s);
	this->intlength = static_cast<std::uint32_t>(strlen(this->str));
	return *this;
}

AString& AString::AssignLower(const char* __s, const char* __e) {
	std::uint32_t _size = std::uint32_t(__s - __e) + 1;

	_size += 1 | MEM_MUST_BE_FREED_FLAG;
	if ((this->capacity & RESERVED_MASK) < _size) {
		this->Grow(_size);
	}
	strcat_s(this->str, this->capacity, __s);
	this->intlength = static_cast<std::uint32_t>(strlen(this->str));
	std::transform(this->begin(), this->end(), this->begin(), ::tolower);
	return *this;
}

AString& AString::AssignUpper(const char* __s, const char* __e) {
	std::uint32_t _size = std::uint32_t(__s - __e) + 1;

	_size += 1 | MEM_MUST_BE_FREED_FLAG;
	if ((this->capacity & RESERVED_MASK) < _size) {
		this->Grow(_size);
	}
	strcat_s(this->str, this->capacity, __s);
	this->intlength = static_cast<std::uint32_t>(strlen(this->str));
	std::transform(this->begin(), this->end(), this->begin(), ::toupper);
	return *this;
}

bool AString::Equals(const char* __s) {
	return !strcmp(this->str, __s);
}

bool AString::Equals(const AString& other) {
	return this->intlength == other.intlength && this->Equals(other.str);
}

bool AString::EqualsI(const char* __s) {
	return !_stricmp(this->str, __s);
}

bool AString::EqualsI(const AString& other) {
	return this->intlength == other.intlength && this->EqualsI(other.str);
}

bool AString::IsEmpty() const {
	return !this->intlength;
}

int AString::Compare(const char* __s) {
	return strcmp(this->str, __s);
}

int AString::CompareI(const char* __s) {
	return _stricmp(this->str, __s);
}

bool AString::EndsWith(char c) const {
	return this->intlength && (this->str[this->intlength - 1] == c);
}

bool AString::EndsWith(const char* __s) const{
	std::uint32_t size = static_cast<std::uint32_t>(strlen(__s));

	const char* Pos = this->str + this->intlength - size;
	if (Pos < this->str) {
		return false;
	}

	while (size > 0)
	{
		if (*Pos == *__s)
		{
			if (*Pos == '\000'){
				return true;
			}
			Pos++;
			__s++;
			size--;
			continue;
		}
		return false;
	}
	return true;
}

bool AString::EndsWith(const AString& other) const{
	std::uint32_t size = other.intlength;

	const char* Pos = this->str + this->intlength - size;
	char* __s = other.str;
	if (Pos < this->str) {
		return false;
	}

	while (size > 0)
	{
		if (*Pos == *__s)
		{
			if (*Pos == '\000') {
				return true;
			}
			Pos++;
			__s++;
			size--;
			continue;
		}
		return false;
	}
	return true;
}

bool AString::EndsWithI(char __s) const{

	std::uint32_t intlength;
	char s;
	char e;

	intlength = this->intlength;
	if (!intlength) {
		return 0;
	}
		
	s = __s;
	if (std::uint8_t(__s - 65) < 0x1Au) {
		s = __s + 32;
	}

	e = this->str[intlength - 1];
	if (std::uint8_t(e - 65) < 0x1Au) {
		e += 32;
	}
		
	return s == e;
}

bool AString::EndsWithI(const char* __s) const{

	size_t otherLen = static_cast<std::uint32_t>(strlen(__s));
	if (otherLen > this->intlength) {
		return false;
	}

	const char* e = this->str + this->intlength - otherLen;

	while (otherLen > 0)
	{
		char s = *e;
		if ((s >= 'A') && (s <= 'Z')){
			s = 'a' + (s - 'A');
		}
		char c = *__s;
		if ((c >= 'A') && (c <= 'Z')){
			c = 'a' + (c - 'A');
		}
		if (s == c)
		{
			if (s == '\000'){
				return true; 
			}

			e++;
			__s++;
			otherLen--;
			continue;
		}
		return (s - c) ? false: true;
	}
	return true;
}

bool AString::EndsWithI(const AString& other) const{
	return this->EndsWithI(other.c_str());
}

bool AString::BeginsWith(char c) const{
	if (this->IsEmpty()) {
		return false;
	}
	return (this->str[0] == c);
}

bool AString::BeginsWith(const char* __s) const{
	const size_t otherLen = static_cast<std::uint32_t>(strlen(__s));
	if (otherLen > this->intlength){
		return false;
	}
	return strcmp(this->str, __s);
}

bool AString::BeginsWith(const AString& other) const{
	return this->BeginsWith(other.c_str());
}

bool AString::BeginsWithI(const char* __s) const{
	return this->EndsWithI(__s);
}

bool AString::BeginsWithI(const AString& other) const{
	return this->BeginsWithI(other.c_str());
}

AString& AString::Format(const char* Format, ...) {
	va_list args;
	va_start(args, Format);
	return this->VFormat(Format, args);
}

AString& AString::VFormat(const char* Format, va_list arg) {
	std::uint32_t _size = 0;

	_size = vsnprintf(0, 0, Format, arg) + 1;
	if (_size) {
		if ((this->capacity & RESERVED_MASK) < _size) {
			this->Grow(_size);
		}
		memset(this->str, 0, _size);
		this->intlength = vsnprintf(this->str, _size, Format, arg);
		va_end(arg);
		return *this;
	}
	else {
		this->str = 0;
		this->capacity = 0;
		this->intlength = 0;
	}
	return *this;
}

void AString::Clear(void) {
	if ((this->capacity & MEM_MUST_BE_FREED_FLAG) != 0) {
		if (this->str) {
			memset(this->str, 0, this->intlength);
			this->intlength = 0;
		}
	}
}

void AString::ClearAndFreeMemory(void) {
	if ((this->capacity & MEM_MUST_BE_FREED_FLAG) != 0) {
		if (this->str) {
			memset(this->str, 0, this->intlength);
			this->intlength = 0;
			delete[]this->str;
			this->str = 0;
			this->capacity = 0;
		}
	}
}

const char* AString::SetLength(std::uint32_t _size) {
	if ((this->capacity & RESERVED_MASK) < _size) {
		this->Grow(_size);
	}
	this->str[_size] = '\0';
	this->intlength = _size;
	return this->str;
}

char* AString::data() noexcept {
	return this->str;
}

const char* AString::c_str() const noexcept {
	return this->str;
}

AString::iterator AString::end() noexcept {
	return (this->str + this->intlength);
}

AString::iterator AString::begin() noexcept {
	return this->str;
}

const AString::iterator AString::begin() const noexcept {
	return this->str;
}

const AString::iterator AString::end() const noexcept {
	return this->str + this->intlength;
}

const AString::iterator AString::c_end() const noexcept {
	return (this->str + this->intlength);
}

std::uint32_t AString::length() const{
	return this->intlength;
}

std::uint32_t AString::size() const{
	return this->capacity;
}

AString& AString::Append(char byte) {
	return this->operator+=(byte);
}

AString& AString::Append(const char* __s) {
	return this->Append(__s, static_cast<std::uint32_t>(strlen(__s)));
}

AString& AString::Append(const AString& other) {
	return this->Append(other.str, other.intlength);
}

AString& AString::Append(const std::string& other) {
	return this->Append(other.data(), static_cast<std::uint32_t>(other.length()));
}

AString& AString::Append(const char* __s, std::uint32_t size) {
	std::uint32_t _size = size + this->intlength;
	if (_size > (this->capacity & RESERVED_MASK)) {
		this->Grow(_size);
	}
	strcat_s(this->str, this->capacity, __s);
	return *this;
}

AString& AString::AppendFormat(const char* Format, ...) {
	char Buffer[0x1000] = { 0 };
	va_list vlArgs;

	va_start(vlArgs, Format);
	vsnprintf_s(Buffer, __crt_countof(Buffer), _TRUNCATE, Format, vlArgs);
	std::vprintf(Format, vlArgs);
	va_end(vlArgs);
	return this->Append(Buffer, static_cast<std::uint32_t>(strlen(Buffer)));
}

AString& AString::operator+= (char byte) {

	if (this->intlength >= (this->capacity & RESERVED_MASK)) {
		this->Grow(intlength + 1);
	}

	this->str[this->intlength] = byte;
	this->intlength += 1;
	this->str[this->intlength] = 0;
	return *this;
}

AString& AString::operator+= (const AString& other) {
	return operator+=(other.str);
}

AString& AString::operator+= (std::string& __s) {
	return operator+=(__s.c_str());
}

AString& AString::operator+= (std::wstring& __s) {
	return this->AppendFormat("%ws", __s.c_str());
}

AString& AString::operator+= (const char* __s) {
	return this->Append(__s, static_cast<std::uint32_t>(strlen(__s)));
}

AString& AString::operator+ (char byte) {
	return this->operator+=(byte);
}

AString& AString::operator+ (const AString& other) {
	return this->operator+=(other);
}

AString& AString::operator+ (std::string& __s) {
	return this->operator+=(__s);
}

AString& AString::operator+ (const char* __s) {
	return this->operator+=(__s);
}

AString& AString::operator= (char byte) {
	return this->Assign(byte);
}

AString& AString::operator= (const AString& other) {
	return this->Assign(other.str);
}

AString& AString::operator= (std::string& __s) {
	return this->Assign(__s.c_str(), __s.c_str() + __s.length());
}

AString& AString::operator= (std::wstring& __s) {
	return this->Format("%ws", __s.c_str());
}

AString& AString::operator= (const char* __s) {
	return this->Assign(__s, __s + static_cast<std::uint32_t>(strlen(__s)));
}

AString& AString::operator= (const void* __ptr) {
	return this->Format("%p", __ptr);
}

AString& AString::operator= (std::uint32_t value) {
	return this->Format("%u", value);
}

bool AString::operator ==(const char* __s) const {

	if (this->IsEmpty()) {
		return false;
	}

	if (this->intlength != static_cast<std::uint32_t>(strlen(__s))) {
		return false;
	}

	return !strcmp(this->str, __s);
}

bool AString::operator ==(const AString& other) const {
	if (this->IsEmpty()) {
		return false;
	}

	if (this->intlength != other.intlength) {
		return false;
	}

	return !strcmp(this->str, other.str);
}

bool AString::operator !=(const AString& other) const {
	return !this->operator ==(other);
}

bool AString::operator !=(const char* __s) const {
	return !this->operator ==(__s);
}

char* AString::operator[] (std::uint32_t index) {
	return &this->str[index];
}

void AString::ToLower() {
	std::transform(this->begin(), this->end(), this->begin(), ::tolower);
}

void AString::ToUpper() {
	std::transform(this->begin(), this->end(), this->begin(), ::toupper);
}

bool AString::find(const char* __s){
	return this->StrstrEx(this->str, __s);
}

char* AString::Find(const char* __s){
	return strstr(this->str,__s);
}

std::uint64_t AString::Hash() {

	if (!this->str || !this->intlength) { return 0; }

	std::uint64_t value{ k_offset_basis };
	do {
		value ^= *this->str++;
		value *= k_prime;
	} while (*(this->str - 1) != '\0');

	return value;
}

void  AString::Grow(std::uint32_t _size) {
	char* Buffer = 0;

	Buffer = new char[_size + 1] {0};
	if (this->str) {
		if ((this->capacity & MEM_MUST_BE_FREED_FLAG) != 0) {
			strcat_s(Buffer, static_cast<rsize_t>(_size) + 1, this->str);
			delete[] this->str;
			this->intlength = static_cast<std::uint32_t>(strlen(Buffer));
		}
	}
	else {
		this->intlength = 0;
	}
	this->str = Buffer;
	this->capacity = _size + 1;
}

void  AString::GrowNoCopy(std::uint32_t _size) {
	if (this->str && (this->capacity & MEM_MUST_BE_FREED_FLAG) != 0) {
		delete[] this->str;
	}

	this->str = new char[_size + 1] {0};
	this->intlength = 0;
	this->capacity = _size + 1 | MEM_MUST_BE_FREED_FLAG;
}

void  AString::SetReserved(std::uint32_t _size) {
	std::uint32_t size = 0;

	size = this->capacity & RESERVED_MASK;
	if (size < _size) {
		this->Grow(_size);
	}

	if (this->str) {
		this->str[_size] = 0;
	}
	this->intlength = _size;
}

bool AString::StrstrEx(const char* Str1, const char* Str2){
	std::string str1_ = Str1;
	std::string str2_ = Str2;
	std::transform(str1_.begin(), str1_.end(), str1_.begin(), ::tolower);
	std::transform(str2_.begin(), str2_.end(), str2_.begin(), ::tolower);
	return std::strstr(str1_.c_str(), str2_.c_str()) != 0;
}
