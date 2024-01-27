#include "AString.hpp"
#include <algorithm>

const char* const AString::s_EmptyString("");

const AString AString::s_EmptyAString;

AString::AString() : str(const_cast<char*>(s_EmptyString)), intlength(0), capacity(0){}

AString::AString(std::uint32_t _capacity){
    char* _sPtr = const_cast<char*>(s_EmptyString);
    if (_capacity) {
        _capacity = Math::RoundUp(_capacity, (std::uint32_t)2);
        _sPtr = new char[_capacity + 1] {0};
    }
    this->str = _sPtr;
    this->intlength = 0;
    SetCapacity(_capacity, true);
}

AString::AString(const AString& string){
    std::uint32_t _length = 0;
    std::uint32_t _capacity = 0;

    _length = string.length();
    this->intlength = _length;
    _capacity = Math::RoundUp(_length, (std::uint32_t)2);
    this->str = new char[_capacity + 1] {0};
    SetCapacity(_capacity, true);
    Copy(string.c_str(), this->str, _length);
}

AString::AString(const std::string& string){
    std::uint32_t _length = 0;
    std::uint32_t _capacity = 0;

    _length = (std::uint32_t)string.length();
    this->intlength = _length;
    _capacity = Math::RoundUp(_length, (std::uint32_t)2);
    this->str = new char[_capacity + 1] {0};
    SetCapacity(_capacity, true);
    Copy(string.c_str(), this->str, _length);
}

AString::AString(const char* string){
    std::uint32_t _length = 0;
    std::uint32_t _capacity = 0;


    _length = (std::uint32_t)StrLen(string);
    this->intlength = _length;
    _capacity = Math::RoundUp(_length, (std::uint32_t)2);
    this->str = new char[_capacity + 1] {0};
    SetCapacity(_capacity, true);
    Copy(string, this->str);
}

AString::AString(const char* start, const char* end){
    std::uint32_t _length = 0;
    std::uint32_t _capacity = 0;

    _length = uint32_t(end - start);
    this->intlength = _length;
    _capacity = Math::RoundUp(_length, (std::uint32_t)2);
    this->str = new char[_capacity + 1] {0};
    SetCapacity(_capacity, true);
    Copy(start, this->str, _length);
}

AString::~AString(){
    ClearAndFreeMemory();
}

std::uint32_t AString::length() const {
    return this->intlength;
}

bool AString::IsEmpty() const {
    return !this->intlength;
}

char* AString::data() {
    return this->str;
}

const char* AString::c_str() const {
    return this->str;
}

const char* AString::c_end() const {
    return (this->str + this->intlength);
}

char* AString::begin() {
    return this->str;
}

const char* AString::c_begin() const {
    return this->str;
}

char* AString::end() {
    return (this->str + this->intlength);
}

const AString& AString::GetEmpty() {
    return s_EmptyAString;
}

void AString::Assign(const char* string){
    Assign(string, string + StrLen(string));
}

void AString::Assign(const char* start, const char* end){
    std::uint32_t _length = 0;

    _length = std::uint32_t(end - start);
    if (_length > get_capacity()){
        GrowNoCopy(_length);
    }
    else if (this->str == s_EmptyString){
        return;
    }
    Copy(start, str, _length);
    this->intlength = _length;
}

void AString::Assign(const AString& string){
    std::uint32_t _length = 0;

    _length = string.length();
    if (_length > get_capacity()) {
        GrowNoCopy(_length);
    }
    else if (str == s_EmptyString){
        return;
    }
    Copy(string.c_str(), this->str, _length);
    this->intlength = _length;
}

void AString::Assign(AString&& string){
    if (string.MustBeFreed() == false){
        Assign(string);
    }
    else{
        if (MustBeFreed()){
            delete[]this->str;
        }
        this->str = string.str;
        this->intlength = string.intlength;
        this->capacity = string.capacity;
    }

    string.str = const_cast<char*>(s_EmptyString);
    string.intlength = 0;
    string.capacity = 0;
}

void AString::clear(){
    if (this->str == this->s_EmptyString){
        return;
    }
    memset(this->str,0, this->intlength);
    this->str = const_cast<char*>(this->s_EmptyString);
    this->intlength = 0;
}

