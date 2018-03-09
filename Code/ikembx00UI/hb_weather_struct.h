#ifndef HB_WEATHER_STRUCT_H
#define HB_WEATHER_STRUCT_H


typedef struct{
    QString city;
    QString cityid;
    QString temp1;
    QString temp2;
    QString weather1;   /// type
    QString weather2;
    QString wind1;      /// feixiang
    QString wind2;      /// fengli
    QString index;
    QString index_d;
    QString index48;
    QString index48_d;
    QString index_xc;   //洗车指数
    QString index_ls;   //晾晒
    QString index_ag;   //过敏
    QString index_tr;   //旅游
    QString index_uv;   //紫外线
    QString date_y;     //日期
    QString week;       /// date 星期
    QString pic;        // 气象图片

}weatherdata;





#endif // HB_WEATHER_STRUCT_H
