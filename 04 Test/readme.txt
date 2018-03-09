// 初始文件，为了把空目录加到Git项目中

2015-01-23添加
dataTest\createData-bak.sql文件
该文件的作用是在X00设备的postgres管理器（大象）中，模拟增加人员及对应的特征，以供大规模测试。
该文件中创建一个postgresql函数CreateData，执行示例为：
SELECT CreateData(1, 2);
该示例的意义是在数据库中增加一个人，这个人每只眼睛增加2个特征（特征是重复且固定的）