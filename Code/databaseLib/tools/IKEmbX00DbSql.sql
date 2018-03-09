/*==============================================================*/
/* DBMS name:      PostgreSQL 8                                 */
/* Created on:     2014/6/11 15:33:46                           */
/*==============================================================*/




/*==============================================================*/
/* Table: Depart                                                */
/*==============================================================*/
create table Depart (
   departID             SERIAL not null,
   departName           VARCHAR(32)          null,
   parentID             INT4                 null,
   memo                 TEXT                 null,
   constraint PK_DEPART primary key (departID)
);

comment on table Depart is
'部门表';

/*==============================================================*/
/* Table: Device                                                */
/*==============================================================*/
create table Device (
   devID                INT4                 not null,
   devName              VARCHAR(32)          null,
   devSn                VARCHAR(16)          null,
   devPlace             VARCHAR(64)          null,
   devIP                VARCHAR(15)          null,
   memo                 TEXT                 null,
   constraint PK_DEVICE primary key (devID)
);

/*==============================================================*/
/* Table: IrisDataBase                                          */
/*==============================================================*/
create table IrisDataBase (
   irisDataID           VARCHAR(38)          not null,
   personID             VARCHAR(38)          null,
   irisCode             bytea                null,
   eyePic               bytea                null,
   matchIrisCode        bytea                null,
   devSn                VARCHAR(32)          null,
   eyeFlag              INT4                 null,
   regTime              TIMESTAMP WITH TIME ZONE null,
   regPalce             VARCHAR(128)         null,
   createTime           TIMESTAMP WITH TIME ZONE null,
   updateTime           TIMESTAMP WITH TIME ZONE null,
   memo                 TEXT                 null,
   pupilRow             INT4                 null,
   pupilCol             INT4                 null,
   pupilRadius          INT4                 null,
   irisRow              INT4                 null,
   irisCol              INT4                 null,
   irisRadius           INT4                 null,
   focusScore           INT4                 null,
   percentVisible       INT4                 null,
   spoofValue           INT4                 null,
   interlaceValue       INT4                 null,
   qualityLevel         INT4                 null,
   qualityScore         INT4                 null,
   constraint PK_IRISDATABASE primary key (irisDataID)
);

comment on table IrisDataBase is
'人员识别记录表，记录人员上下班的识别记录，用于考勤等。';

/*==============================================================*/
/* Table: OperateLog                                            */
/*==============================================================*/
create table OperateLog (
   logID                INT4                 not null,
   logComment           TEXT                 null,
   recTime              TIMESTAMP WITH TIME ZONE null,
   operatorID           INT4                 null,
   constraint PK_OPERATELOG primary key (logID)
);

comment on table OperateLog is
'操作人员的操作日志表';

/*==============================================================*/
/* Table: Operator                                              */
/*==============================================================*/
create table Operator (
   operatorID           INT4                 not null,
   name                 VARCHAR(32)          null,
   password             VARCHAR(16)          null,
   personID             VARCHAR(38)          null,
   sex                  VARCHAR(4)           null,
   memo                 TEXT                 null,
   constraint PK_OPERATOR primary key (operatorID)
);

comment on table Operator is
'insert into Operator (operatorid,name,password) values(1,''admin'',''123456'')';

/*==============================================================*/
/* Table: Person                                                */
/*==============================================================*/
create table Person (
   personID             VARCHAR(38)          not null,
   name                 VARCHAR(32)          null,
   workSN               VARCHAR(64)          null,
   superID              INT4                 null,
   sex                  VARCHAR(4)           null,
   cardID               VARCHAR(16)          null,
   IDcard               VARCHAR(18)          null,
   memo                 TEXT                 null,
   departid             INT4                 null,
   departName           VARCHAR(64)          null,
   disableTime          TIMESTAMP WITH TIME ZONE null,
   hasIrisData          INT4                 null,
   constraint PK_PERSON primary key (personID)
);

comment on table Person is
'人员的列表';

