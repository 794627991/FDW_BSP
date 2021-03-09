#ifndef __CJSONTEST_H__
#define __CJSONTEST_H__

/*include*/
#include "api_define_all.h"
#include "bsp_def.h"
#include "cJSON.h"

typedef struct
{
    char MeterID[20];   //δ֪��С �ٸ�
    char Key1[20];      //δ֪��С �ٸ�
    char key2[20];      //δ֪��С �ٸ�
    char AtuoTime1[10]; //δ֪��С �ٸ�
    char AtuoTime2[10]; //δ֪��С �ٸ�
    char AtuoTime3[10]; //δ֪��С �ٸ�
    char SettlDate[20]; //δ֪��С �ٸ�
    char SettlBal[20];  //δ֪��С �ٸ�
    char RechDate[20];  //δ֪��С �ٸ�
    char RechAmt[20];   //δ֪��С �ٸ�
    char ValveCtrl[20]; //δ֪��С �ٸ�
} para_type;

typedef struct
{
    char service_id[20];
    char command_name[20];
    char object_device_id[50];
    para_type para;
} cmddata_type;
extern cmddata_type cmddata;

int cJSON_to_cmddata(char *json_string, cmddata_type *cmddata);

#endif
