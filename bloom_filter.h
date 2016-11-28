#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include "stdio.h"
#include "stdlib.h" 
#include "string.h"
#include "inttypes.h"
#include "assert.h"

#include "preprocess.h"
#include "hash_table.h"

#define BLOOM_SIZE 1000                          //bitmap�ܹ�32 * 1000 = 32000λ 
#define HASH_TIME  3                             //BF�й�ϣ����Ϊ3 
#define BF_DEEP    5                             //BF�ж������������ log2n 

int calculate_Bloom_Filter(Hash_Table_P ht[], Name_Prefix_P name_list);                                        //����ÿ������ǰ׺��BFֵ
int merge_Bloom_Filter(Hash_Table_P ht[], Name_Prefix_P name_list[], int line);                         //����³ķ�ϲ���һ�����յĲ�³ķ�������� 
int lookup_Bloom_Filter(Hash_Table_P ht[], Name_Prefix_P name_list, int identity[][PP_MAX_LENGTH + 1]); //��³ķ���������� 
int calculate_Bloom_Loc(Hash_Table_P ht[], Name_Prefix_P name_list, uint64_t loc[]);//��¼��³ķ�������й�ϣȷ����λ�� ����loc[] 

#endif 
