// ChangeVersionInfo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include <locale>
#include <codecvt>

#define MAX_FILE_LENGTH         2048
const int versionSeg = 4;

std::vector<std::string> splitString(std::string srcStr, std::string delimStr, bool repeatedCharIgnored)
{
    std::vector<std::string> resultStringVector;
    std::replace_if(srcStr.begin(), srcStr.end(), [&](const char& c){if (delimStr.find(c) != std::string::npos){ return true; } else{ return false; }}/*pred*/, delimStr.at(0));//将出现的所有分隔符都替换成为一个相同的字符（分隔符字符串的第一个）
    size_t pos = srcStr.find(delimStr.at(0));
    std::string addedString = "";
    while (pos != std::string::npos) {
        addedString = srcStr.substr(0, pos);
        if (!addedString.empty() || !repeatedCharIgnored) {
            resultStringVector.push_back(addedString);
        }
        srcStr.erase(srcStr.begin(), srcStr.begin() + pos + 1);
        pos = srcStr.find(delimStr.at(0));
    }
    addedString = srcStr;
    if (!addedString.empty() || !repeatedCharIgnored) {
        resultStringVector.push_back(addedString);
    }
    return resultStringVector;
}

std::vector<std::wstring> wsplitString(std::wstring srcStr, std::wstring delimStr, bool repeatedCharIgnored)
{
    std::vector<std::wstring> resultStringVector;
    std::replace_if(srcStr.begin(), srcStr.end(), [&](const wchar_t& c){if (delimStr.find(c) != std::string::npos){ return true; } else{ return false; }}/*pred*/, delimStr.at(0));//将出现的所有分隔符都替换成为一个相同的字符（分隔符字符串的第一个）
    size_t pos = srcStr.find(delimStr.at(0));
    std::wstring addedString = L"";
    while (pos != std::wstring::npos) {
        addedString = srcStr.substr(0, pos);
        if (!addedString.empty() || !repeatedCharIgnored) {
            resultStringVector.push_back(addedString);
        }
        srcStr.erase(srcStr.begin(), srcStr.begin() + pos + 1);
        pos = srcStr.find(delimStr.at(0));
    }
    addedString = srcStr;
    if (!addedString.empty() || !repeatedCharIgnored) {
        resultStringVector.push_back(addedString);
    }
    return resultStringVector;
}



// 需包含locale、string头文件、使用setlocale函数。
std::wstring StringToWstring(const std::string& str)
{// string转wstring
    unsigned len = str.size() * 2;// 预留字节数
    setlocale(LC_CTYPE, "");     //必须调用此函数
    wchar_t *p = new wchar_t[len];// 申请一段内存存放转换后的字符串
    mbstowcs(p, str.c_str(), len);// 转换
    std::wstring str1(p);
    delete[] p;// 释放申请的内存
    return str1;
}

// 读出开头为\xff\xfe的文件，UTF16-Little Endian格式
bool read_utf16_file(const std::string& path, std::vector<std::wstring> &vec_lines)
{
    vec_lines.clear();

    // open as a byte stream
    std::wifstream fin(path, std::ios::binary);

    // apply BOM-sensitive UTF-16 facet
    const unsigned long MaxCode = 0x10ffff;
    const std::codecvt_mode Mode = (std::codecvt_mode)(std::consume_header | std::little_endian);
    std::locale utf16_locale(fin.getloc(), new std::codecvt_utf16<wchar_t, MaxCode, Mode>);
    fin.imbue(utf16_locale);

    // read
    std::wstring line;
    while (getline(fin, line))
    {
        vec_lines.push_back(line);
    }

    fin.close();

    return true;
}

// 读出开头为\xef\xbb\xbf的文件，UTF8格式
bool read_utf8_file(const std::string& path, std::vector<std::wstring> &vec_lines)
{
    vec_lines.clear();

    // open as a byte stream
    std::wifstream fin(path, std::ios::binary);

    // apply BOM-sensitive UTF-16 facet
    const unsigned long MaxCode = 0x10ffff;
    const std::codecvt_mode Mode = (std::codecvt_mode)(std::consume_header | std::little_endian);
    std::locale utf8_locale(fin.getloc(), new std::codecvt_utf8<wchar_t, MaxCode, Mode>);
    fin.imbue(utf8_locale);

    // read
    std::wstring line;
    while (getline(fin, line))
    {
        vec_lines.push_back(line);
    }

    fin.close();

    return true;
}

