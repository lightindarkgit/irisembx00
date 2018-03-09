/*******************************************************************
** 文件名称：IKDatabaseWrapper.h
hldh214
hldh214

安装Ubuntu之后默认英文,选简体中文，正常来说这样装好就应该能用的，可是这个时候又出现了错误，說：broken package database. 请用apt-get install -f解决问题之后再安装。
1/4

如图,左上角红色按钮,单击输入software,选择Ubuntu Software Center
2/4

如图,选择installed,右边搜索thunder,选择列表中的thunderbird,点右边的remove,输入密码之后稍等片刻
3/4

此时再次进入language support,按部就班,应该大家都熟悉啦:)
4/4

老套路了,稍等片刻下载好了就能进去,最后祝大家玩的开心~

** 主 要 类：
** 描    述：自己封装的支持的数据类型
**
** 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
** 创 建 人：yqhe
** 创建日期：2014/2/25 17:03:08
**
** 版    本：0.0.1
** 备    注：命名及代码编写遵守C++编码规范
*******************************************************************/
#pragma once

#include <soci.h>

#ifdef _WIN32
#include <objbase.h>
#else
#include <uuid/uuid.h>
#endif

#include <algorithm>
#include <string.h>
#include <assert.h>

#include <libpq-fe.h>
#include <iomanip>      // 控制格式输出

#include "IKDatabaseLib.h"

/////////////////////////////////////////////

// IkUuid是适应于soci库，用户自定义的一种数据类
// 用于包装uuid，并可保存到数据库中
// 对应的postgresql数据库字段类型可以为CHAR(XX) ,或者uuid都可以

// 使用方法见DataAccessType.h中Person结构里，id的定义及使用
//
// modified at 20140311 by yqhe
// 1、增加比较操作符 ==， !=
// 2、数据库绑定用的功能提取为函数
class IkUuid
{
public:
    // 构造函数
    IkUuid ()
    {

    }

    // 构造函数
    IkUuid(uuid_t inputUuid)
    {
        // 保存传入的uuid
        uuid_copy(_uuid, inputUuid);
    }

    // 设置uuid
    void Set (const uuid_t &inputUuid)
    {
        uuid_copy(_uuid, inputUuid);
    }

    // 获得保存的uuid_t类型的值
    void Get (uuid_t &uuid) const
    {
        uuid_copy(uuid, _uuid);
    }

    // added at 20140311 by yqhe
    // 增加Ikuuid的比较操作符重载
    bool operator==(const IkUuid &givedUuid) const
    {
        uuid_t givedUd;
        givedUuid.Get(givedUd);
        // 内部使用的是memcmp方法
        if (uuid_compare(_uuid, givedUd)==0)
        {
            return true;
        }

        return false;
    }

    // added at 20140311 by yqhe
    // 增加Ikuuid的比较操作符重载
    bool operator!=(const IkUuid &givedUuid) const
    {
        return !(*this==givedUuid);
    }

    // 通过合法的uuid字符串赋值
    IkUuid &operator=(const std::string &strValue)
    {
        uuid_parse(strValue.c_str(), _uuid);
        return *this;
    }

    // 把_uuid转为字符串形式输出
    std::string Unparse() const
    {
        std::string toString;
        char transStr[256];
        uuid_unparse(_uuid, transStr);
        toString = transStr;
        return toString;
    }

private:
    uuid_t      _uuid;

};

// 定义数据库访问方式
namespace soci
{
    template <>
    struct type_conversion<IkUuid>
    {
        // 从数据库读取时是按照std::string类型
        typedef std::string base_type;

        // 从std::string->IkUuid的绑定，从数据库读出的绑定
        static void from_base(base_type const &fromDbString, indicator ind, IkUuid &outUuid)
        {
            if (ind==i_null)
            {
                //  从数据库读的时候出现问题
                throw soci_error("Null value not allowed for IkUuid data type");
            }

            outUuid = fromDbString;

        }

        // 从IkUuid->std::string的绑定，写入数据库的绑定
        static void to_base (const IkUuid &inUuid, base_type &toDbString, indicator &ind)
        {
            toDbString = inUuid.Unparse();
            ind = i_ok;
        }

    };
}
/////////////////////////////////////////////

/////////////////////////////////////////////
const size_t IrisCodeLen = 1024;
const unsigned char apostropheCh = 0x27;    // 单引号 ' 的ASCII值

