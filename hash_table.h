#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "inttypes.h"

#include "preprocess.h"

#define HT_CACHE_LINE 64                               //�����ֽ���
//#define HT_COMPONETS 32                                //����ǰ׺�������ֶ���
//#define HT_LENGTH 10                                   //����ǰ׺�ֶ��е���ַ���
#define HT_BUCKET_ENTRY_NUMBER 7                       //1����ϣͰ��������7����Ŀ
//#define HT_BUCKET_NUMBER 900//300                    //���ַ������Ĺ�ϣ����Ҫ900��Ͱ��˳������Ҫ300����ͳһ������Ҫ�ò���

typedef struct Hash_Entry{                             //��ϣ��Ŀ�ṹ
	uint64_t fp: 20;                                   //20bit����ǰ׺��ָ����Ϣ
	uint64_t addr: 44;	                               //44bit����ǰ׺�ĵ�ַ��Ϣ
}Hash_Entry;
typedef Hash_Entry *Hash_Entry_P;                      //������ϣ��Ŀ�ṹ����ָ������

typedef struct Hash_Bucket{                            //��ϣͰ�ṹ
	uint64_t occupied: 7;                              //7bitռ��λ��1��ʾռ�ã�0��ʾΪ��
	uint64_t collided: 7;					           //7bit��ͻλ��1��ʾ�г�ͻ
	uint64_t leaf: 7;                                  //7bitҶ��λ��1��ʾΪҶ��ǰ׺
	uint64_t next : 42;                                //����һ��Ͱ�ľ���
	uint64_t reserved : 1;                             //1bit��ʹ�õ�λ
	Hash_Entry_P entry[HT_BUCKET_ENTRY_NUMBER];        //��ϣͰ�洢��7����ϣ��Ŀ
}__attribute__ ((aligned(HT_CACHE_LINE)))Hash_Bucket;  //��64�ֽڵķ�ʽ���ڴ��ж��� 1*8+7*8
typedef Hash_Bucket *Hash_Bucket_P;                    //������ϣͰ�ṹ����ָ������

typedef struct Hash_Table{                             //��ϣ���ṹ
  uint32_t length;                                     //��ϣ���洢������ǰ׺����
  Hash_Bucket_P *buckets;                              //��ϣ���洢�Ĺ�ϣͰ
  uint32_t *bloom;                                     //��ϣ���е�BF
  int bucket_number;                                   //Ͱ����Ŀ
  int bf_number;                                       //BF����Ŀ��һ��Ϊ32λ
}Hash_Table;
typedef Hash_Table *Hash_Table_P;                      //������ϣ���ṹ����ָ������


int hash_bucket_init(Hash_Bucket_P &hb);                                                    //��ϣͰ��ʼ��
int hash_table_init(Hash_Table_P &ht, int length, int bucket_numer);                        //��ϣ����ʼ��
int hash_table_withBF_init(Hash_Table_P &ht, int length, int bucket_numer, int bf_number);  //����BF�Ĺ�ϣ����ʼ��
int hash_table_insert(Hash_Table_P ht[], Name_Prefix_P name_list[], int line, Hash_Bucket_P addition[], int identity[][PP_MAX_LENGTH + 1]);  //��ϣ������
int hash_table_lookup(Hash_Table_P ht[], Name_Prefix_P name_list, int identity[][PP_MAX_LENGTH + 1]);      //��ϣ������
int hash_table_delete(Hash_Table_P ht[], Name_Prefix_P name_list, int identity[][PP_MAX_LENGTH + 1]);      //��ϣ��ɾ��
int hash_table_output(Hash_Table_P ht);                                                     //��ϣ������
int write_ht_information(char name[], Hash_Table_P ht[], Hash_Bucket_P addition[]);         //����ϣ����������Ϣд���ļ���

#endif