/*==============================================================*/
/* Table: PersonDisableInfo                                     */
/*==============================================================*/
create table PersonDisableInfo (
   disableID            INT4                 not null,
   startTime            TIMESTAMP            null,
   endTime              TIMESTAMP            null,
   personID             VARCHAR(38)          not null,
   constraint PK_PERSONDISABLEINFO primary key (disableID)
);

comment on table PersonDisableInfo is
'人员虹膜信息禁止和启用的数据表';

/*==============================================================*/
/* Table: PersonImage                                           */
/*==============================================================*/
create table PersonImage (
   imageID              VARCHAR(38)          not null,
   personID             VARCHAR(38)          not null,
   personimage          bytea                not null,
   memo                 TEXT                 null,
   constraint PK_PERSONIMAGE primary key (imageID)
);

comment on table PersonImage is
'相关人员的图像';

/*==============================================================*/
/* Table: PersonRecLog                                          */
/*==============================================================*/
create table PersonRecLog (
   recLogID             INT4                 not null,
   personID             VARCHAR(38)          null,
   irisDataID           VARCHAR(38)          null,
   recogTime            TIMESTAMP            null,
   recogType            INT2                 null,
   atPostion            VARCHAR(64)          null,
   devSN                VARCHAR(32)          not null,
   devType              INT2                 null,
   delTime              TIMESTAMP            null,
   memo                 TEXT                 null,
   uploadStatus         INT4                 null,
   constraint PK_PERSONRECLOG primary key (recLogID, devSN)
);

comment on table PersonRecLog is
'记录人员识别记录';

/*==============================================================*/
/* Table: Purview                                               */
/*==============================================================*/
create table Purview (
   purviewID            INT4                 not null,
   operatorID           INT4                 null,
   purview              INT4                 null,
   memo                 TEXT                 null,
   constraint PK_PURVIEW primary key (purviewID)
);

comment on table Purview is
'涉及到相关人员权限的数据';

/*==============================================================*/
/* Table: TempIrisDataBase                                      */
/*==============================================================*/
create table TempIrisDataBase (
   irisDataID           VARCHAR(38)          not null,
   personID             VARCHAR(38)          not null,
   operation            INT2                 not null
);

comment on table TempIrisDataBase is
'临时虹膜表';

/*==============================================================*/
/* Table: TempPerson                                            */
/*==============================================================*/
create table TempPerson (
   personID             VARCHAR(38)          not null,
   operation            INT2                 not null
);

comment on table TempPerson is
'临时人员';

/*==============================================================*/
/* Table: TempPersonImage                                       */
/*==============================================================*/
create table TempPersonImage (
   imageID              VARCHAR(38)          not null,
   personID             VARCHAR(38)          not null,
   operation            INT2                 not null
);

comment on table TempPersonImage is
'临时人员图像';

/*==============================================================*/
/* Table: Token                                                 */
/*==============================================================*/
create table Token (
   personToken          INT4                 not null,
   irisToken            INT4                 null,
   imageToken           INT4                 null
);

comment on table Token is
'令牌';

/*==============================================================*/
/* View: V_IirsDisable                                          */
/*==============================================================*/
create or replace view V_IirsDisable as
select
   Person.personID Person_personID,
   Person.name,
   Person.sex,
   Person.cardID,
   Person.IDcard,
   Person.memo,
   Person.departName,
   Person.disableTime,
   PersonDisableInfo.disableID,
   PersonDisableInfo.startTime,
   PersonDisableInfo.endTime
from
   Person,
   PersonDisableInfo
where
   Person.personID = PersonDisableInfo.personID;

comment on view V_IirsDisable is
'关于人员的虹膜启禁的数据连接';

/*==============================================================*/
/* View: V_IrisData                                             */
/*==============================================================*/
create or replace view V_IrisData as
select
   Person.personID Person_personID,
   Person.name,
   Person.superID,
   Person.sex,
   Person.cardID,
   Person.workSN,
   Person.IDcard,
   Person.departid,
   Person.departName,
   Person.disableTime,
   IrisDataBase.irisDataID,
   IrisDataBase.irisCode,
   IrisDataBase.devSn,
   IrisDataBase.eyeFlag,
   IrisDataBase.regTime,
   IrisDataBase.regPalce