// 封装虹膜特征数据类型，用于从数据库中读或写虹膜特征
// 可接受最长数据长度为IrisCodeLen（1024）
// 如果设置的数据长度超过IrisCodeLen，则内部保存的数据截取长度IrisCodeLen

// 使用方法见DataAccessType.h中PersonExtent结构里，testData的定义及使用

class IkByteaIrisCode
{
public:
    // 构造函数
    IkByteaIrisCode ():
    _size(0)
    {
    }

    // 拷贝pBytea到私有数组_pBytea，最多拷贝IrisCodeLen(1024)字节
    // 输入参数：
    //      _pBytea —— 需要保存的数据，如虹膜特征
    //      size    —— _pBytea的长度，请注意必须与实际长度保持一致
    //                 此函数不负责验证，只负责使用
    IkByteaIrisCode(const unsigned char* pBytea, size_t size):
    _size(0)
    {
        // 保存传入的byte，_size为拷贝后实际数据大小
        ByteaIn(pBytea, size);
    }

    // 析构
    ~IkByteaIrisCode()
    {
    }

    // 拷贝pBytea到私有数组_pBytea，最多拷贝min(IrisCodeLen, size)字节
    // 输入参数：
    //      pBytea —— 需要保存的数据，如虹膜特征
    //      size   —— _pBytea的长度，请注意必须与实际长度保持一致
    //                 此函数不负责验证，只负责使用
    void Set (const unsigned char* pBytea, size_t size)
    {
        ByteaIn(pBytea, size);
    }

    // 拷贝私有数组_pBytea中的数据到pBytea，最多拷贝min(IrisCodeLen, size)字节
    // 输入参数：
    //      size_t —— pBytea数组的最大长度，请注意必须与实际长度保持一致
    //                 此函数不负责验证，只负责使用
    // 输出参数：
    //      pBytea —— 返回的数据
    // 返回值：
    //      实际拷贝数据长度
    size_t Get (unsigned char* pBytea, size_t size) const
    {
        return ByteOut(pBytea, size);
    }

