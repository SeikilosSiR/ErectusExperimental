#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "utils.h"
#include <algorithm>
#include <codecvt>

void Utils::ValidateRgb(float* value)
{
	for (auto i = 0; i < 3; i++)
	{
		value[i] = std::clamp(value[i], 0.0f, 1.0f);
	}
}

bool Utils::Valid(const std::uintptr_t ptr)
{
	if (ptr < 0x10000UL || ptr > 0xF000000000000ULL || ptr & 1)
		return false;

	return true;
}

Vector3 Utils::ProjectView(const Vector3& forward, const Vector3& origin, const float distance)
{
	return origin + forward * distance;
}

float Utils::RadiansToDegrees(const float radians)
{
	return radians * (180.0f / 3.14159265358979323846f);
}

float Utils::GetDegrees(const Vector3& src, const Vector3& forward, const Vector3& origin)
{
	const auto buffer = ProjectView(forward, origin, src.DistanceTo(origin));
	return RadiansToDegrees(sin(src.DistanceTo(buffer) / origin.DistanceTo(buffer)));
}

int Utils::GetRangedInt(const int min, const int max)
{
	if (min < max)
		return rand() % (1 + (max - min)) + (max - (max - min));

	return min;
}

// Random string generator to be used in the process name
std::string Utils::RandomString(std::size_t length)
{
	const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	std::random_device random_device;
	std::mt19937 generator(random_device());
	std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

	std::string random_string = "";

	for (std::size_t i = 0; i < length; ++i)
	{
		random_string += CHARACTERS[distribution(generator)];
	}

	return random_string;
}

std::string Utils::UTF8ToGBK(const char* strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}

std::string Utils::strtowstr(const std::string& str)
{
	// Convert an ASCII string to a Unicode String
	std::wstring wstrTo;
	wchar_t* wszTo = new wchar_t[str.length() + 1];
	wszTo[str.size()] = L'\0';
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wszTo, (int)str.length());
	wstrTo = wszTo;
	delete[] wszTo;
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.to_bytes(wstrTo);
}