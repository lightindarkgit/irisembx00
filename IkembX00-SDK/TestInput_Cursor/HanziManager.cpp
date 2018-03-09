#include <iostream>
#include <fstream>
#include "HanziManager.h"

HanziManage::HanziManage(const std::string &filename)
{
    readFile( filename );
}

HanziManage::~HanziManage()
{

}

int HanziManage::readFile(std::string name)
{
    std::ifstream in( name.c_str() );
    if(!in)
    {
        printf("Can not find %s \n", name.c_str());
        return -1;
    }

    std::string line;
    getline( in, line, '\n' );
    while(!in.eof())
    {
        std::string strLine( line );

        std::string::size_type index = 0;
        std::string::size_type pos = 0;
        if( ( index = strLine.find( ' ',  pos) ) !=  std::string::npos )
        {
            std::string key = strLine.substr( pos, index );
            std::string val = strLine.substr( index + 1 );
            m_mapData.insert( std::pair< std::string , std::string >( key, val ) );
        }
        getline( in, line, '\n' );
    }

    return 0;
}

std::vector<std::string> *HanziManage::currentHanziList() const
{
    return m_vecHanzi.get();
}

void HanziManage::splitHanzi(const std::string &strHanzi, std::auto_ptr<std::vector<std::string> > &vecHanzi)
{
    vecHanzi.reset(new std::vector<std::string>());

    std::string::size_type index = 0;
    std::string::size_type pos = 0;
    while( (index = strHanzi.find( ' ', pos ) ) != std::string::npos )
    {
        std::string hanzi = strHanzi.substr( pos, index - pos );
        vecHanzi->push_back(hanzi);
        pos = index + 1;
    }
}

std::vector<std::string> *HanziManage::setPinyin(const std::string &py) const
{
    m_curPinyin = py;
    //m_vecHanzi.reset((std::vector<std::string> *)NULL);

    std::map<std::string, std::string >::const_iterator iter = m_mapData.find( py );
    if( iter == m_mapData.end() )
    {
        //printf( "not find pinyin: %s \n ", py.c_str());
        return m_vecHanzi.get();
    }

    std::string strHanzi = iter->second;
    //std::cout<< strHanzi.c_str() << "\n";

    splitHanzi(strHanzi, m_vecHanzi);

    return m_vecHanzi.get();
}


