#pragma once
#include <iostream>
#include <vector>
using namespace std;
class AsciiConvert {
public:
	AsciiConvert();
	~AsciiConvert();
	// form 'M' to 77 or 4D
	unsigned char fromAsciiToHex(string ascii);

	// from "M S CR LF" to  <77,83,13,10>  or
	// from "M,S,CR,LF" to  <77,83,13,10> 
	bool fromAsciiGroupToHex(string ascii, vector<unsigned char> &hexOut);
	
	// from "M,S,CR,LF" to  "4D 53 0D 0A" or 
	// if if0xMod = true, from "M,S,CR,LF" to  "0x4D 0x53 0x0D 0x0A" 
	string fromAsciiGroupToHexStr(string ascii, bool if0xMod = false);
	
	static AsciiConvert* &GetAsciiConvHandle(void);
private:
	static AsciiConvert* m_asciiThis;
	vector<string> split(const string& str, const string& pattern);
	std::string charToHexString(unsigned char* bytes, int len, bool if0xMode);
};


