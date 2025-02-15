#include "AsciiConvert.h"
#include <map>
#include <string>
#include <algorithm>
#include <mutex>
typedef unsigned char A_HEX;
#define ERROR_HEX 0xff
std::mutex g_Mutex;
AsciiConvert* AsciiConvert::m_asciiThis = NULL;

map<string, A_HEX> g_MapHideAsciiList = {
	{"NUL", 0x00}, {"SOH", 0x01}, {"STX",0x02}, {"ETX",0x03},
	{"EOT", 0x04}, {"ENQ", 0x05}, {"ACK",0x06}, {"BEL",0x07},
	{"BS",  0x08}, {"HT",  0x09}, {"LF", 0x0A}, {"VT", 0x0B},
	{"FF",  0x0C}, {"CR",  0x0D}, {"SO", 0x0E}, {"SI", 0x0F},
	{"DLE", 0x10}, {"DC1", 0x11}, {"DC2",0x12}, {"DC3",0x13},
	{"DC4", 0x14}, {"NAK", 0x15}, {"SYN",0x16}, {"ETB",0x17},
	{"CAN", 0x18}, {"EM",  0x19}, {"SUB",0x1A}, {"ESC",0x1B},
	{"FS",  0x1C },{"GS",  0x1D}, {"RS", 0x1E}, {"US", 0x1F},
	{"DEL", 0x7F},

};
AsciiConvert::AsciiConvert()
{

}
AsciiConvert::~AsciiConvert()
{

}

unsigned char AsciiConvert::fromAsciiToHex(string ascii)
{
	if (ascii.length() == 1)
		return ascii[0];
	transform(ascii.begin(), ascii.end(), ascii.begin(), ::toupper);
	map<string, A_HEX>::iterator iter = g_MapHideAsciiList.find(ascii);
	if (iter != g_MapHideAsciiList.end())
		return iter->second;
	else 
		return ERROR_HEX;
}


bool AsciiConvert::fromAsciiGroupToHex(string ascii, vector<unsigned char>& vcHexOut)
{
	if (ascii == "")
		return false;
	vcHexOut.clear();
	vector<string> vcAscii;
	if (ascii.find(",") != string::npos) {
		vcAscii = split(ascii, ",");
	}
	if (ascii.find(" ") != string::npos) {
		vcAscii = split(ascii, " ");
	}
	for (int i = 0; i < vcAscii.size(); i++) {
		unsigned char hexOne = fromAsciiToHex(vcAscii[i]);
		if (hexOne == 0xff) {
			return false;
		}
		else {
			vcHexOut.push_back(hexOne);
		}
	}
	return true;
}

string AsciiConvert::fromAsciiGroupToHexStr(string ascii, bool if0xMod)
{
	vector<unsigned char> vcHexOut;
	if (!fromAsciiGroupToHex(ascii, vcHexOut))
		return "";
	int vcHexSize = vcHexOut.size();
	unsigned char* uchHex = new unsigned char[vcHexSize];
	for (int i = 0; i < vcHexSize; i++) {
		uchHex[i] = vcHexOut[i];
	}
	return charToHexString(uchHex, vcHexSize, if0xMod);
}

AsciiConvert*& AsciiConvert::GetAsciiConvHandle(void)
{
	if (m_asciiThis == nullptr) {
		g_Mutex.try_lock();

		if (m_asciiThis == nullptr) {
			m_asciiThis = new AsciiConvert();
		}
		g_Mutex.unlock();

	}
	return m_asciiThis;
}

vector<string> AsciiConvert::split(const string& str, const string& pattern)
{
	vector<string> res;
	if (str == "")
		return res;

	string strs = str + pattern;
	size_t pos = strs.find(pattern);

	while (pos != strs.npos) {
		string temp = strs.substr(0, pos);
		res.push_back(temp);
		strs = strs.substr(pos + 1, strs.size());
		pos = strs.find(pattern);
	}
	return res;
}


std::string AsciiConvert::charToHexString(unsigned char* bytes, int len, bool if0xMode)
{
	const char HEX[] = "0123456789ABCDEF";
	std::string lStr;
	if (if0xMode)
		lStr = "0x";
	for (int i = 0; i < len; i++)
	{
		char lTemp = bytes[i];
		lStr.push_back(HEX[(lTemp & 0xf0) >> 4]);
		lStr.push_back(HEX[lTemp & 0x0f]);
		if (i < len - 1)
		{
			if (if0xMode) {
				lStr += " 0x";
			}
			else {
				lStr += " ";
			}
		}
	}
	return lStr;
}