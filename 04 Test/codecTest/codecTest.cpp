// codecTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <iostream>
#include "IKEMB1000Codec.h"

int main(int argc, char* argv[])
{
    std::cout << "codecTest" << std::endl;

    PersonBase personBase[3];
    /*
    uuid_t			Pid;
    int				OpToken;	// ���Ӳ�������, added at 20140429
    // ֵ��X00������ƣ��豸ֻ������ա�����
    char			PersonName[g_nameLen];
    char			PersonSn[g_snLen];
    EnumPersonType  PersonType;
    EnumX00Sex		PersonSex;
    char			IdCardNumber[g_idCardNoLen];
    char			DepartName[g_nameLen];
    char			CardNumber[g_cardNoLen];
    char			Memo[g_memoLen];
    */
    personBase[0].OpToken = 1234;
    strcpy(personBase[0].PersonName, "123456");
    strcpy(personBase[0].PersonSn, "777777");
    personBase[0].PersonType = EnumPersonType::ordinaryPerson;
    personBase[0].PersonSex  = EnumX00Sex::female;
    strcpy(personBase[0].IdCardNumber, "IdCard");
    strcpy(personBase[0].DepartName, "���Բ���");
    strcpy(personBase[0].CardNumber, "�Ž�����001");
    strcpy(personBase[0].Memo, "memo  hehe");
    
    
    strcpy(personBase[1].PersonName, "��������");
    strcpy(personBase[1].PersonSn, "002");

    strcpy(personBase[2].PersonName, "a����+English");
    strcpy(personBase[2].PersonSn, "003");

    char buf[0x20000];
    memset(buf, 0, 0x20000);

    int size = EncodeAddPerson(personBase, 3, buf);


    char buf2[0x20000];
    memset(buf2, 0, 0x20000);
    size = EncodeAddPerson(personBase, 3, buf2);

    char trans[0x20000];
    int  arraySize = 0;
    bool bret = DecodeAddPerson(buf, (PPersonBase)trans, &arraySize);
    
    PPersonBase pTemp = (PPersonBase)trans;

    PPersonBase pTemp0 = pTemp;
    PPersonBase pTemp1 = pTemp+1;
    PPersonBase pTemp2 = pTemp+2;

    memset(buf, 0, 1024);
    NormalAck ack;
    ack.ErrorCode = 0;
    EncodeAckDeletePerson(ack, buf);

	return 0;
}