from
   Person,
   IrisDataBase
where
   Person.personID = IrisDataBase.personID;

comment on view V_IrisData is
'人员虹膜数据';

/*==============================================================*/
/* View: V_Operate                                              */
/*==============================================================*/
create or replace view V_Operate as
select
   Operator.operatorID Operator_operatorID,
   Operator.name,
   Operator.sex,
   Operator.memo Operator_memo,
   Purview.purviewID,
   Purview.purview,
   Purview.memo Purview_memo,
   OperateLog.logID,
   OperateLog.logComment,
   OperateLog.recTime 
from
   Operator,
   Purview,
   OperateLog
where
   OperateLog.operatorID = Operator.operatorID and 
   Operator.operatorID = Purview.operatorID;

comment on view V_Operate is
'操作日志记录视图';

/*==============================================================*/
/* View: V_Person                                               */
/*==============================================================*/
create or replace view V_Person as
select
   PersonImage.imageID,
   Person.personID,
   Person.name,
   Person.sex,
   Person.cardID,
   Person.IDcard,
   Person.memo Person_memo,
   Person.departName,
   Person.disableTime,
   PersonImage.personimage,
   PersonImage.memo PersonImage_memo
from
   PersonImage,
   Person
where
   PersonImage.personID = Person.personID;

comment on view V_Person is
'人员的视图';


/*==============================================================*/
/* View: V_Person_Detail                                               */
/*==============================================================*/
create or replace view V_Person_Detail as
SELECT 
    personID,
    name,
    sex,
    cardID,
    IDcard,
    memo,
    departID,
    departName,
    disableTime,
    workSN,
    superID,
    CASE WHEN irisdatacount > 0 THEN '是'  ELSE '否' END AS hasirisdata
FROM
     (SELECT p.*, irisdatacount
      FROM person AS P LEFT JOIN (SELECT personid, CAST(count(*) AS int) irisdatacount FROM irisdatabase GROUP BY personid) AS t
      ON P.personid = t.personid) AS tmp;
comment on view V_Person_Detail is
'人员的详细视图';


/*==============================================================*/
/* View: V_PersonRec                                            */
/*==============================================================*/
create or replace view V_PersonRec as
select
   Person.personID Person_personID,
   Person.name,
   Person.sex,
   Person.cardID,
   Person.IDcard,
   Person.memo Person_memo,
   Person.departName,
   Person.disableTime,
   PersonRecLog.recLogID,
   PersonRecLog.irisDataID,
   PersonRecLog.recogTime,
   PersonRecLog.recogType,
   PersonRecLog.atPostion,
   PersonRecLog.devSN,
   PersonRecLog.devType,
   PersonRecLog.delTime,
   PersonRecLog.memo PersonRecLog_memo
from
   Person,
   PersonRecLog
where
   Person.personID = PersonRecLog.personID;

comment on view V_PersonRec is
'人员识别记录的详细信息';

alter table IrisDataBase
   add constraint FK_IRISDATA_PERSONIRI_PERSON foreign key (personID)
      references Person (personID)
      on delete restrict on update restrict;

alter table PersonDisableInfo
   add constraint FK_PERSONDI_DISABLEPE_PERSON foreign key (personID)
      references Person (personID)
      on delete restrict on update restrict;

alter table PersonImage
   add constraint FK_PERSONIM_FK_PERSON_PERSON foreign key (personID)
      references Person (personID)
      on delete restrict on update restrict;

alter table Purview
   add constraint FK_PURVIEW_PURVIEWOP_OPERATOR foreign key (operatorID)
      references Operator (operatorID)
      on delete restrict on update restrict;

insert into Operator (operatorid,name,password) values(1,'admin','123456');
insert into Token (persontoken,iristoken,imagetoken) values(0,0,0);

