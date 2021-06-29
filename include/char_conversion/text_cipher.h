#pragma once

inline std::string text_cipher(std::string Str)
{
	char Key[3] = {'A', 'B', 'C'};
	std::string Encrypted = Str;
	for (unsigned int i = 0; i < Str.size(); i++)
	{
		Encrypted[i] = Str[i] ^ Key[i % (sizeof(Key) / sizeof(char))];
	}
	return Encrypted;
}

inline std::string from_hex(const std::string &in)
{
	std::string output;
	if ((in.length() % 2) != 0)
	{
		return "";
	}
	size_t cnt = in.length() / 2;
	for (size_t i = 0; cnt > i; ++i)
	{
		unsigned long int s = 0;
		std::stringstream ss;
		ss << std::hex << in.substr(i * 2, 2);
		ss >> s;
		output.push_back(static_cast<unsigned char>(s));
	}
	return output;
}

inline std::string to_hex(const std::string &in)
{
	std::stringstream ss;
	for (size_t i = 0; i < in.length(); ++i)
		ss << std::hex << std::setfill('0') << std::setw(2) << (int)in[i];
	return ss.str();
}

inline std::string string_encrypt(std::string str)
{
	return to_hex(text_cipher(str));
}

inline std::string string_decrypt(std::string MyString)
{
	return text_cipher(from_hex(MyString));
}
