#include "order.h"

int hash_table_lookup_order(Hash_Table_P ht[], Name_Prefix_P name_list, int identity[][PP_MAX_LENGTH + 1])
{						//???ǰ׺hlo_
	int hlo_length, hlo_flag, hlo_len;
	int hlo_entry = 0;
	int hlo_bucket = 0;
	int hlo_table = 0;
	hlo_length = name_list->component;
	if(hlo_length > 32)
		hlo_length = 32;
	for(hlo_length; hlo_length > 0; hlo_length--)
	{
		if(ht[hlo_length] == NULL)
			continue;
		hlo_flag = 0;
		char hlo_name[500];
		uint64_t hlo_temp, hlo_hash;
		hlo_len = cut_string(name_list->name, hlo_length);
		memcpy(hlo_name, name_list->name, hlo_len);
		hlo_name[hlo_len] = '\0';
		hlo_temp = murmurHash64B(hlo_name, strlen(hlo_name), 0xEE6B27EB) & 0xfffff;
		hlo_hash = hlo_temp % ht[hlo_length]->bucket_number;
		for(int hlo_j = 0; hlo_j < HT_BUCKET_ENTRY_NUMBER; hlo_j++)
		{
			if(ht[hlo_length]->buckets[hlo_hash]->entry[hlo_j] != NULL)
			{
				if(ht[hlo_length]->buckets[hlo_hash]->entry[hlo_j]->fp == hlo_temp)
				{
					if(memcmp((char *) ht[hlo_length]->buckets[hlo_hash]->entry[hlo_j]->addr, hlo_name, hlo_len) == 0)
					{
						hlo_table = hlo_length;
						hlo_bucket = hlo_hash;
						hlo_entry = hlo_j;
						hlo_flag = 1;
						break;
					}
				}
			}
		}
		if((hlo_flag == 0) && (ht[hlo_length]->buckets[hlo_hash]->collided == 1))
		{
			if(identity[hlo_hash][hlo_length] != -1)
			{
				for(int hlo_i = 0; hlo_i < HT_BUCKET_ENTRY_NUMBER;hlo_i++)
				{
					if(ht[0]->buckets[identity[hlo_hash][hlo_length]]->entry[hlo_i] != NULL)
					{
						if(ht[0]->buckets[identity[hlo_hash][hlo_length]]->entry[hlo_i]->fp == hlo_temp)
						{
							if(memcmp((char *) ht[0]->buckets[identity[hlo_hash][hlo_length]]->entry[hlo_i]->addr, hlo_name, hlo_len) == 0)
							{
								hlo_table = 0;
								hlo_bucket = identity[hlo_hash][hlo_hash];
								hlo_entry = hlo_i;
								hlo_flag = 1;
								break;
							}
						}
					}
				}
			}
		}
		if(hlo_flag == 1)
			break;
	}
//	FILE *hlo_result;
//	hlo_result = fopen("result.txt","a+");
	if(hlo_flag == 1)
	{
		//printf("The lookup result is entry[%d] in bucket[%d] in hash table[%d]!\n", hlo_entry, hlo_bucket, hlo_table);
//		fprintf(hlo_result, "The %d name is lookuped %d times, and match the prefix successful!\n", hlo_i, hlo_number);
//		fclose(hlo_result);
		return 1;
	}
	else if(hlo_flag == 0)
	{
		//printf("The name %s lookup Failed!\n",name_list->name);
//		fprintf(hlo_result, "The %d name is lookuped %d times, and match the prefix failed!\n", hlo_i, hlo_number);
//		fclose(hlo_result);
		return -1;
	}
}