void AString::ClearAndFreeMemory(){
    if (MustBeFreed()){
        memset(this->str, 0, intlength);
        delete[]this->str;
        this->str = const_cast<char*>(s_EmptyString);
        this->intlength = 0;
        this->capacity = 0;
    }
    else{
        if (this->str != const_cast<char*>(s_EmptyString)){
            this->str[0] = '\000';
        }
        this->intlength = 0;
    }
}

void AString::SetReserved(size_t capacity){
    if (capacity < length()){
        SetCapacity((std::uint32_t)capacity);
    }
    Grow((std::uint32_t)capacity);
}

void AString::SetCapacity(std::uint32_t _length){
    if (_length > get_capacity()){
        Grow(_length);
    }

    if (this->str != s_EmptyString){
        this->str[_length] = '\000';
    }
    this->intlength = _length;
}

char& AString::operator [](size_t index) {
    return this->str[index];
}

AString& AString::operator = (const char* string) { 
    Assign(string); 
    return *this;
}

AString& AString::operator = (const AString& string) { 
    Assign(string); 
    return *this; 
}

AString& AString::operator + (char c) {

    if (this->intlength >= get_capacity()) {
        Grow(this->intlength + 1);
    }
    this->str[this->intlength++] = c;
    this->str[this->intlength] = '\000';

    return *this;
}

AString& AString::operator + (std::uint32_t value){
    char Val[255] = { 0 };

    sprintf_s(Val, 255, "%u", value);
    return operator+(Val);
}

AString& AString::operator + (std::uint64_t value){
    char Val[255] = { 0 };

    sprintf_s(Val, 255, "%I64u", value);
    return operator+(Val);
}

AString& AString::operator + (const std::string& string){
    return operator+(string.c_str());
}

AString& AString::operator + (const char* string) {
    std::uint32_t _length = 0;

    _length = (std::uint32_t)StrLen(string);
    if (_length) {
        const std::uint32_t newLen = this->intlength + _length;
        if (newLen > get_capacity()) {
            Grow(newLen);
        }

        Copy(string, str + this->intlength);
        this->intlength += _length;
    }
    return *this;
}

AString& AString::operator + (const AString& string) {
    std::uint32_t _length = 0;
    std::uint32_t new_length = 0;
    _length = string.length();
    if (_length) {
        new_length = this->intlength + _length;
        if (new_length > get_capacity()) {
            Grow(new_length);
        }

        Copy(string.c_str(), this->str + this->intlength, _length);
        this->intlength += _length;
    }
    return *this;
}

AString& AString::operator += (char c){

    if (this->intlength >= get_capacity()){
        Grow(this->intlength + 1);
    }
    this->str[this->intlength++] = c;
    this->str[this->intlength] = '\000';

    return *this;
}

AString& AString::operator += (std::uint32_t value){
    char Val[255] = { 0 };

    sprintf_s(Val, 255, "%u", value);
    return operator+(Val);
}

AString& AString::operator += (std::uint64_t value){
    char Val[255] = { 0 };

    sprintf_s(Val, 255, "%I64u", value);
    return operator+(Val);
}

AString& AString::operator+=(const std::string& string){
    return operator+(string.c_str());
}

AString& AString::operator += (const char* string){
    std::uint32_t _length = 0; 
    
    _length = (std::uint32_t)StrLen(string);
    if (_length){
        const std::uint32_t newLen = this->intlength + _length;
        if (newLen > get_capacity()) {
            Grow(newLen);
        }

        Copy(string, this->str + this->intlength);
        this->intlength += _length;
    }
    return *this;
}

AString& AString::operator += (const AString& string){
    std::uint32_t _length = 0;
    std::uint32_t new_length = 0;
    _length = string.length();
    if (_length){
        new_length = this->intlength + _length;
        if (new_length > get_capacity()) {
            Grow(new_length);
        }

        Copy(string.c_str(), this->str + this->intlength, _length);
        this->intlength += _length;
    }
    return *this;
}

