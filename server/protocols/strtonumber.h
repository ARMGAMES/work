// ===========================================================
// Copyright (C) 2019 PYR Software Ltd. All rights reserved.
// ===========================================================

#ifndef protocols_strtonumber_h
#define protocols_strtonumber_h

template<typename T>
bool strToNumber( T& value, const char* str, char** end /* could be nullptr */, int base )
{
	static_assert( std::is_integral<T>::value && !std::is_same<bool, T>::value, "only integral and not boolean types supported" );

	static constexpr bool isSigned = std::is_signed<T>::value;
	typename std::conditional<isSigned, long long, unsigned long long>::type intrVal = 0;
	char* endStr = nullptr;

	// https://pubs.opengroup.org/onlinepubs/9699919799/functions/strtol.html
	// Since 0, {LONG_MIN} or {LLONG_MIN}, and {LONG_MAX} or {LLONG_MAX} are returned on error and are also valid returns
	// on success, an application wishing to check for error situations should set errno to 0, then call strtol() or
	// strtoll(), then check errno.
	errno = 0;

	if( isSigned )
	{
		intrVal = ::strtoll( str, &endStr, base );
	}
	else
	{
		intrVal = ::strtoull( str, &endStr, base );
	}

	static constexpr auto minIntrVal = std::numeric_limits<decltype(intrVal)>::min();
	static constexpr auto maxIntrVal = std::numeric_limits<decltype(intrVal)>::max();

	// https://pubs.opengroup.org/onlinepubs/9699919799/functions/strtol.html
	// If the correct value is outside the range of representable values, {LONG_MIN}, {LONG_MAX}, {LLONG_MIN},
	// or {LLONG_MAX} shall be returned (according to the sign of the value), and errno set to [ERANGE].
	// #NOTE: Use `errno` ASAP.
	const bool inRange = (ERANGE != errno || (minIntrVal != intrVal && maxIntrVal != intrVal));

	if( nullptr != end )
	{
		*end = endStr;
	}

	if( str == endStr )
	{
		value = 0;
		return false;
	}

	static constexpr T minT = std::numeric_limits<T>::min();
	static constexpr T maxT = std::numeric_limits<T>::max();

	if( intrVal < minT )
	{
		value = minT;
		return false;
	}

	if( intrVal > maxT )
	{
		value = maxT;
		return false;
	}

	value = static_cast<T>(intrVal);
	return inRange;
}

#endif // protocols_strtonumber_h
