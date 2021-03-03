#include "define_all.h"
#include "12864.h"
#include "malloc.h"
#include "cJSON.h"

extern void Refresh(void);
uint8_t showemu = 0;

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
cmddata_type cmddata;

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

int main(void)
{
    Sysinit_Exp(); //系统初始化
    uCOS_Start();
}

void User_IO_Init(void)
{
    //	OutputIO(LED_GPIO, LED_PIN, 0);
    OutputIO(NB_EN_GPIO, NB_EN_PIN, 0);
    NB_OFF;
    PWM_DIR;
    InputtIO(KEY1_GPIO, KEY1_PIN, 1); /* 如果外部电路有上拉配成0 */
    InputtIO(KEY2_GPIO, KEY2_PIN, 1); /* 如果外部电路有上拉配成0 */
    InputtIO(KEY3_GPIO, KEY3_PIN, 1); /* 如果外部电路有上拉配成0 */

    //FM33A0XX的7组GPIO（A~G）最多可以产生24个外部引脚中断，部分IO不能同时开启中断功能
    GPIO_EXTI_Init(KEY1_GPIO, KEY1_PIN, EXTI_BOTH);
    GPIO_EXTI_Init(KEY2_GPIO, KEY2_PIN, EXTI_BOTH);
    GPIO_EXTI_Init(KEY3_GPIO, KEY3_PIN, EXTI_BOTH);

    /*NVIC中断配置*/
    NVIC_DisableIRQ(GPIO_IRQn);
    NVIC_SetPriority(GPIO_IRQn, 0); //中断优先级配置
    NVIC_EnableIRQ(GPIO_IRQn);
}

void LowPower_IO_Init(void)
{
    /*低功耗使用*/
    NB_OFF;
}

extern void PressureAndTempTest(void);

void uCOS_SystemInit(void)
{
    Uart_INIT_Type Uart_init;

    Uart_init.Nvicpry = 1; //中断优先级选择  越小优先级越高  不能大于3

    //Uart_init.SEL = Uart1_UsePE3PE4; //只有使用UART1时候才需要选择

    Uart_init.SEL = Uart1_UsePB0PB1; //只有使用UART1时候才需要选择

    Uart_init.IRMod = NoUSEIRMod;          //不使用红外38K调制
    Uart_init.RxIEEN = 1;                  //开启接收中断
    Uart_init.TxIEEN = 0;                  //开启发送中断
    Uart_init.uartint.BaudRate = 9600;     //设置波特率
    Uart_init.uartint.DataBit = Eight8Bit; //8位数据位
    Uart_init.uartint.ParityBit = NONE;    //不开校验
    Uart_init.uartint.StopBit = OneBit;    //两位停止位

    API_Uartx_Init(UART1, &Uart_init); //设置Uart
    // fd 12 00 0C 00 00 00 00 d5 d8 c7 ec bd f0 c5 f4 dd cc bb aa
    /* 系统初始化函数都放在这里 */
    //LowPower_IO_Init();
    uCOS_LEDCreate();
    GUI_Init();
    printf("\r\n程序已开始运行\r\n");
    SIMLT_I2C_OP_INIT();
}

extern uint8_t notintosleep;
void uCOS_LowPower(void)
{
    /*空闲任务*/
    // if (!notintosleep)
    //     Sleep(0);
}
void UART1_IRQ_Rx_CallBack(uint8_t data)
{
    uint8_t os_err;
    static uint8_t Buf[60], LEN = 0;

    Buf[LEN] = data;
    if (LEN == 0 && Buf[0] != 0x55)
        return;

    LEN++;
    if (LEN == 11) //接收到 11 个数据
    {
        OSTaskQPost((OS_TCB *)&TaskUart1TCB,
                    (void *)&Buf,
                    (OS_MSG_SIZE)LEN,
                    (OS_OPT)OS_OPT_POST_FIFO,
                    (OS_ERR *)&os_err);
        LEN = 0;
    }
}

float _60yCura[3], _60yCurw[3], _60yCurangle[3];
float _60ya[3], _60yw[3], _60yangle[3];
// void uCOS_APP_Uart1(uint8_t *buf, uint16_t len)
// {
//     if (buf[0] == 0x55)
//     {
//         //检查帧头
//         switch (buf[1])
//         {
//         case 0x51:
//             _60ya[0] = ((short)(buf[3] << 8 | buf[2])) / 32768.0 * 16;
//             _60ya[1] = ((short)(buf[5] << 8 | buf[4])) / 32768.0 * 16;
//             _60ya[2] = ((short)(buf[7] << 8 | buf[6])) / 32768.0 * 16;