// 固定读取ServerVersion.h，其代表版本的格式固定
// const std::string X00SRV_VERSION_CODE = "v3.0.0.53";
// 用""分割即可
bool read_version_string(const std::string& path, std::wstring& version)
{
    std::vector<std::wstring> vec_lines;
    bool bret = read_utf8_file(path, vec_lines);
    std::wstring search = L"X00SRV_VERSION_CODE";

    for (auto vecIt = vec_lines.begin(); vecIt != vec_lines.end(); ++vecIt)
    {
        int index = vecIt->find(search, 0);
        if (index != std::wstring::npos)
        {
            std::vector<std::wstring> splitResult = wsplitString(*vecIt, L"\"", true);
            int size = splitResult.size();
            if (size == 3)
            {
                auto splitIt = splitResult.begin();
                splitIt++;
                version = *splitIt;
                return true;
            }

            return false;
        }
    }

    return false;
}

// 例如，将"v3.0.0.53" 转换为" 3,0,0,53"
// 例如，将"v3.0.0.53" 转换为"3.0.0.53"
bool get_format_version(const std::wstring& version, std::wstring& versionComma, std::wstring& versionDot)
{
    std::vector<std::wstring> splitResult = wsplitString(version, L".", true);

    int len = splitResult.size();

    if (len != versionSeg)
    {
        return false;
    }

    std::vector<std::wstring>::iterator vecIt = splitResult.begin();
    *vecIt = (*vecIt).substr(1);

    std::wstringstream ossCaptial;
    std::wstringstream ossString;

    int count = 0;
    for (; vecIt != splitResult.end(); ++vecIt, count++)
    {

        ossCaptial << *vecIt;
        ossString << *vecIt;

        if (count != (versionSeg - 1))
        {
            ossCaptial << L",";
            ossString << L".";
        }
    }

    versionComma = ossCaptial.str();
    versionDot = ossString.str();

    return true;
}

// 形成回车(Carriage Return)即\r的输出
std::wstring creturn()
{
    unsigned short wchar;

    std::wstring wstr = L"";
    wchar = 0x0d;
    wstr += wchar;

    return wstr;
}

// 形成换行(LineFeed)即\n的输出
std::wstring lfeed()
{
    unsigned short wchar;

    std::wstring wstr = L"";
    wchar = 0x0a;
    wstr += wchar;

    return wstr;
}

// 替换版本中以逗号分隔的版本数据
// 格式如下： FILEVERSION 3,0,0,59
bool modify_version_comma(std::wofstream& outfile, 
    const std::wstring& line, const std::wstring& search, const std::wstring& versioncomma)
{
    int index = line.find(search, 0);
    if (index != std::wstring::npos)
    {
        // 发现要找的字符串
        size_t len = search.length();
        std::wstring wnewString = line.substr(0, len + index);
        wnewString += L" ";
        wnewString += versioncomma;
        outfile << wnewString;
        outfile << creturn() << lfeed();

        return true;
    }

    return false;
}

// 替换版本中以点号分隔的版本数据
// 格式如下：            VALUE "FileVersion", "3.0.0.59"
bool modify_version_dot(std::wofstream& outfile,
    const std::wstring& line, const std::wstring& search, const std::wstring& versiondot)
{
    int index = line.find(search, 0);
    if (index != std::wstring::npos)
    {
        // 发现要找的字符串
        size_t len = search.length();
        std::wstring wnewString = line.substr(0, len + index);
        wnewString += L"\", \"";
        wnewString += versiondot;
        wnewString += L"\"";
        outfile << wnewString;
        outfile << creturn() << lfeed();

        return true;
    }

    return false;
}

// 修改资源文件，类似TestX00SrvManager.rc
bool modify_resource_file(const std::string& inpath, 
            const std::string& tmppath, 
            std::wstring& versionComma, 
            std::wstring& versionDot)
{
    std::wstring search0 = L"FILEVERSION";
    std::wstring search1 = L"PRODUCTVERSION";

    std::wstring search2 = L"FileVersion";
    std::wstring search3 = L"ProductVersion";

    std::vector<std::wstring> vec_lines;
    read_utf16_file(inpath, vec_lines);

    std::wofstream outFile(tmppath, std::ios::binary);

    const unsigned long MaxCode = 0x10ffff;
    const std::codecvt_mode Mode = (std::codecvt_mode)(std::generate_header | std::little_endian);
    std::locale utf16_locale(outFile.getloc(), new std::codecvt_utf16<wchar_t, MaxCode, Mode>);
    outFile.imbue(utf16_locale);

    for (auto vecIt = vec_lines.begin(); vecIt != vec_lines.end(); ++vecIt)
    {
        if (modify_version_comma(outFile, *vecIt, search0, versionComma))
        {
            continue;
        }

        if (modify_version_comma(outFile, *vecIt, search1, versionComma))
        {
            continue;
        }

        if (modify_version_dot(outFile, *vecIt, search2, versionDot))
        {
            continue;
        }

        if (modify_version_dot(outFile, *vecIt, search3, versionDot))
        {
            continue;
        }

        outFile << *vecIt;
        outFile << lfeed();
    }

    outFile.clear();
    outFile.close();

    return true;
}


