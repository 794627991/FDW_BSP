/*
*********************************************************************************************************
*
*	模块名称 : cjson测试
*	文件名称 : cjsontest.c
*	版    本 : V1.0.0
*	说    明 : 自行参悟
*	修改记录 :
*
*		版本号     日期      作者    说明
*		V1.0.0   2021.3.4   高屹     无
*
*	Copyright (C), 2020-2030,  辽宁思凯-高屹
*
*********************************************************************************************************
*/
#include "cjsontest.h"

cmddata_type cmddata;

char *Get_service_id(const char *string)
{
    char s[200];
    char *p = 0;
    int i;
    memset(s, 0, sizeof(s));
    p = strstr("\"service_id\"", string);
    for (i = 0; i < sizeof(s); i++)
    {
        if (p[i] == ':')
        {
            if (p > 0)
            {
                sscanf(p + 1, "\"%s\"", s);
            }
        }
    }
    p = s;
    return p;
}

int cJSON_to_cmddata(char *json_string, cmddata_type *cmddata)
{
    cJSON *root = cJSON_Parse(json_string);

    if (!root)
    {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        return -1;
    }
    else
    {
        printf("\r\n");
        cJSON *item = cJSON_GetObjectItem(root, "paras");
        if (item != NULL)
        {
            cJSON *para_item;
            para_item = cJSON_GetObjectItem(item, "MeterID");
            if (para_item != NULL)
            {
                printf("Name is :%s type=%d, value is %s\n", para_item->string, para_item->type, para_item->valuestring);
                memcpy(cmddata->para.MeterID, para_item->valuestring, strlen(para_item->valuestring));
            }

            para_item = cJSON_GetObjectItem(item, "Key1");
            if (para_item != NULL)
            {
                printf("Name is :%s type=%d, value is %s\n", para_item->string, para_item->type, para_item->valuestring);
                memcpy(cmddata->para.Key1, para_item->valuestring, strlen(para_item->valuestring));
            }

            para_item = cJSON_GetObjectItem(item, "key2");
            if (para_item != NULL)
            {
                printf("Name is :%s type=%d, value is %s\n", para_item->string, para_item->type, para_item->valuestring);
                memcpy(cmddata->para.key2, para_item->valuestring, strlen(para_item->valuestring));
            }

            para_item = cJSON_GetObjectItem(item, "AtuoTime1");
            if (para_item != NULL)
            {
                printf("Name is :%s type=%d, value is %s\n", para_item->string, para_item->type, para_item->valuestring);
                memcpy(cmddata->para.AtuoTime1, para_item->valuestring, strlen(para_item->valuestring));
            }

            para_item = cJSON_GetObjectItem(item, "AtuoTime2");
            if (para_item != NULL)
            {
                printf("Name is :%s type=%d, value is %s\n", para_item->string, para_item->type, para_item->valuestring);
                memcpy(cmddata->para.AtuoTime2, para_item->valuestring, strlen(para_item->valuestring));
            }

            para_item = cJSON_GetObjectItem(item, "AtuoTime3");
            if (para_item != NULL)
            {
                printf("Name is :%s type=%d, value is %s\n", para_item->string, para_item->type, para_item->valuestring);
                memcpy(cmddata->para.AtuoTime3, para_item->valuestring, strlen(para_item->valuestring));
            }

            para_item = cJSON_GetObjectItem(item, "SettlDate");
            if (para_item != NULL)
            {
                printf("Name is :%s type=%d, value is %s\n", para_item->string, para_item->type, para_item->valuestring);
                memcpy(cmddata->para.SettlDate, para_item->valuestring, strlen(para_item->valuestring));
            }

            para_item = cJSON_GetObjectItem(item, "SettlBal");
            if (para_item != NULL)
            {
                printf("Name is :%s type=%d, value is %s\n", para_item->string, para_item->type, para_item->valuestring);
                memcpy(cmddata->para.SettlBal, para_item->valuestring, strlen(para_item->valuestring));
            }

            para_item = cJSON_GetObjectItem(item, "RechDate");
            if (para_item != NULL)
            {
                printf("Name is :%s type=%d, value is %s\n", para_item->string, para_item->type, para_item->valuestring);
                memcpy(cmddata->para.RechDate, para_item->valuestring, strlen(para_item->valuestring));
            }

            para_item = cJSON_GetObjectItem(item, "RechAmt");
            if (para_item != NULL)
            {
                printf("Name is :%s type=%d, value is %s\n", para_item->string, para_item->type, para_item->valuestring);
                memcpy(cmddata->para.RechAmt, para_item->valuestring, strlen(para_item->valuestring));
            }

            para_item = cJSON_GetObjectItem(item, "ValveCtrl");
            if (para_item != NULL)
            {
                printf("Name is :%s type=%d, value is %s\n", para_item->string, para_item->type, para_item->valuestring);
                memcpy(cmddata->para.ValveCtrl, para_item->valuestring, strlen(para_item->valuestring));
            }
        }

        item = cJSON_GetObjectItem(root, "service_id");
        if (item != NULL)
        {
            printf("Name is :%s type=%d, value is %s\n", item->string, item->type, item->valuestring);
            memcpy(cmddata->service_id, item->valuestring, strlen(item->valuestring));
        }

        item = cJSON_GetObjectItem(root, "command_name");
        if (item != NULL)
        {
            printf("Name is :%s type=%d, value is %s\n", item->string, item->type, item->valuestring);
            memcpy(cmddata->command_name, item->valuestring, strlen(item->valuestring));
        }

        item = cJSON_GetObjectItem(root, "object_device_id");
        if (item != NULL)
        {
            printf("Name is :%s type=%d, value is %s\n", item->string, item->type, item->valuestring);
            memcpy(cmddata->object_device_id, item->valuestring, strlen(item->valuestring));
        }

        cJSON_Delete(root);
    }
    return 0;
}