//             break;
//         case 0x52:
//             _60yw[0] = ((short)(buf[3] << 8 | buf[2])) / 32768.0 * 2000;
//             _60yw[1] = ((short)(buf[5] << 8 | buf[4])) / 32768.0 * 2000;
//             _60yw[2] = ((short)(buf[7] << 8 | buf[6])) / 32768.0 * 2000;

//             break;
//         case 0x53:
//             _60yangle[0] = ((short)(buf[3] << 8 | buf[2])) / 32768.0 * 180;
//             _60yangle[1] = ((short)(buf[5] << 8 | buf[4])) / 32768.0 * 180;
//             _60yangle[2] = ((short)(buf[7] << 8 | buf[6])) / 32768.0 * 180;

//             break;
//         }
//     }
//     (abs((int)_60yw[0]) > 5) ? (LED1ON) : (LED1OFF);
//     (abs((int)_60yw[1]) > 5) ? (LED2ON) : (LED2OFF);
//     (abs((int)_60yw[2]) > 5) ? (LED3ON) : (LED3OFF);
// }

void uCOS_APP_Uart1(uint8_t *buf, uint16_t len)
{
    static uint8_t acnt = 0, wcnt = 0, anglecnt = 0;
    uint8_t tbuf[3] = {0xFF, 0xAA, 0x60};
    if (acnt == 0 && wcnt == 0 && anglecnt == 0)
    {
        memcpy(_60ya, _60yCura, sizeof(float) * 3);
        memcpy(_60yw, _60yCurw, sizeof(float) * 3);
        memcpy(_60yangle, _60yCurangle, sizeof(float) * 3);
        memset(_60yCura, 0, sizeof(float) * 3);
        memset(_60yCurw, 0, sizeof(float) * 3);
        memset(_60yCurangle, 0, sizeof(float) * 3);
    }
    if (buf[0] == 0x55)
    {
        //检查帧头
        switch (buf[1])
        {
        case 0x51:
            if (++acnt > 2)
            {
                _60yCura[0] = ((short)(buf[3] << 8 | buf[2])) / 32768.0 * 16;
                _60yCura[1] = ((short)(buf[5] << 8 | buf[4])) / 32768.0 * 16;
                _60yCura[2] = ((short)(buf[7] << 8 | buf[6])) / 32768.0 * 16;
            }
            break;
        case 0x52:
            if (++wcnt > 2)
            {
                _60yCurw[0] = ((short)(buf[3] << 8 | buf[2])) / 32768.0 * 2000;
                _60yCurw[1] = ((short)(buf[5] << 8 | buf[4])) / 32768.0 * 2000;
                _60yCurw[2] = ((short)(buf[7] << 8 | buf[6])) / 32768.0 * 2000;
            }
            break;
        case 0x53:
            if (++anglecnt > 2)
            {
                _60yCurangle[0] = ((short)(buf[3] << 8 | buf[2])) / 32768.0 * 180;
                _60yCurangle[1] = ((short)(buf[5] << 8 | buf[4])) / 32768.0 * 180;
                _60yCurangle[2] = ((short)(buf[7] << 8 | buf[6])) / 32768.0 * 180;
            }
            break;
        }
    }

    if (acnt > 2 && wcnt > 2 && anglecnt > 2)
    {
        acnt = 0;
        wcnt = 0;
        anglecnt = 0;
        API_Uart_Send(UART1, tbuf, 3);
    }
}

void uCOS_APP_Uart2(uint8_t *buf, uint16_t len)
{
    iotRecDat(buf, len);
}

