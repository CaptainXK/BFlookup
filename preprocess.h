#ifndef PREPROCESS_H
#define PREPROCESS_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "inttypes.h"

#define PP_FILE_NAME 60                       //最长的文件名称，包含.txt 
#define PP_MAX_NUMBER 300000                   //命名前缀数目的最大值 
#define PP_MAX_LENGTH 32                      //字块数的最大值 

typedef struct Name_Prefix{                   //命名前缀存储信息 
  char name[500];                             //完整命名信息 
  uint64_t fp;                                //命名对应的指纹信息 
  int component;                              //命名字块数
  uint32_t bloom[1000];                       //命名对应布鲁姆值 ,32000bits 
}Name_Prefix;
typedef Name_Prefix *Name_Prefix_P;           //命名前缀的指针类型 

int count_component(char name[]);                                                                   //根据符号“/”计算命名前缀的字块数 
int load_prefixes(char name[], Name_Prefix_P name_list[], int length[]);                            //读入命名前缀,返回文件的行数，其中length存储每个字块长度的命名数目 
int process_middle_prefix(char input[], char output[]);                                             //产生虚拟的中间命名 
uint64_t murmurHash64B (const void * key, int len, uint32_t seed);                                  //网上直接找的Murmur哈希 
int cut_string(char name[], int n);                                                                 //截取n个字块的字符串，用于二分法和BF的查找  
int clr_Name_Prefix(Name_Prefix_P item);															//清空输入的前缀实体                        

#endif