bool AString::operator == (const char* other) const {
    const char* thisPos = str;
    const char* otherPos = other;

loop:
    const char byte = *thisPos;
    const char byte2 = *otherPos;

    if (byte != byte2) {
        return false;
    }
    if (byte == '\000') {
        return true;
    }

    thisPos++;
    otherPos++;
    goto loop;
}

bool AString::operator == (const AString& other) const {
    if (other.length() != length()) {
        return false;
    }
    return (*this == other.c_str());
}

bool AString::operator != (const char* other) const {
    return !(*this == other);
}

bool AString::operator != (const AString& other) const {
    return !(*this == other);
}

bool AString::operator < (const AString& other) const {
    return (Compare(other) < 0);
}

bool AString::operator > (const AString& other) const {
    return (Compare(other) > 0);
}

AString& AString::Append(const AString& string) { 
    return this->operator +=(string);
}

AString& AString::Append(const char* string, std::size_t _length){
    std::uint32_t new_length = 0;
    
    if (_length){
        new_length = this->intlength + (std::uint32_t)_length;
        if (new_length > get_capacity()){
            Grow(new_length);
        }

        Copy(string, str + this->intlength, _length); // handles terminator
        this->intlength = new_length;
    }

    return *this;
}

AString& AString::Append(const char* start, const char* end) {
    return Append(start, static_cast<size_t>(end - start));
}

AString& AString::AppendFormat(_In_z_ _Printf_format_string_ const char* Format, ...){
    AStackString< 0x1000 > buffer;
    va_list args;
    va_start(args, Format);
    buffer.VFormat(Format, args);
    va_end(args);
    Append(buffer);
    return *this;
}

std::int32_t AString::Compare(const AString& other) const {
    return strcmp(this->str, other.c_str());
}

std::int32_t AString::Compare(const char* other) const {
    return strcmp(this->str, other);
}

std::int32_t AString::CompareI(const AString& other) const {
    return _stricmp(this->str, other.c_str());
}

std::int32_t AString::CompareI(const char* other) const {
    return _stricmp(this->str, other);
}

bool AString::Equals(const char* other) const { 
    return (*this == other);
}

bool AString::Equals(const AString& other) const { 
    return (*this == other);
}

bool AString::EqualsI(const char* other) const { 
    return !CompareI(other); 
}

bool AString::EqualsI(const AString& other) const {
    return (CompareI(other) == 0); 
}

bool AString::MustBeFreed() const { 
    return ((this->capacity & MEM_MUST_BE_FREED_FLAG) == MEM_MUST_BE_FREED_FLAG);
}

std::uint64_t AString::Hash(){

    if (!this->str || !this->intlength) {
        return 0; 
    }

    std::uint64_t value{ k_offset_basis };

    for (size_t i = 0; i < this->intlength; i++){
        value ^= this->str[i];
        value *= k_prime;
    }

    return value;
}

std::uint32_t AString::Hashw32(){
    AString T;
    std::uint32_t value = 0x811C9DC5;
    if (!this->str || !this->intlength) { 
        return 0; 
    }

    T = this->str;
    T.ToLower();
    for (size_t i = 0; i < this->intlength; i++) {
        value ^= T.str[i];
        value *= 0x1000193;
    }

    return value;
}

AString& AString::Format(const char* Format,...) {
    va_list _ArgList = 0;

    __crt_va_start(_ArgList, Format);
    VFormat(Format, _ArgList);
    return *this;
}

AString& AString::VFormat(const char* Format, va_list args) {
    std::uint32_t _length = 0;

    _length = _vscprintf(Format, args);
    if (!_length || _length == AString::npos) {
        if (this->str != "") {
            clear();
        }
        return *this;
    }
    else {
        _length += 1;
    }

    if ((this->capacity & RESERVED_MASK) < _length) {
        this->Grow(_length);
    }
    
    memset(this->str, 0, _length);
    this->intlength = vsnprintf(this->str, _length, Format, args);
    return *this;
}

std::int32_t AString::Scan(_Scanf_s_format_string_ const char* _Format, ...) const {
    va_list args;
    va_start(args, _Format);
    const std::int32_t result = vsscanf(this->str, _Format, args);
    va_end(args);
    return result;
}