extern iot_foo_type FOO;
extern void jiamitest(uint8_t *buf, uint16_t len);
extern void jiamitest2(uint8_t *buf, uint16_t len);
void uCOS_APP_Uart3(uint8_t *buf, uint16_t len)
{
    if (strstr((char *)buf, "coap") || strstr((char *)buf, "COAP"))
    {
        FOO.LinkMode = CoapNum;
        uCOS_NBTaskCreate();
    }
    else if (strstr((char *)buf, "tcp") || strstr((char *)buf, "TCP"))
    {
        FOO.LinkMode = TCPNum;
        uCOS_NBTaskCreate();
    }
    else if (strstr((char *)buf, "udp") || strstr((char *)buf, "UDP"))
    {
        FOO.LinkMode = UDPNum;
        uCOS_NBTaskCreate();
    }
    else
    {
        cJSON_to_cmddata((char *)buf, &cmddata);
    }
    //API_Uart_Send(UART2, buf, len);

    //    jiamitest(buf, len);
    //    jiamitest2(buf, len);
}

uint32_t sysvdd;
float systemp;
void uCOS_APP_1s(void)
{
    static uint8_t ss = 0;
    if (ss++ > 5)
    {
        API_Uart_Send(UART1, &ss, 1);
        ss = 0;
    }

    //	float ljl = 1234.23, bat = 3.6;
    //	uint16_t rss1 = 123;

    //	upceo(22334455, Hex_YearH * 100 + Hex_YearL, Hex_Mon, Hex_Day, Hex_Hour, Hex_Min, Hex_Sec);
    //	upmeternum((char *)meter);

    //	upMeterReading((char *)meter, ljl, rss1, bat, Hex_YearH * 100 + Hex_YearL, Hex_Mon, Hex_Day, Hex_Hour, Hex_Min, Hex_Sec, 1, 2, 3);

    //	upState((char *)meter, 1);

    //	upFastate((char *)meter, 1);

    //  upreply(1);

    API_GetTime(RealTime);
    iotTim();

    sysvdd = ADC_Exp(CH_VDD);
    systemp = Temp_Exp();

    //printf("\r\n当前时间为：%d%d年 %d月 %d日 %d:%d:%d\r\n",Hex_YearH,Hex_YearL,Hex_Mon,Hex_Day,Hex_Hour,Hex_Min,Hex_Sec);
}

void uCOS_APP_1m(void)
{
#if OS_CFG_DBG_EN > 0
    CPU_STK_SIZE free;
    CPU_STK_SIZE used;
    OS_ERR err;

    CPU_SR_ALLOC();
    OS_TCB *p_tcb;
    CPU_CRITICAL_ENTER();
    p_tcb = OSTaskDbgListPtr;
    CPU_CRITICAL_EXIT();
    printf("\r\n已用栈(B)   剩余栈(B)   栈使用率(%%)  优先级  CPU使用率(%%)   CPU最大使用率(%%)   调用次数     任务名\r\n");
    for (;;)
    {
        if (p_tcb != (OS_TCB *)0)
        {
            OSTaskStkChk(p_tcb, &free, &used, &err);
            printf("%4d        %4d         %3d%%        %3d  %10.2f%%      %10.2f%%       %8d       %s   \r\n",
                   used, free, (used * 100) / (used + free), p_tcb->Prio,
                   p_tcb->CPUUsage / 100.0, p_tcb->CPUUsageMax / 100.0, p_tcb->CtxSwCtr, p_tcb->NamePtr);
            CPU_CRITICAL_ENTER();
            p_tcb->CPUUsageMax = 0;
            p_tcb = p_tcb->DbgNextPtr; //指向下一个任务的TCB
            CPU_CRITICAL_EXIT();
        }
        else
        {
            break;
        }
    }
    RchfAdjust(ETIM4, clkmode);
#endif
}

void uCOS_APP_1h(void)
{
}

void uCOS_APP_1d(void)
{
}

void uCOS_APP_Lptim(void)
{
    PWM_CPL;
}
extern void yasuotest(void);
void uCOS_APP_IO(void)
{
    uint8_t re;

    re = API_GPIO_Judge(KEY1_GPIO, KEY1_PIN);
    if (re == 1)
    {
        /* 下降沿中断处理 */
        uCOS_NBTaskCreate();
    }

    re = API_GPIO_Judge(KEY2_GPIO, KEY2_PIN);
    if (re == 1)
    {
        /* 下降沿中断处理 */
        showemu++;
        GUI_Clear();
        if (showemu >= 4)
        {
            showemu = 0;
        }
    }

    re = API_GPIO_Judge(KEY3_GPIO, KEY3_PIN);
    if (re == 1)
    {
        /* 下降沿中断处理 */
        yasuotest();
    }
}