    // 私有数组_pBytea中的数据是否为空
    bool Empty() const
    {
        if (0==_size)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    // 私有数组_pBytea最大长度
    size_t GetMaxSize() const
    {
        return IrisCodeLen;
    }

    // 私有数组_pBytea中的数据当前长度
    size_t Size() const
    {
        return _size;
    }

    // 生成用于写入数据库的数据，在与数据库绑定时调用
    // 一般情况下不需要使用
    std::string PrepareDbString() const
    {
        return EscapeBlobOwn();
    }

    // 打印私有数组_pBytea中的数据，以16进制的形式
    // 可用于调试输出
    void PrintBytea() const
    {
        size_t printSize = 16;
        if (_size<printSize)
        {
            printSize = _size;
        }

        std::cout << "================================" << std::endl;
        for (size_t count=0; count<printSize; count++)
        {
            if ((count!=0)&&(count%16==0))
            {
                std::cout << std::endl;
            }
            int c = _pBytea[count];
            std::cout << std::hex << std::setw(2) << std::setfill('0') << c << ", ";

        }
        std::cout << "--over!" << std::endl;
        std::cout << "================================" << std::endl;
    }

    // 在与数据库绑定时调用
    // 一般情况下不需要使用
    IkByteaIrisCode &operator=(const std::string &strValue)
    {
//        std::vector<unsigned char> vecUchar = Str2Hex(strValue);
//
//        ByteaIn(vecUchar);
        SetFromDbString(strValue);

        return *this;
    }

    // 根据从数据库中读出的数据设置私有数组_pBytea中的数据
    // 用于解析从数据库中读出的数据，在与数据库绑定时调用，一般情况下不需要使用
    // 输入参数：
    //      strValue —— 从数据库中读出的内容
    void SetFromDbString (const std::string &strValue)
    {
        std::vector<unsigned char> vecUchar = Str2Hex(strValue);
        ByteaIn(vecUchar);
    }

private:
    unsigned char  _pBytea[IrisCodeLen];
    size_t         _size;

private:
    // 计算最大可拷贝数据长度
    size_t CalcCopySize(size_t size) const
    {
        size_t copySize = (size>=IrisCodeLen) ?  IrisCodeLen : size;
        return copySize;
    }

    // 拷贝输入的数据到_pBytea，并根据实际拷贝大小设置_size
    void ByteaIn(const unsigned char* pBytea, size_t size)
    {
        //  取得拷贝数据的可能长度，不能超过IrisCodeLen及size
        _size = CalcCopySize(size);

        std::copy(pBytea, pBytea+_size, _pBytea);
    }

    // 拷贝vector中的值到_pBytea，并根据实际拷贝大小设置_size
    void ByteaIn(const std::vector<unsigned char> vecBytea)
    {
        size_t size = vecBytea.size();
        _size = CalcCopySize(size);

        std::copy(vecBytea.begin(), vecBytea.begin()+_size, _pBytea);
    }

    // 输出_pBytea中的数据
    size_t ByteOut(unsigned char* pBytea, size_t maxSize) const
    {
        //  取得拷贝数据的最小长度
        int copySize = CalcCopySize(maxSize);

        std::copy(_pBytea, _pBytea+copySize, pBytea);

        return copySize;
    }

    inline bool Char2Hex(char c, int &val)
    {
        bool bret = false;
        int  ival = 0;

        if (c>='0' && c<='9')
        {
            ival = (c-'0');
            bret = true;
        }
        else if (c>='a' && c<='f')
        {
            ival = (c-'a' + 10);
            bret = true;
        }

        if (c>='A' && c<='F')
        {
            ival = (c-'A' + 10);
            bret = true;
        }

        val = ival;
        return bret;
    }

    std::vector<unsigned char> Str2Hex (const std::string &hexStr)
    {
        std::vector<unsigned char> retVal;

        bool highPart = ((hexStr.size()%2)==0);

        if (!highPart)
        {
            retVal.push_back(0);
        }

        std::string::const_iterator it;
        for (it=hexStr.begin(); it!=hexStr.end(); ++it)
        {
            int ival = 0;
            if (Char2Hex(*it, ival))
            {
                if (highPart)
                {
                    retVal.push_back(0x10*ival);
                }
                else
                {
                    retVal.back() += ival;
                }

                highPart = !highPart;
            }
        }

        return retVal;
    }

//    std::vector<unsigned char> str2hex (const std::string &hexStr)
//    {
//        std::vector<unsigned char> retVal;
//
//        std::cout << "original string :" << hexStr << std::endl;
//
//        return retVal;
//    }

    // added at 20140310 by yqhe
    // 在测试中发现，当数字中有0x27（单引号）时，转义为两个单引号，
    // 写入后读出为两个0x27 （即 0x27 0x27），导致读出数据有问题
    // 现在的解决办法是遇到这个字符，就不用PQescapeBytea解析了
    // 以后可以尝试其他的解决办法
    std::string EscapeBlobOwn() const
    {
        std::string s = "";

        size_t count;
        for (count=0; count<_size; count++)
        {
            unsigned char ch = _pBytea[count];
            if (apostropheCh!=ch)
            {
                s += EscapeBlob(&ch);
            }
            else
            {
                s += apostropheCh;
            }
        }

        return s;
    }

    // added at 20140310 by yqhe
    // 在测试中发现，当数字中有0x27（单引号）时，转义为两个单引号，
    // 写入后读出为两个0x27 （即 0x27 0x27），导致读出数据有问题
    // 现在的解决办法是遇到这个字符，就不用PQescapeBytea解析了
    // 以后可以尝试其他的解决办法
    // 转义单个字符
    std::string EscapeBlob(unsigned char *ch) const
    {
        std::string s = "";
        size_t escapeByteaLen = 0;
        unsigned char *escapeBytea = PQescapeBytea(ch, 1, &escapeByteaLen);
        s += reinterpret_cast<char *>(escapeBytea);
        size_t s_size = s.size();
        PQfreemem(escapeBytea);
        assert(s_size == escapeByteaLen - 1);

//        std::cout << "Bolb Out : " << s << std::endl;
        return s;
    }

    std::string EscapeBlob() const
    {
        std::string s = "";
        if (_size > 0)
        {
            size_t escapeByteaLen = 0;
            unsigned char *escapeBytea = PQescapeBytea(_pBytea, _size, &escapeByteaLen);
            s += reinterpret_cast<char *>(escapeBytea);
            size_t s_size = s.size();
            PQfreemem(escapeBytea);
            assert(s_size == escapeByteaLen - 1);
        }
        std::stringstream ss;
//        ss << "E'";
//        //if (s.start_with("\\x"))	// 9.2.1 bug: 做逃逸处理时，需要两个反斜杠，但它只给生成了一个
//        ss << "\\";
        ss << s  ;
//        ss << "'::bytea";
        // std::cout << "Bolb Out : " << ss.str() << std::endl;
        return ss.str();
    }

};


// 定义IkByteIrisCode的数据库访问方式
namespace soci
{
    template <>
    struct type_conversion<IkByteaIrisCode>
    {
        // 从数据库读取时是按照std::string类型
        typedef std::string base_type;