void test(void)
{
    //创建一个空的文档（对象）
    char *json_data = NULL;

    cJSON *json = cJSON_CreateObject();

    //向文档中增加一个键值对{"name":"王大锤"}
    cJSON_AddItemToObject(json, "name", cJSON_CreateString("王大锤"));
    //向文档中添加一个键值对
    //cJSON_AddItemToObject(json,"age",cJSON_CreateNumber(29));
    cJSON_AddNumberToObject(json, "age", 29);

    cJSON *array = NULL;
    cJSON_AddItemToObject(json, "love", array = cJSON_CreateArray());
    cJSON_AddItemToArray(array, cJSON_CreateString("LOL"));
    cJSON_AddItemToArray(array, cJSON_CreateString("NBA"));
    cJSON_AddItemToArray(array, cJSON_CreateString("Go shopping"));

    cJSON_AddNumberToObject(json, "score", 59);
    cJSON_AddStringToObject(json, "address", "beijing");
    //释放json结构所占用的内存
    json_data = cJSON_Print(json);
    printf("data:%s\r\n", json_data);
    myfree(json_data);

    json_data = cJSON_PrintUnformatted(json);
    printf("data:%s\r\n", json_data);
    myfree(json_data);

    cJSON_Delete(json);
}

const char meter[] = {"5fc4e387b4ec2202e999b732_642200000001"};