std::int32_t AString::ScanS(const char* Buffer, _Scanf_s_format_string_ const char* _Format, ...) {
    va_list args;
    va_start(args, _Format);
    const std::int32_t result = vsscanf(Buffer, _Format, args);
    va_end(args);
    return result;
}

std::uint32_t AString::Replace(char from, char to, std::uint32_t maxReplaces){
    std::uint32_t replaceCount = 0;
    char* pos = this->str;
    const char* end = this->str + this->intlength;

    while (pos < end){
        if (*pos == from){
            *pos = to;
            replaceCount++;
            if (replaceCount == maxReplaces) {
                break;
            }
        }
        pos++;
    }
    return replaceCount;
}

std::uint32_t AString::Replace(const char* from, const char* to, std::uint32_t maxReplaces){
    const size_t fromLength = StrLen(from);
    if (!fromLength){
        return 0;
    }

    AStackString<2048> temp;
    std::uint32_t replaceCount = 0;

    const char* pos = this->str;
    const char* end = this->str + this->intlength;
    while (pos <= (end - fromLength)) {
        if (!StrNCmp(pos, from, fromLength)){
            temp += to;
            pos += fromLength;
            maxReplaces--;
            replaceCount++;
            if (!maxReplaces){
                break;
            }
        }
        else{
            temp += *pos;
            pos++;
        }
    }

    if (replaceCount){
        while (pos < end){
            temp += *pos;
            pos++;
        }
        Assign(temp);
    }
    return replaceCount;
}

void AString::ToLower(){
    char* pos = str;
    const char* const end = this->str + this->intlength;
    while (pos < end){
        char c = *pos;
        if ((c >= 'A') && (c <= 'Z')) {
            c = 'a' + (c - 'A');
            *pos = c;
        }
        pos++;
    }
}

void AString::ToUpper(){
    char* pos = str;
    const char* const end = this->str + this->intlength;
    while (pos < end){
        char c = *pos;
        if ((c >= 'a') && (c <= 'z')){
            c = 'A' + (c - 'a');
            *pos = c;
        }
        pos++;
    }
}

void AString::Trim(std::uint32_t startCharsToTrim, std::uint32_t endCharsToTrim){

    if (!startCharsToTrim) {
        if (!endCharsToTrim){
            return;
        }
        SetCapacity(length() - endCharsToTrim);
        return;
    }
    Assign(c_str() + startCharsToTrim, c_end() - endCharsToTrim);
}

void AString::TrimStart(char charToTrimFromStart){
    std::uint32_t nbrCharsToRemoveFromStart = 0;
    const char* pos = this->str;
    const char* end = this->str + this->intlength;

    for (; pos < end && *pos == charToTrimFromStart; ++pos, ++nbrCharsToRemoveFromStart){}

    Trim(nbrCharsToRemoveFromStart, 0);
}

void AString::TrimEnd(char charToTrimFromEnd){
    std::uint32_t nbrCharsToRemoveFromEnd = 0;
    const char* pos = this->str + this->intlength - 1;
    const char* end = this->str;

    for (; pos >= end && *pos == charToTrimFromEnd; --pos, ++nbrCharsToRemoveFromEnd) {}
    Trim(0, nbrCharsToRemoveFromEnd);
}

bool AString::Find(const char* _str){
    if (!this->intlength) {
        return false;
    }
    return this->StrstrEx(this->str, _str);
}

const char* AString::Find(char c, const char* startPos, const char* endPos) const{
    const char* pos = startPos ? startPos : this->str;
    const char* end = endPos ? endPos : this->str + this->intlength;

    while (pos < end){
        if (*pos == c){
            return pos;
        }
        pos++;
    }
    return nullptr;
}

const char* AString::Find(const char* subString, const char* startPos, const char* endPos) const{
    std::size_t _length = 0;

    _length = StrLen(subString);
    const char* pos = startPos ? startPos : this->str;
    const char* end = endPos ? endPos : this->str + intlength;
    end -= _length;

    while (pos <= end) {
        if (!StrNCmp(pos, subString, _length)){
            return pos;
        }
        pos++;
    }
    return nullptr;
}