        // 从std::string->IkUuid的绑定，从数据库读出的绑定
        static void from_base(base_type const &fromDbString, indicator ind, IkByteaIrisCode &outBytea)
        {
            if (ind==i_null)
            {
                //  从数据库读的时候出现问题
                throw soci_error("Null value not allowed for IkByteaIrisCode data type");
            }

            outBytea.SetFromDbString(fromDbString);
        }

        // 从IkUuid->std::string的绑定，写入数据库的绑定
        static void to_base (const IkByteaIrisCode &inBytea, base_type &toDbString, indicator &ind)
        {
            toDbString = inBytea.PrepareDbString();
            ind = i_ok;
        }

    };
}
// end of IkByteaIrisCode
/////////////////////////////////////////////

/////////////////////////////////////////////
// 封装图像数据类型，用于从数据库中读或写图像
//（数据大小超过1024字节的unsigned char数据）
// 可接受最长数据长度为与使用者提供的内存空间有关
// 如果数据长度超过使用者提供的内存空间长度，则内部保存的数据截取长度

// 注意IkByteaLarge和IkByteaIrisCode的区别：
// 1、IkByteaIrisCode内部保存数据，不需要使用者维护
// 2、IkByteaIrisCode有长度限制，最多1024字节

class IkByteaLarge
{
public:
    // 构造函数
    IkByteaLarge ():
    _pBytea(nullptr), _size(0), _maxSize(0), _ownAlloc(false)
    {

    }

    // 使用者提供内存空间指针
    // 由使用者负责维护此内存空间指针
    // 输入参数：
    //      pBytea —— 使用者提供的内存空间指针
    //      size   —— pBytea的长度，请注意必须与实际长度保持一致
    //                 此函数不负责验证，只负责使用
    IkByteaLarge(unsigned char* pBytea, size_t size):
    _pBytea(nullptr), _size(0), _maxSize(size), _ownAlloc(false)
    {
        // 保存传入的指针，_size为拷贝后实际数据大小
        ByteaIn(pBytea, size);
    }

    // 析构
    ~IkByteaLarge()
    {
    	if (_ownAlloc)
    	{
    		Clear();
    	}
    }

    // 拷贝pBytea指针到私有变量指针_pBytea
    // 输入参数：
    //      pBytea —— 使用者提供的内存空间指针，例如其中保存着图像数据
    //      size   —— _pBytea的长度，请注意必须与实际长度保持一致
    //                 此函数不负责验证，只负责使用
    void Set (unsigned char* pBytea, size_t size)
    {
        ByteaIn(pBytea, size);
    }

    // 获得结构中保存的数据
    // 输出参数：
    //      pBytea —— 返回的数据指针
    //      size   —— 数据指针指向的内存中，实际数据的长度
    // 返回值：
    //      实际数据长度
    size_t Get (unsigned char* pBytea, size_t &size) const
    {
        return ByteOut(pBytea, size);
    }

    // 获得结构中数据指针
    // 输出参数：
    //      size   —— 数据指针指向的内存中，实际数据的长度
    // 返回值：
    //      数据指针
    unsigned char* GetPointe(size_t &size) const
    {
    	size = _size;
    	return _pBytea;
    }

