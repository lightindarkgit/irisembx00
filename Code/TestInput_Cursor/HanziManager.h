#pragma once
#include <vector>
#include <string>
#include <memory>
#include <map>

// ºº×Ö¹ÜÀí
class HanziManage
{
public:
    HanziManage(const std::string &filename);
    ~HanziManage();

    const std::string &pinyin() const
    {
        return m_curPinyin;
    }
    std::vector<std::string> *setPinyin(const std::string &py) const;
    std::vector<std::string> *currentHanziList() const;

private:
    int readFile(std::string filename);

protected:
    static void splitHanzi(const std::string &strHanzi,
                    std::auto_ptr<std::vector<std::string> > &vecHanzi);

protected:
    std::map< std::string, std::string >	m_mapData;

private:
    mutable std::string m_curPinyin;
    mutable std::auto_ptr<std::vector<std::string> > m_vecHanzi;
};