const char* AString::Find(const AString& subString, const char* startPos, const char* endPos) const{
    std::size_t _length = 0;

    _length = subString.length();
    const char* pos = startPos ? startPos : this->str;
    const char* end = endPos ? endPos : this->str + this->intlength;

    end -= _length;

    while (pos <= end) {
        if (!StrNCmp(pos, subString.c_str(), _length)){
            return pos;
        }
        ++pos;
    }
    return nullptr;
}

const char* AString::FindI(char c, const char* startPos, const char* endPos) const{

    const char* pos = startPos ? startPos : this->str;
    const char* end = endPos ? endPos : this->str + this->intlength;

    char a1 = c;
    if ((a1 >= 'A') && (a1 <= 'Z')){
        a1 = 'a' + (a1 - 'A');
    }

    while (pos < end){
        char b1 = *pos;
        if ((b1 >= 'A') && (b1 <= 'Z')){
            b1 = 'a' + (b1 - 'A');
        }
        if (a1 == b1) {
            return pos;
        }
        ++pos;
    }
    return nullptr;
}

const char* AString::FindI(const char* subString, const char* startPos, const char* endPos) const{
    std::size_t _length = 0;

    _length = StrLen(subString);
    const char* pos = startPos ? startPos : this->str;
    const char* end = endPos ? endPos : this->str + this->intlength;

    end -= _length;


    while (pos <= end){
        if (!StrNCmpI(pos, subString, _length)) {
            return pos;
        }
        pos++;
    }
    return nullptr;
}

const char* AString::FindI(const AString& subString, const char* startPos, const char* endPos) const{
    std::size_t _length = 0;

    _length = subString.length();
    const char* pos = startPos ? startPos : this->str;
    const char* end = endPos ? endPos : this->str + this->intlength;
    end -= _length;


    while (pos <= end){
        if (!StrNCmpI(pos, subString.c_str(), _length)){
            return pos;
        }
        pos++;
    }
    return nullptr;
}

const char* AString::FindLast(char c, const char* startPos, const char* endPos) const{

    const char* pos = startPos ? startPos : (this->str + this->intlength - 1);
    const char* end = endPos ? endPos : this->str;
    while (pos >= end){
        if (*pos == c){
            return pos;
        }
        pos--;
    }
    return nullptr;
}

const char* AString::FindLast(const char* subString, const char* startPos, const char* endPos) const{
    std::size_t _length = 0;
    
    _length = StrLen(subString);

    const char* pos = startPos ? startPos : (this->str + intlength - _length);
    const char* end = endPos ? endPos : this->str;

    while (pos >= end) {
        if (!StrNCmp(pos, subString, _length)){
            return pos;
        }
        pos--;
    }
    return nullptr;
}

const char* AString::FindLast(const AString& subString, const char* startPos, const char* endPos) const{
    std::size_t _length = 0;

    _length = subString.length();
    const char* pos = startPos ? startPos : (this->str + intlength - _length);
    const char* end = endPos ? endPos : this->str;
 
    while (pos >= end){
        if (!StrNCmp(pos, subString.c_str(), _length)) {
            return pos;
        }
        pos--;
    }
    return nullptr;
}

const char* AString::FindLastI(char c, const char* startPos, const char* endPos) const{
    const char* pos = startPos ? startPos : (this->str + this->intlength - 1);
    const char* end = endPos ? endPos : this->str;

    char a1 = c;
    if ((a1 >= 'A') && (a1 <= 'Z')){
        a1 = 'a' + (a1 - 'A');
    }

    while (pos >= end){
        char b1 = *pos;
        if ((b1 >= 'A') && (b1 <= 'Z')){
            b1 = 'a' + (b1 - 'A');
        }
        if (a1 == b1){
            return pos;
        }
        pos--;
    }
    return nullptr;
}