// 替换版本中以点号分隔的版本数据
// 格式如下：[assembly: AssemblyVersion("3.0.0.53")]
bool modify_csharp_version_dot(std::wofstream& outfile,
    const std::wstring& line, const std::wstring& search, const std::wstring& versiondot)
{
    int index = line.find(search, 0);

    // 只查询顶格的字符串，顶格的字符串是示例，不需要修改
    if ((index != std::wstring::npos) && (index==0))
    {
        // 发现要找的字符串
        size_t len = search.length();
        std::wstring wnewString = line.substr(0, len + index);
        wnewString += L"(\"";
        wnewString += versiondot;
        wnewString += L")\"]";
        outfile << wnewString;
        outfile << creturn() << lfeed();

        return true;
    }

    return false;
}

// 修改C#资源文件，类似AssemblyInfo.cs
//
bool modify_csharp_cs_file(const std::string& inpath,
    const std::string& tmppath,
    std::wstring& versionComma,
    std::wstring& versionDot)
{
    std::wstring search0 = L"[assembly: AssemblyVersion";
    std::wstring search1 = L"[assembly: AssemblyFileVersion";

    std::vector<std::wstring> vec_lines;
    read_utf8_file(inpath, vec_lines);

    std::wofstream outFile(tmppath, std::ios::binary);

    const unsigned long MaxCode = 0x10ffff;
    const std::codecvt_mode Mode = (std::codecvt_mode)(std::generate_header | std::little_endian);
    std::locale utf8_locale(outFile.getloc(), new std::codecvt_utf8<wchar_t, MaxCode, Mode>);
    outFile.imbue(utf8_locale);

    for (auto vecIt = vec_lines.begin(); vecIt != vec_lines.end(); ++vecIt)
    {
        if (modify_csharp_version_dot(outFile, *vecIt, search0, versionDot))
        {
            continue;
        }

        if (modify_csharp_version_dot(outFile, *vecIt, search1, versionDot))
        {
            continue;
        }

        outFile << *vecIt;
        outFile << lfeed();
    }

    outFile.clear();
    outFile.close();

    return true;
}

std::string get_code_path()
{
    char codeFile[MAX_FILE_LENGTH];
    strcpy(codeFile, __FILE__);

    strrchr(codeFile, '\\')[1] = 0;
    std::cout << codeFile << std::endl;

    std::stringstream oss;
    oss << codeFile << "..\\..\\";

    return (oss.str());
}

int _tmain(int argc, _TCHAR* argv[])
{
    std::string codeFilePath = get_code_path();
    std::string versionFileName = codeFilePath + "X00SrvManager\\include\\" + "ServerVersion.h";

    std::string inFileName = codeFilePath + "TestX00SrvManager\\" + "TestX00SrvManager.rc";
    std::string outFileName = codeFilePath + "TestX00SrvManager\\" + "TestX00SrvManager.rc1";

    std::string incsFileName = codeFilePath + "fmMain\\\Properties\\" + "AssemblyInfo.cs";
    std::string outcsFileName = codeFilePath + "fmMain\\\Properties\\" + "AssemblyInfo.cs1";

    
    // 从设置版本号的文件(ServerVersion.h)中获得当前版本号
    std::wstring wversion;
    read_version_string(versionFileName, wversion);

    // 根据版本号，获得以逗号分隔的版本和点号分割的版本
    // 因为文件中不同位置对应的要求不一样
    std::wstring wversionComma;
    std::wstring wversionDot;
    get_format_version(wversion, wversionComma, wversionDot);

    // 修改c++文件中的rc文件
    modify_resource_file(inFileName, outFileName, wversionComma, wversionDot);

    // 修改c#文件中的cs文件
    modify_csharp_cs_file(incsFileName, outcsFileName, wversionComma, wversionDot);

    return 0;
}


