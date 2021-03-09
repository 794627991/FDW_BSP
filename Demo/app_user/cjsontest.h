#ifndef __CJSONTEST_H__
#define __CJSONTEST_H__

/*include*/
#include "api_define_all.h"
#include "bsp_def.h"
#include "cJSON.h"

typedef struct
{
    char MeterID[20];   //未知大小 再改
    char Key1[20];      //未知大小 再改
    char key2[20];      //未知大小 再改
    char AtuoTime1[10]; //未知大小 再改
    char AtuoTime2[10]; //未知大小 再改
    char AtuoTime3[10]; //未知大小 再改
    char SettlDate[20]; //未知大小 再改
    char SettlBal[20];  //未知大小 再改
    char RechDate[20];  //未知大小 再改
    char RechAmt[20];   //未知大小 再改
    char ValveCtrl[20]; //未知大小 再改
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