const char* AString::FindLastI(const char* subString, const char* startPos, const char* endPos) const{
    std::size_t _length = 0;

    _length = StrLen(subString);
    const char* pos = startPos ? startPos : (this->str + this->intlength - _length);
    const char* end = endPos ? endPos : this->str;

    while (pos >= end) {
        if (!StrNCmpI(pos, subString, _length)){
            return pos;
        }
        pos--;
    }
    return nullptr;
}

const char* AString::FindLastI(const AString& subString, const char* startPos, const char* endPos) const{
    std::size_t _length = 0;

    _length = subString.length();
    const char* pos = startPos ? startPos : (this->str + this->intlength - _length);
    const char* end = endPos ? endPos : this->str;
 
    while (pos >= end){
        if (!StrNCmpI(pos, subString.c_str(), _length)){
            return pos;
        }
        pos--;
    }
    return nullptr;
}

bool AString::EndsWith(char c) const{
    std::uint32_t _length = 0;

    _length = this->intlength;
    if (!_length){
        return false;
    }
    return (this->str[_length - 1] == c);
}

bool AString::EndsWith(const char* string) const{
   

    std::size_t _length = 0;
    _length = StrLen(string);
    const char* possiblePos = this->str + this->intlength - _length;
    if (possiblePos < this->str){
        return false;
    }
    return !StrNCmp(possiblePos, string, _length);
}

bool AString::EndsWith(const AString& other) const{
    std::size_t _length = 0;

    _length = other.length();
    if (_length > length()){
        return false;
    }
    return !StrNCmp(c_end() - _length, other.c_str(), _length);
}

bool AString::EndsWithI(const char* other) const{
    std::size_t _length = 0;

    _length = StrLen(other);
    if (_length > length()){
        return false;
    }
    return !StrNCmpI(c_end() - _length, other, _length);
}

bool AString::EndsWithI(const AString& other) const{
    std::size_t _length = 0;

    _length = other.length();
    if (_length > length()){
        return false;
    }
    return !StrNCmpI(c_end() - _length, other.c_str(), _length);
}

bool AString::BeginsWith(char c) const{
    if (IsEmpty()){
        return false;
    }
    return (str[0] == c);
}

bool AString::BeginsWith(const char* string) const{
    std::size_t _length = 0;

    _length = StrLen(string);
    if (_length > length()){
        return false;
    }
    return !StrNCmp(str, string, _length);
}

bool AString::BeginsWith(const AString& string) const{
    std::size_t _length = 0;

    _length = string.length();
    if (_length > length()){
        return false;
    }
    return !StrNCmp(str, string.c_str(), _length);
}

bool AString::BeginsWithI(const char* string) const{
    std::size_t _length = 0;

    _length = StrLen(string);
    if (_length > length()){
        return false;
    }
    return !StrNCmpI(str, string, _length);
}

bool AString::BeginsWithI(const AString& string) const{
    std::size_t _length = 0;

    _length = string.length();
    if (_length > length()){
        return false;
    }
    return !StrNCmpI(str, string.c_str(), _length);
}

bool AString::Match(const char* pat, const char* str){
    bool star;

new_segment:
    star = false;
    if (*pat == '*'){
        star = true;
        do { pat++; } while (*pat == '*');
    }

test_match:
    int i;
    for (i = 0; pat[i] && (pat[i] != '*'); i++) {
        const char a = str[i];
        const char b = pat[i];
        if (a != b) {
            if (!str[i]) {
                return false;
            }
            if ((pat[i] == '?') && (str[i] != '.')) {
                continue;
            }
            if (!star) {
                return false;
            }
            str++;
            goto test_match;
        }
    }
    if (pat[i] == '*'){
        str += i;
        pat += i;
        goto new_segment;
    }
    if (!str[i]) {
        return true;
    }
    if (i && pat[i - 1] == '*') {
        return true;
    }
    if (!star) {
        return false;
    }
    str++;
    goto test_match;
}

bool AString::Matches(const char* pattern) const { 
    return Match(pattern, this->str);
}

bool AString::Matches(const AString& pattern) const{
    return Match(pattern.str, this->str);
}

