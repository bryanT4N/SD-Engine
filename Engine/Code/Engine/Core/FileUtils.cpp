#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <cstdio>


int FileReadToBuffer(std::vector<uint8_t>& outBuffer, const std::string& filename)
{
	FILE* file = nullptr;
	errno_t err = fopen_s(&file, filename.c_str(), "rb");
	if (err != 0 || file == nullptr)
	{
		ERROR_RECOVERABLE(Stringf("FileReadToBuffer: Could not open file \"%s\"", filename.c_str()));
		return 0;
	}

	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	outBuffer.resize(fileSize);
	size_t bytesRead = fread(outBuffer.data(), 1, fileSize, file);
	fclose(file);

	return static_cast<int>(bytesRead);
}


int FileReadToString(std::string& outString, const std::string& filename)
{
	std::vector<uint8_t> buffer;
	int bytesRead = FileReadToBuffer(buffer, filename);
	if (bytesRead == 0)
	{
		return 0;
	}

	buffer.push_back('\0');
	outString = reinterpret_cast<const char*>(buffer.data());
	return bytesRead;
}