void upreply(char *rename, bool state)
{
    char *json_data = NULL; //用于输出
    cJSON *json = cJSON_CreateObject();
    cJSON *obj1 = NULL;

    cJSON_AddItemToObject(json, "result_code", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(json, "response_name", cJSON_CreateString(rename));
    //在对象上添加对象
    cJSON_AddItemToObject(json, "paras", obj1 = cJSON_CreateObject());
    if (state == 1)
        cJSON_AddItemToObject(obj1, "result", cJSON_CreateString("success"));
    else
    {
        cJSON_AddItemToObject(obj1, "result", cJSON_CreateString("fail")); //失败的回复码待改
    }

    //释放json结构所占用的内存
    json_data = cJSON_Print(json);
    printf("data:%s\r\n", json_data);
    myfree(json_data);

    json_data = cJSON_PrintUnformatted(json);
    printf("data:%s\r\n", json_data);
    myfree(json_data);

    cJSON_Delete(json);
}

void upFastate(char *metternum, bool state)
{
    /*{ "devices":[{"device_id":"5fc4e387b4ec2202e999b732_642200000001",
    "services":[{ "service_id":"ValveCtrlSever","properties":{"ValveStatus":"OPEN"}}]}]}  //上报子设备属性*/
    char buf[50];
    char *json_data = NULL; //用于输出
    cJSON *json = cJSON_CreateObject();
    cJSON *array = NULL;
    cJSON *array2 = NULL;
    cJSON *obj1 = NULL;
    cJSON *obj2 = NULL;
    cJSON *obj3 = NULL;

    cJSON_AddItemToObject(json, "devices", array = cJSON_CreateArray());

    //在数组上添加对象
    cJSON_AddItemToArray(array, obj1 = cJSON_CreateObject());
    memset(buf, 0, 50);
    memcpy(buf, metternum, strlen(metternum));
    cJSON_AddItemToObject(obj1, "device_id", cJSON_CreateString(buf));

    //在数组上添加数组
    cJSON_AddItemToObject(obj1, "services", array2 = cJSON_CreateArray());
    cJSON_AddItemToArray(array2, obj2 = cJSON_CreateObject());
    cJSON_AddItemToObject(obj2, "service_id", cJSON_CreateString("ValveCtrlSever"));

    //在对象上添加对象
    cJSON_AddItemToObject(obj2, "properties", obj3 = cJSON_CreateObject());

    if (state)
        cJSON_AddItemToObject(obj3, "ValveStatus", cJSON_CreateString("OPEN"));
    else
        cJSON_AddItemToObject(obj3, "ValveStatus", cJSON_CreateString("OFF"));

    //释放json结构所占用的内存
    json_data = cJSON_Print(json);
    printf("data:%s\r\n", json_data);
    myfree(json_data);

    json_data = cJSON_PrintUnformatted(json);
    printf("data:%s\r\n", json_data);
    myfree(json_data);

    cJSON_Delete(json);
}

void upState(char *metternum, bool state)
{
    /*{"services": [{"service_id": "$sub_device_manager","event_type": "sub_device_update_status",
    "paras": {"device_statuses": 
    [{"device_id": "5fc4e387b4ec2202e999b732_642200000001","status": "ONLINE"}]}}]}    //更新子设备状态*/
    char buf[50];
    char *json_data = NULL; //用于输出
    cJSON *json = cJSON_CreateObject();
    cJSON *array = NULL;
    cJSON *array2 = NULL;
    cJSON *obj1 = NULL;
    cJSON *obj2 = NULL;
    cJSON *obj3 = NULL;

    cJSON_AddItemToObject(json, "services", array = cJSON_CreateArray());

    //在数组上添加对象
    cJSON_AddItemToArray(array, obj1 = cJSON_CreateObject());
    cJSON_AddItemToObject(obj1, "service_id", cJSON_CreateString("$sub_device_manager"));
    cJSON_AddItemToObject(obj1, "event_type", cJSON_CreateString("sub_device_update_status"));

    //在对象上添加对象
    cJSON_AddItemToObject(obj1, "paras", obj2 = cJSON_CreateObject());
    cJSON_AddItemToObject(obj2, "device_statuses", array2 = cJSON_CreateArray());

    //在数组上添加对象
    cJSON_AddItemToArray(array2, obj3 = cJSON_CreateObject());

    memset(buf, 0, 50);
    memcpy(buf, metternum, strlen(metternum));
    cJSON_AddItemToObject(obj3, "device_id", cJSON_CreateString(buf));

    if (state)
        cJSON_AddItemToObject(obj3, "status", cJSON_CreateString("ONLINE"));
    else
        cJSON_AddItemToObject(obj3, "status", cJSON_CreateString("OFFLINE"));

    //释放json结构所占用的内存
    json_data = cJSON_Print(json);
    printf("data:%s\r\n", json_data);
    myfree(json_data);

    json_data = cJSON_PrintUnformatted(json);
    printf("data:%s\r\n", json_data);
    myfree(json_data);

    cJSON_Delete(json);
}

void upMeterReading(char *metternum, float LJL, uint16_t rssi, float BatteryVol,
                    uint16_t year, uint8_t mon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec,
                    uint8_t LowVolAlarm, uint8_t BatteryPowerDown, uint8_t TimeAbnormal)
{

    /*{ "devices":[{"device_id":"5fc4e387b4ec2202e999b732_642200000001",
    "services":[{ "service_id":"ReadMeterSever","properties":
    {"MeterReading":"11.1","RSSI":"99","BatteryVol":"3.6","MeterTime":"202011302242",
    "LowVolAlarm":"1","BatteryPowerDown":"2","TimeAbnormal":"3"}}]}]}  //上报子设备属性*/

    char buf[50];
    char *json_data = NULL; //用于输出
    cJSON *json = cJSON_CreateObject();
    cJSON *array = NULL;
    cJSON *array2 = NULL;
    cJSON *obj1 = NULL;
    cJSON *obj2 = NULL;
    cJSON *obj3 = NULL;

    cJSON_AddItemToObject(json, "devices", array = cJSON_CreateArray());

    memset(buf, 0, 50);
    memcpy(buf, metternum, strlen(metternum));

    //在数组上添加对象
    cJSON_AddItemToArray(array, obj1 = cJSON_CreateObject());
    cJSON_AddItemToObject(obj1, "device_id", cJSON_CreateString(buf));
    cJSON_AddItemToObject(obj1, "services", array2 = cJSON_CreateArray());

    //在数组上添加对象
    cJSON_AddItemToArray(array2, obj2 = cJSON_CreateObject());
    cJSON_AddItemToObject(obj2, "service_id", cJSON_CreateString("ReadMeterSever"));

    //在对象上添加对象
    memset(buf, 0, 50);
    sprintf(buf, "%.1f", LJL);
    cJSON_AddItemToObject(obj2, "properties", obj3 = cJSON_CreateObject());
    cJSON_AddItemToObject(obj3, "MeterReading", cJSON_CreateString(buf)); //输出累积量

    memset(buf, 0, 50);
    sprintf(buf, "%d", rssi);
    cJSON_AddItemToObject(obj3, "RSSI", cJSON_CreateString(buf)); //rssi

    memset(buf, 0, 50);
    sprintf(buf, "%1.1f", BatteryVol);
    cJSON_AddItemToObject(obj3, "BatteryVol", cJSON_CreateString(buf)); //BatteryVol

    memset(buf, 0, 50);
    sprintf(buf, "%d%d%d%d%d%d", year, mon, day, hour, min, sec);
    cJSON_AddItemToObject(obj3, "MeterTime", cJSON_CreateString(buf)); //MeterTime

    memset(buf, 0, 50);
    sprintf(buf, "%d", LowVolAlarm);
    cJSON_AddItemToObject(obj3, "LowVolAlarm", cJSON_CreateString(buf)); //LowVolAlarm

    memset(buf, 0, 50);
    sprintf(buf, "%d", BatteryPowerDown);
    cJSON_AddItemToObject(obj3, "BatteryPowerDown", cJSON_CreateString(buf)); //BatteryPowerDown

    memset(buf, 0, 50);
    sprintf(buf, "%d", TimeAbnormal);
    cJSON_AddItemToObject(obj3, "TimeAbnormal", cJSON_CreateString(buf)); //TimeAbnormal

    //释放json结构所占用的内存
    json_data = cJSON_Print(json);
    printf("data:%s\r\n", json_data);
    myfree(json_data);

    json_data = cJSON_PrintUnformatted(json);
    printf("data:%s\r\n", json_data);
    myfree(json_data);

    cJSON_Delete(json);
}

void upmeternum(char *metternum)
{
    /*{ "devices":[{"device_id":"5fc4e387b4ec2202e999b732_642200000001",
    "services":[{ "service_id":"BasicSever","properties":{"MeterID":"642200000001"}}]}]} */
    char buf[50];
    char *json_data = NULL; //用于输出
    cJSON *json = cJSON_CreateObject();
    cJSON *array = NULL;
    cJSON *array2 = NULL;
    cJSON *obj1 = NULL;
    cJSON *obj2 = NULL;
    cJSON *obj3 = NULL;

    cJSON_AddItemToObject(json, "devices", array = cJSON_CreateArray());

    memset(buf, 0, 50);
    memcpy(buf, metternum, strlen(metternum));

    //在数组上添加对象
    cJSON_AddItemToArray(array, obj1 = cJSON_CreateObject());
    cJSON_AddItemToObject(obj1, "device_id", cJSON_CreateString(buf));
    cJSON_AddItemToObject(obj1, "services", array2 = cJSON_CreateArray());

    //在数组上添加对象
    cJSON_AddItemToArray(array2, obj2 = cJSON_CreateObject());
    cJSON_AddItemToObject(obj2, "service_id", cJSON_CreateString("BasicSever"));

    //在对象上添加对象
    memset(buf, 0, 50);
    memcpy(buf, metternum, mystrlen(metternum, 12));
    cJSON_AddItemToObject(obj2, "properties", obj3 = cJSON_CreateObject());
    cJSON_AddItemToObject(obj3, "MeterID", cJSON_CreateString(buf));

    //释放json结构所占用的内存
    json_data = cJSON_Print(json);
    printf("data:%s\r\n", json_data);
    myfree(json_data);

    json_data = cJSON_PrintUnformatted(json);
    printf("data:%s\r\n", json_data);
    myfree(json_data);

    cJSON_Delete(json);
}

void upceo(uint32_t DeviceID, uint16_t year, uint8_t mon, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec)
{

    /*{"services": [{"service_id": "BasicSever","properties": {"DeviceID":"12345678","DeviceTime":"202011301524"}}]}  //上报网关属性*/
    //创建一个空的文档（对象）
    char buf[20];
    char *json_data = NULL; //用于输出
    cJSON *json = cJSON_CreateObject();
    cJSON *array = NULL;
    cJSON *obj1 = NULL;
    cJSON *obj2 = NULL;

    cJSON_AddItemToObject(json, "services", array = cJSON_CreateArray());
    //在数组上添加对象

    cJSON_AddItemToArray(array, obj1 = cJSON_CreateObject());
    cJSON_AddItemToObject(obj1, "service_id", cJSON_CreateString("BasicSever"));
    cJSON_AddItemToObject(obj1, "properties", obj2 = cJSON_CreateObject());

    sprintf(buf, "%d", DeviceID);

    cJSON_AddItemToObject(obj2, "DeviceID", cJSON_CreateString(buf));

    sprintf(buf, "%d%d%d%d%d%d", year, mon, day, hour, min, sec);

    cJSON_AddItemToObject(obj2, "DeviceTime", cJSON_CreateString(buf));

    //释放json结构所占用的内存
    json_data = cJSON_Print(json);
    printf("data:%s\r\n", json_data);
    myfree(json_data);

    json_data = cJSON_PrintUnformatted(json);
    printf("data:%s\r\n", json_data);
    myfree(json_data);

    cJSON_Delete(json);
}

void cjsontest(void)
{
    float ljl = 1234.23, bat = 3.6;
    uint16_t rss1 = 123;
    upceo(22334455, Hex_YearH * 100 + Hex_YearL, Hex_Mon, Hex_Day, Hex_Hour, Hex_Min, Hex_Sec);
    upmeternum((char *)meter);

    upMeterReading((char *)meter, ljl, rss1, bat, Hex_YearH * 100 + Hex_YearL, Hex_Mon, Hex_Day, Hex_Hour, Hex_Min, Hex_Sec, 1, 2, 3);

    upState((char *)meter, 1);

    upFastate((char *)meter, 1);

    upreply("test", 1);
}