bool AString::MatchI(const char* pat, const char* str){
    bool star;

new_segment:
    star = false;
    if (*pat == '*'){
        star = true;
        do { pat++; } while (*pat == '*');
    }

test_match:
    int i;
    for (i = 0; pat[i] && (pat[i] != '*'); i++){
        char a = str[i]; a = ((a >= 'A') && (a <= 'Z')) ? 'a' + (a - 'A') : a;
        char b = pat[i]; b = ((b >= 'A') && (b <= 'Z')) ? 'a' + (b - 'A') : b;
        if (a != b) {
            if (!str[i]) {
                return false;
            }
            if ((pat[i] == '?') && (str[i] != '.')) continue;
            if (!star) { 
                return false;
            }
            str++;
            goto test_match;
        }
    }
    if (pat[i] == '*')
    {
        str += i;
        pat += i;
        goto new_segment;
    }
    if (!str[i]) {
        return true;
    }
    if (i && pat[i - 1] == '*') {
        return true;
    }
    if (!star) {
        return false;
    }
    str++;
    goto test_match;
}

bool AString::MatchesI(const char* pattern) const { 
    return MatchI(pattern, this->str);
}

bool AString::MatchesI(const AString& pattern) const{
    return MatchI(pattern.str, this->str);
}

bool AString::MidStr(AString left, AString right, AString& Ret){ /*取中间文本*/
    const char* pos = 0;
    const char* end = 0;

    pos = this->Find(left); //查找左边字符串
    end = this->Find(right); //查找右边字符串

    if (!pos || !end) {
        return false;
    }

    pos += left.length(); //移动指针到左边字符串结尾

    Ret.clear();
    Ret.Append(pos, end);
    return true;
}

void AString::Copy(const char* src, char* dst){
    for (;; ) {
        const char c = *src;
        *dst = c;
        if (!c) {
            break;
        }
        ++dst;
        ++src;
    }
}

void AString::Copy(const char* src, char* dst, size_t len){
    memmove(dst, src, len);
    dst[len] = '\000';
}

size_t AString::StrLen(const char* string){
    const char* pos = string;
    while (*pos != '\000'){
        pos++;
    }
    return (size_t)(pos - string);
}

std::int32_t AString::StrNCmp(const char* a, const char* b, size_t num){
    while (num > 0){
        if (*a == *b){
            if (*a == '\000'){
                return 0;
            }
            a++;
            b++;
            num--;
            continue;
        }
        return (*a > *b) ? 1 : -1;
    }
    return 0;
}

std::int32_t AString::StrNCmpI(const char* a, const char* b, size_t num){
    while (num > 0){
        char a1 = *a;
        if ((a1 >= 'A') && (a1 <= 'Z')){
            a1 = 'a' + (a1 - 'A');
        }
        char b1 = *b;
        if ((b1 >= 'A') && (b1 <= 'Z')){
            b1 = 'a' + (b1 - 'A');
        }
        if (a1 == b1){
            if (a1 == '\000'){
                return 0;
            }
            a++;
            b++;
            num--;
            continue;
        }
        return (a1 - b1);
    }
    return 0;
}

bool AString::IsWhitespace(char c) { 
    return ((c == ' ') || (c == '\r') || (c == '\n') || (c == '\t')); 
}

bool AString::IsUppercaseLetter(char c) {
    return ((c >= 'A') && (c <= 'Z')); 
}

bool AString::IsLowercaseLetter(char c) { 
    return ((c >= 'a') && (c <= 'z')); 
}

bool AString::IsLetter(char c) {
    return IsUppercaseLetter(c) || IsLowercaseLetter(c);
}

bool AString::IsNumber(char c) {
    return ((c >= '0') && (c <= '9')); 
}

void  AString::SetCapacity(std::uint32_t reserved, bool mustFreeMemory) {
    this->capacity = (reserved ^ (mustFreeMemory ? MEM_MUST_BE_FREED_FLAG : 0));
}

std::uint32_t AString::get_capacity() const {
    return (this->capacity & RESERVED_MASK);
}

