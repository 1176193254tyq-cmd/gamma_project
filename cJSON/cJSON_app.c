#include "cJSON_app.h"


static sys_rom_data_str default_g_sys_rom_data=
{
    .uint8_data[0]=5,
    .uint8_data[1]=253,
    .uint16_data[0]=5,
    .uint16_data[1]=105,
    .uint16_data[2]=65500,
    .uint32_data[0]=5,
    .uint32_data[1]=65555,
    .uint32_data[2]=123456,
    .uint32_data[3]=4567800,
    .float_data[0]=0.12586455,
    .float_data[1]=11111.1111,
    .float_data[2]=55555555.55,
    .float_data[3]=4522.666,
    .float_data[4]=35.6668554,
    .string_data="test json save data",
};
static sys_rom_data_str backup_g_sys_rom_data;
sys_rom_data_str g_sys_rom_data;



static void save_rom_data(sys_rom_data_str *p1)
{
    char *json_data;
    cJSON *file_head = cJSON_CreateObject();        //文件表头
    cJSON *sys_rom_data=cJSON_CreateObject();       //文件内第一个对象

    cJSON *uint8_data = cJSON_CreateIntArray(p1->uint8_data,2);
    cJSON_AddItemToObject(sys_rom_data,"uint8_data",uint8_data);

    cJSON *uint16_data = cJSON_CreateIntArray(p1->uint16_data,3);
    cJSON_AddItemToObject(sys_rom_data,"uint16_data",uint16_data);

    cJSON *uint32_data = cJSON_CreateIntArray(p1->uint32_data,4);
    cJSON_AddItemToObject(sys_rom_data,"uint32_data",uint32_data);      

    cJSON *float_data = cJSON_CreateDoubleArray(p1->float_data,5);
    cJSON_AddItemToObject(sys_rom_data,"float_data",float_data);  

    cJSON_AddStringToObject(sys_rom_data,"string_data","test json save data");		//添加字符串 

    cJSON_AddItemToObject(file_head,"sys_rom_data",sys_rom_data);		//添加数组到对象

    json_data = cJSON_Print(file_head);	//JSON数据结构转换为JSON字符串

    FILE *fp=fopen(JOSN_FILE,"w+");
    if(fp == NULL)
    {
        return;
    }
    else
    {
        fputs(json_data,fp);            //保存到文件
        fclose(fp);
    }
	cJSON_Delete(file_head);//清除结构体
}



void Read_Rom_Data(sys_rom_data_str *p1)
{
    FILE *fp = fopen(JOSN_FILE, "r");
    uint8_t arry_size;
    char json_read[2*1024*1024]={0};
    char *read_str;
    if (fp == NULL) 
    {
        printf("没有读取到文件\r\n");
        exit(EXIT_FAILURE);
    }    
    fread(json_read,1,sizeof(json_read),fp);
    // printf("%s\n",json_read);
    cJSON* cjson = cJSON_Parse(json_read);
	if(cjson == NULL)						//判断转换是否成功
	{
		printf("cjson error...\r\n");
        return;
	}
    cJSON *sys_rom_data = cJSON_GetObjectItem(cjson,"sys_rom_data");    //获取对象sys_rom_data

    cJSON *uint8_data = cJSON_GetObjectItem(sys_rom_data,"uint8_data");
    arry_size = cJSON_GetArraySize(uint8_data);
    for(uint8_t i=0;i<arry_size;i++)
    {
        p1->uint8_data[i]=cJSON_GetArrayItem(uint8_data,i)->valueint;
        // printf("p1->uint8_data[%d] %d\n",i,p1->uint8_data[i]);
    }

    cJSON *uint16_data = cJSON_GetObjectItem(sys_rom_data,"uint16_data");
    arry_size = cJSON_GetArraySize(uint16_data);
    for(uint8_t i=0;i<arry_size;i++)
    {
        p1->uint16_data[i]=cJSON_GetArrayItem(uint16_data,i)->valueint;
        // printf("p1->uint16_data[%d] %d\n",i,p1->uint16_data[i]);
    }

    cJSON *uint32_data = cJSON_GetObjectItem(sys_rom_data,"uint32_data");
    arry_size = cJSON_GetArraySize(uint32_data);
    for(uint8_t i=0;i<arry_size;i++)
    {
        p1->uint32_data[i]=cJSON_GetArrayItem(uint32_data,i)->valueint;
        // printf("p1->uint32_data[%d] %d\n",i,p1->uint32_data[i]);
    }

    cJSON *float_data = cJSON_GetObjectItem(sys_rom_data,"float_data");
    arry_size = cJSON_GetArraySize(float_data);
    for(uint8_t i=0;i<arry_size;i++)
    {
        p1->float_data[i]=cJSON_GetArrayItem(float_data,i)->valuedouble;
        // printf("p1->uint32_data[%d] %f\n",i,p1->float_data[i]);
    }

    p1->string_data = cJSON_GetObjectItem(sys_rom_data,"string_data")->valuestring;
    // printf("%s\n",p1->string_data);

    cJSON_Delete(cjson);//清除结构体 
    fclose(fp);
}

void Sys_Rom_Data_Init()
{
    int ret;
    int fd=0;
    ret = access(JOSN_FILE, F_OK); //如果文件存在，返回0，不存在，返回-1
    if (ret == -1) 
    {
        printf("%s 不存在,创建新的!\n", JOSN_FILE);
        save_rom_data(&default_g_sys_rom_data);
    }
    else 
    {
        printf(" %s 存在!\n", JOSN_FILE);
    }
    Read_Rom_Data(&g_sys_rom_data);
    backup_g_sys_rom_data=g_sys_rom_data;
}

void Sys_Rom_Data_Check()
{
    uint8_t *source_data=(uint8_t *)(&g_sys_rom_data);
    uint8_t *backup_data=(uint8_t *)(&backup_g_sys_rom_data);
    uint16_t data_size=sizeof(sys_rom_data_str);
    for(uint16_t i=0;i<data_size;i++)
    {
        if(*(source_data+i) != *(backup_data+i))
        {
            backup_g_sys_rom_data=g_sys_rom_data;
            save_rom_data(&g_sys_rom_data);
            printf("数据有差异，保存最新的数值\n");
        }
    }
    sleep(1);
}

void Sys_Rom_Data_Task()
{
    Sys_Rom_Data_Init();
    while (1)
    {
        Sys_Rom_Data_Check();
        sleep(1);
    }
    
}