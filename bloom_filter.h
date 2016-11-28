#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include "stdio.h"
#include "stdlib.h" 
#include "string.h"
#include "inttypes.h"
#include "assert.h"

#include "preprocess.h"
#include "hash_table.h"

#define BLOOM_SIZE 1000                          //bitmap总共32 * 1000 = 32000位 
#define HASH_TIME  3                             //BF中哈希次数为3 
#define BF_DEEP    5                             //BF中二分搜索的深度 log2n 

int calculate_Bloom_Filter(Hash_Table_P ht[], Name_Prefix_P name_list);                                        //计算每个命名前缀的BF值
int merge_Bloom_Filter(Hash_Table_P ht[], Name_Prefix_P name_list[], int line);                         //将布鲁姆合并到一个最终的布鲁姆过滤器中 
int lookup_Bloom_Filter(Hash_Table_P ht[], Name_Prefix_P name_list, int identity[][PP_MAX_LENGTH + 1]); //布鲁姆过滤器查找 
int calculate_Bloom_Loc(Hash_Table_P ht[], Name_Prefix_P name_list, uint64_t loc[]);//记录布鲁姆过滤器中哈希确定的位置 放在loc[] 

#endif 