void AString::Grow(std::uint32_t new_length){
    std::uint32_t artd_capacity = 0;
    std::uint32_t _capacity = 0;
    char* Buffer = 0;


    artd_capacity = (get_capacity() * 2);
    _capacity = Math::RoundUp(Math::Max(artd_capacity, new_length), (std::uint32_t)2);
    Buffer = new char[_capacity + 1] {0};


    Copy(this->str, Buffer, this->intlength);
    if (MustBeFreed()){
        delete[] this->str;
    }

    this->str = Buffer;
    SetCapacity(_capacity, true);
}

void AString::GrowNoCopy(std::uint32_t new_length){
    std::uint32_t _capacity = 0;

    if (MustBeFreed()){
        delete[] this->str;
    }

    _capacity = Math::RoundUp(new_length, (std::uint32_t)2);
    this->str = new char[_capacity + 1] {0};
    SetCapacity(_capacity, true);
}

bool AString::StrstrEx(const char* Str1, const char* Str2){

    AString str1_ = Str1;
    AString str2_ = Str2;

    str1_.ToLower();
    str2_.ToLower();
    return std::strstr(str1_.c_str(), str2_.c_str()) != 0;
}

template < std::uint32_t RESERVED >
AStackString< RESERVED >::AStackString(){
    static_assert((RESERVED % 2) == 0, "Capacity must be multiple of 2");
    this->str = this->_str;
    SetCapacity(RESERVED, false);
    this->_str[0] = '\0';
}

template < std::uint32_t RESERVED >
AStackString< RESERVED >::AStackString(const AString& string){
    static_assert((RESERVED % 2) == 0, "Capacity must be multiple of 2");
    this->str = this->_str;
    SetCapacity(RESERVED, false);
    Assign(string);
}

template < std::uint32_t RESERVED >
AStackString< RESERVED >::AStackString(AString&& string): AString(){
    static_assert((RESERVED % 2) == 0, "Capacity must be multiple of 2");
    str = _str;
    SetCapacity(RESERVED, false);
    Assign(static_cast<RemoveReferenceT<decltype(string)>&&>(string));
}

template < std::uint32_t RESERVED >
AStackString< RESERVED >::AStackString(const AStackString& string) : AString(){
    static_assert((RESERVED % 2) == 0, "Capacity must be multiple of 2");
    this->str = this->_str;
    SetCapacity(RESERVED, false);
    Assign(string);
}

template < std::uint32_t RESERVED >
AStackString< RESERVED >::AStackString(AStackString&& string) : AString(){
    static_assert((RESERVED % 2) == 0, "Capacity must be multiple of 2");
    this->str = this->_str;
    SetCapacity(RESERVED, false);
    Assign(static_cast<RemoveReferenceT<decltype(string)>&&>(string));
}

template < std::uint32_t RESERVED >
AStackString< RESERVED >::AStackString(const char* string){
    static_assert((RESERVED % 2) == 0, "Capacity must be multiple of 2");
    this->str = this->_str;
    SetCapacity(RESERVED, false);
    Assign(string);
}

template < std::uint32_t RESERVED >
AStackString< RESERVED >::AStackString(const char* start, const char* end){
    static_assert((RESERVED % 2) == 0, "Capacity must be multiple of 2");
    this->str = this->_str;
    SetCapacity(RESERVED, false);
    Assign(start, end);
}

template < std::uint32_t RESERVED >
AStackString< RESERVED >& AStackString< RESERVED >::operator = (const char* string) {
    Assign(string); 
    return *this; 
}

template < std::uint32_t RESERVED >
AStackString< RESERVED >& AStackString< RESERVED >::operator = (const AString& string) { 
    Assign(string); 
    return *this; 
}

template < std::uint32_t RESERVED >
AStackString< RESERVED >& AStackString< RESERVED >::operator = (AString&& string) {
    Assign(static_cast<RemoveReferenceT<decltype(string)>&&>(string)); 
    return *this;
}

template < std::uint32_t RESERVED >
AStackString< RESERVED >& AStackString< RESERVED >::operator = (const AStackString& string) {
    Assign(string);
    return *this; 
}

template < std::uint32_t RESERVED >
AStackString< RESERVED >& AStackString< RESERVED >::operator = (AStackString&& string) { 
    Assign(static_cast<RemoveReferenceT<decltype(string)>&&>(string)); 
    return *this;
}
