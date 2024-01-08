#ifndef _AString_
#define _AString_

#include <string>
#include <cstdint>

class AString {
public:
	AString();
	AString(char* __s);
	AString(std::string& __s);
	AString(const char* __s);
	AString(std::uint32_t reserve);
	AString(AString& other);
	~AString() throw();

typedef char* iterator;

	AString& Assign(char byte);
	AString& Assign(AString& __s);
	AString& Assign(const AString& __s);
	AString& Assign(char* __s);
	AString& Assign(const char* __s);
	AString& Assign(std::string& __s);
	AString& Assign(const char* __s, const char* __e);
	AString& AssignLower(const char* __s, const char* __e);
	AString& AssignUpper(const char* __s, const char* __e);


	bool Equals(const char* __s);
	bool Equals(const AString& other);
	bool EqualsI(const char* __s);
	bool EqualsI(const AString& other);
	bool IsEmpty() const;
	int  Compare(const char* __s);
	int  CompareI(const char* __s);
	bool EndsWith(char c) const;
	bool EndsWith(const char* __s) const;
	bool EndsWith(const AString& other) const;
	bool EndsWithI(char __s) const;
	bool EndsWithI(const char* __s) const;
	bool EndsWithI(const AString& other) const;
	bool BeginsWith(char c) const;
	bool BeginsWith(const char* __s) const;
	bool BeginsWith(const AString& other) const;
	bool BeginsWithI(const char* __s) const;
	bool BeginsWithI(const AString& other) const;


	AString& Format(const char* Format, ...);
	AString& VFormat(const char* Format, va_list arg);

	void     Clear(void);
	void     ClearAndFreeMemory(void);

	const char* SetLength(std::uint32_t _size);

	// C-style compatibility
	char* data() noexcept;
	const char* c_str() const noexcept;
	iterator       end() noexcept;
	iterator       begin() noexcept;
	const iterator begin() const noexcept;
	const iterator end() const noexcept;
	const iterator c_end() const noexcept;
	std::uint32_t length() const;
	std::uint32_t size() const;


	// append
	AString& Append(char byte);
	AString& Append(const char* __s);
	AString& Append(const AString& other);
	AString& Append(const std::string& other);
	AString& Append(const char* __s, std::uint32_t size);
	AString& AppendFormat(const char* Format, ...);


	// operator override
	AString& operator+=(char byte);
	AString& operator+=(const AString& other);
	AString& operator+=(std::string& __s);
	AString& operator+=(std::wstring& __s);
	AString& operator+=(const char* __s);
	AString& operator+ (char byte);
	AString& operator+ (const AString& other);
	AString& operator+ (std::string& __s);
	AString& operator+ (const char* __s);
	AString& operator= (char byte);
	AString& operator= (const AString& other);
	AString& operator= (std::string& __s);
	AString& operator= (std::wstring& __s);
	AString& operator= (const char* __s);
	AString& operator= (const void* __ptr);
	AString& operator= (std::uint32_t value);
	bool     operator==(const char* __s) const;
	bool     operator==(const AString& other) const;
	bool     operator!=(const AString& other) const;
	bool     operator!=(const char* __s) const;
	char*    operator[](std::uint32_t index);

	void ToLower();
	void ToUpper();

	bool  find(const char* __s);
	char* Find(const char* __s);

	std::uint64_t Hash();
private:
	void  Grow(std::uint32_t _size);
	void  GrowNoCopy(std::uint32_t _size);
	void  SetReserved(std::uint32_t _size);
	bool  StrstrEx(const char* Str1, const char* Str2);
protected:
	enum : std::uint32_t { MEM_MUST_BE_FREED_FLAG = 0x00000001 };
	enum : std::uint32_t { RESERVED_MASK = 0xFFFFFFFE };
	enum : std::uint64_t { k_offset_basis = 0xcbf29ce484222325ull };
	enum : std::uint64_t { k_prime = 1099511628211ull };

	char* str;
	std::uint32_t intlength;
	std::uint32_t capacity;
};





#endif