    // 内存指针_pBytea中的数据是否为空
    bool Empty() const
    {
        if ((0==_size)||(nullptr==_pBytea))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    // 内存指针_pBytea指向内存的最大可使用长度
    size_t GetMaxLen() const
    {
        return _maxSize;
    }

    // 内存指针_pBytea指向内存中的数据当前长度
    size_t Size() const
    {
        return _size;
    }

    // 生成用于写入数据库的数据，在与数据库绑定时调用
    // 一般情况下不需要使用
    std::string PrepareDbString() const
    {
        return EscapeBlobOwn();
    }

    // 打印指针_pBytea中的数据，以16进制的形式
    // 可用于调试输出
    void PrintBytea() const
    {
        std::cout << "================================" << std::endl;
        if (_ownAlloc)
        {
            PrintVector(16);
        }
        else
        {
            PrintUserBytea(16);
        }

        std::cout << "--over!" << std::endl;
        std::cout << "================================" << std::endl;
    }

    // 在与数据库绑定时调用
    // 一般情况下不需要使用
    IkByteaLarge &operator=(const std::string &strValue)
    {
//        std::vector<unsigned char> vecUchar = Str2Hex(strValue);
//
//        ByteaIn(vecUchar);
        SetFromDbString(strValue);

        return *this;
    }

    // 根据从数据库中读出的数据设置私有数组_pBytea中的数据
    // 用于解析从数据库中读出的数据，在与数据库绑定时调用，一般情况下不需要使用
    // 输入参数：
    //      strValue —— 从数据库中读出的内容
    void SetFromDbString (const std::string &strValue)
    {
        std::vector<unsigned char> vecUchar = Str2Hex(strValue);
        ByteaIn(vecUchar);
    }

private:
    unsigned char  *_pBytea;
    size_t         _size;
    size_t         _maxSize;
    bool           _ownAlloc;
    std::vector<unsigned char> _vecBak;

private:

    void Clear()
    {
    	_vecBak.clear();
    	_vecBak.resize(0);
    }

    // 显示输出_vecBak中的部分数据
    void PrintVector(size_t printCount) const
    {
        std::vector<unsigned char>::const_iterator vecIt = _vecBak.begin();
        for (size_t count=0; count<printCount; count++)
        {
            if( vecIt!=_vecBak.end())
            {
                int c = *vecIt;
                std::cout << std::hex << c << "| ";
                ++vecIt;
            }
        }
    }

    // 显示输出_pBytea中的部分数据
    void PrintUserBytea(size_t printCount) const
    {
        // std::cout << "pointer=" << static_cast<const void *>(_pBytea) << std::endl;

        size_t printSize = printCount;
        if (_size<printCount)
        {
            printSize = printCount;
        }

        for (size_t count=0; count<printSize; count++)
        {
            int c = _pBytea[count];
            std::cout << std::hex << std::setw(2) << std::setfill('0') << c << ", ";
        }
    }

    // 计算最大可拷贝数据长度
    size_t CalcCopySize(size_t size) const
    {
        size_t copySize = (size>=_maxSize) ? _maxSize : size;
        return copySize;
    }

    // 拷贝输入的指针保留到_pBytea，_size总设置为size
    void ByteaIn(unsigned char* pBytea, size_t size)
    {
        // 保留内存空间的长度
        _maxSize = size;
        _size    = size;

        // 保留指针
        _pBytea  = pBytea;
    }

    // 拷贝vector中的值到_pBytea指向的内存空间，并根据实际拷贝大小设置_size
    void ByteaIn(const std::vector<unsigned char> vecBytea)
    {
        size_t size = vecBytea.size();

        if (!_ownAlloc)
        {
            _ownAlloc = true;
            _maxSize  = size;
        }

        _size = CalcCopySize(size);

        _vecBak.clear();
        _vecBak.resize(vecBytea.size());
        std::copy(vecBytea.begin(), vecBytea.end(), _vecBak.begin());
    }


    // 输出_pBytea指针
    size_t ByteOut(unsigned char* pBytea, size_t &size) const
    {
        //  取得拷贝数据的最小长度
        int copySize = CalcCopySize(size);

        // 提供指针地址
        if (!_ownAlloc)
        {
            pBytea = _pBytea;
        }
        else
        {
            std::copy(_vecBak.begin(), _vecBak.begin()+copySize, pBytea);
        }

        return _size;
    }

    inline bool Char2Hex(char c, int &val)
    {
        bool bret = false;
        int  ival = 0;

        if (c>='0' && c<='9')
        {
            ival = (c-'0');
            bret = true;
        }
        else if (c>='a' && c<='f')
        {
            ival = (c-'a' + 10);
            bret = true;
        }

        if (c>='A' && c<='F')
        {
            ival = (c-'A' + 10);
            bret = true;
        }

        val = ival;
        return bret;
    }

    std::vector<unsigned char> Str2Hex (const std::string &hexStr)
    {
        std::vector<unsigned char> retVal;

        bool highPart = ((hexStr.size()%2)==0);

        if (!highPart)
        {
            retVal.push_back(0);
        }

        std::string::const_iterator it;
        for (it=hexStr.begin(); it!=hexStr.end(); ++it)
        {
            int ival = 0;
            if (Char2Hex(*it, ival))
            {
                if (highPart)
                {
                    retVal.push_back(0x10*ival);
                }
                else
                {
                    retVal.back() += ival;
                }

                highPart = !highPart;
            }
        }

        return retVal;
    }

//    std::vector<unsigned char> str2hex (const std::string &hexStr)
//    {
//        std::vector<unsigned char> retVal;
//
//        std::cout << "original string :" << hexStr << std::endl;
//
//        return retVal;
//    }

    // added at 20140310 by yqhe
    // 在测试中发现，当数字中有0x27（单引号）时，转义为两个单引号，
    // 写入后读出为两个0x27 （即 0x27 0x27），导致读出数据有问题
    // 现在的解决办法是遇到这个字符，就不用PQescapeBytea解析了
    // 以后可以尝试其他的解决办法
    std::string EscapeBlobOwn() const
    {
        std::string s = "";

        size_t count;
        for (count=0; count<_size; count++)
        {
            unsigned char ch = _pBytea[count];
            if (apostropheCh!=ch)
            {
                s += EscapeBlob(&ch);
            }
            else
            {
                s += apostropheCh;
            }
        }

        return s;
    }

    // added at 20140310 by yqhe
    // 在测试中发现，当数字中有0x27（单引号）时，转义为两个单引号，
    // 写入后读出为两个0x27 （即 0x27 0x27），导致读出数据有问题
    // 现在的解决办法是遇到这个字符，就不用PQescapeBytea解析了
    // 以后可以尝试其他的解决办法
    // 转义单个字符
    std::string EscapeBlob(unsigned char *ch) const
    {
        std::string s = "";
        size_t escapeByteaLen = 0;
        unsigned char *escapeBytea = PQescapeBytea(ch, 1, &escapeByteaLen);
        s += reinterpret_cast<char *>(escapeBytea);
        size_t s_size = s.size();
        PQfreemem(escapeBytea);
        assert(s_size == escapeByteaLen - 1);

//        std::cout << "Bolb Out : " << s << std::endl;
        return s;
    }

    std::string EscapeBlob() const
    {
        DatabaseAccess dbAccess;
        std::string s = "";
        if (_size > 0)
        {
            size_t escapeByteaLen = 0;
            unsigned char *escapeBytea = PQescapeByteaConn(dbAccess.GetPsqlBackend(),
                                                       _pBytea, _size, &escapeByteaLen);
            //unsigned char *escapeBytea = PQescapeBytea(_pBytea, _size, &escapeByteaLen);
            s += reinterpret_cast<char *>(escapeBytea);
            size_t s_size = s.size();
            PQfreemem(escapeBytea);
            assert(s_size == escapeByteaLen - 1);
        }
        std::stringstream ss;
//        ss << "E'";
//        //if (s.start_with("\\x"))	// 9.2.1 bug: 做逃逸处理时，需要两个反斜杠，但它只给生成了一个
//        ss << "\\";
        ss << s ;
//        ss << "'::bytea";

        return ss.str();
    }

};


// 定义IkByteaLarge的数据库访问方式
namespace soci
{
    template <>
    struct type_conversion<IkByteaLarge>
    {
        // 从数据库读取时是按照std::string类型
        typedef std::string base_type;

        // 从std::string->IkUuid的绑定，从数据库读出的绑定
        static void from_base(base_type const &fromDbString, indicator ind, IkByteaLarge &outBytea)
        {
            if (ind==i_null)
            {
                //  从数据库读的时候出现问题
                throw soci_error("Null value not allowed for IkByteaLarge data type");
            }

            outBytea.SetFromDbString(fromDbString);
        }

        // 从IkUuid->std::string的绑定，写入数据库的绑定
        static void to_base (const IkByteaLarge &inBytea, base_type &toDbString, indicator &ind)
        {
            toDbString = inBytea.PrepareDbString();
            ind = i_ok;
        }

    };
}

// end of IkByteaLarge
/////////////////////////////////////////////
