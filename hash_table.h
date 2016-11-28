#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "stdio.h"
#include "stdlib.h" 
#include "string.h"
#include "inttypes.h"

#include "preprocess.h"

#define HT_CACHE_LINE 64                               //缓存字节数 
//#define HT_COMPONETS 32                                //命名前缀的最大字段数 
//#define HT_LENGTH 10                                   //命名前缀字段中的最长字符数 
#define HT_BUCKET_ENTRY_NUMBER 7                       //1个哈希桶最多包含7个条目  
//#define HT_BUCKET_NUMBER 900//300                    //二分法最大的哈希表需要900个桶，顺序法需要300个，统一定义需要该参数 

typedef struct Hash_Entry{                             //哈希条目结构 
	uint64_t fp: 20;                                   //20bit命名前缀的指纹信息  
	uint64_t addr: 44;	                               //44bit命名前缀的地址信息  
}Hash_Entry;
typedef Hash_Entry *Hash_Entry_P;                      //定义哈希条目结构体的指针类型 

typedef struct Hash_Bucket{                            //哈希桶结构 
	uint64_t occupied: 7;                              //7bit占用位，1表示占用，0表示为空 
	uint64_t collided: 7;					           //7bit冲突位，1表示有冲突 
	uint64_t leaf: 7;                                  //7bit叶子位，1表示为叶子前缀 
	uint64_t next : 42;                                //离下一个桶的距离 
	uint64_t reserved : 1;                             //1bit不使用的位 
	Hash_Entry_P entry[HT_BUCKET_ENTRY_NUMBER];        //哈希桶存储的7个哈希条目 
}__attribute__ ((aligned(HT_CACHE_LINE)))Hash_Bucket;  //以64字节的方式在内存中对其 1*8+7*8
typedef Hash_Bucket *Hash_Bucket_P;                    //定义哈希桶结构体的指针类型 

typedef struct Hash_Table{                             //哈希表结构 
  uint32_t length;                                     //哈希表存储的命名前缀长度 
  Hash_Bucket_P *buckets;                              //哈希表存储的哈希桶      
  uint32_t *bloom;                                     //哈希表中的BF       
  int bucket_number;                                   //桶的数目 
  int bf_number;                                       //BF的数目，一个为32位 
}Hash_Table;
typedef Hash_Table *Hash_Table_P;                      //定义哈希表结构体的指针类型 


int hash_bucket_init(Hash_Bucket_P &hb);                                                    //哈希桶初始化 
int hash_table_init(Hash_Table_P &ht, int length, int bucket_numer);                        //哈希表初始化
int hash_table_withBF_init(Hash_Table_P &ht, int length, int bucket_numer, int bf_number);  //带有BF的哈希表初始化 
int hash_table_insert(Hash_Table_P ht[], Name_Prefix_P name_list[], int line, Hash_Bucket_P addition[], int identity[][PP_MAX_LENGTH + 1]);  //哈希表插入 
int hash_table_lookup(Hash_Table_P ht[], Name_Prefix_P name_list, int identity[][PP_MAX_LENGTH + 1]);      //哈希表查找
int hash_table_delete(Hash_Table_P ht[], Name_Prefix_P name_list, int identity[][PP_MAX_LENGTH + 1]);      //哈希表删除
int hash_table_output(Hash_Table_P ht);                                                     //哈希表输出
int write_ht_information(char name[], Hash_Table_P ht[], Hash_Bucket_P addition[]);         //将哈希表的相关信息写入文件中  

#endif
