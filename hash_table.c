#include "hash_table.h"

  


int hash_bucket_init(Hash_Bucket_P &hb)
{						//???ǰ׺hb_ 
	Hash_Bucket_P hp = new Hash_Bucket();
	hp->occupied = 0;
	hp->collided = 0;
	hp->leaf = 0;
	hp->reserved = 0;
	
	for(int hb_i = 0; hb_i < HT_BUCKET_ENTRY_NUMBER; hb_i++)
		hp->entry[hb_i] = NULL;
	
	hp->next = 0;
	hb = hp;
	return 0;
}

int hash_table_init(Hash_Table_P &ht, int length, int bucket_number)
{						//???ǰ׺ht_ 
	Hash_Table_P hp = new Hash_Table();
	
	hp->length = length;
	hp->buckets = new Hash_Bucket_P[bucket_number];                     //???int*, new int???????Ĺ??	
	for(int ht_i = 0; ht_i < bucket_number; ht_i++)
		hash_bucket_init(hp->buckets[ht_i]);
    
	hp->bloom = new uint32_t[1];
	hp->bloom[0] = 0;
	hp->bucket_number = bucket_number;
	hp->bf_number = 1; 
	
	ht = hp;
	return 0;
}

int hash_table_withBF_init(Hash_Table_P &ht, int length, int bucket_number, int bf_number)
{						//???ǰ׺htb_ 
	Hash_Table_P hp = new Hash_Table();
	
	hp->length = length;
	hp->buckets = new Hash_Bucket_P[bucket_number];                     //???int*, new int???????Ĺ??	
	hp->bloom = new uint32_t [bf_number];
	
	for(int ht_i = 0; ht_i < bucket_number; ht_i++)
		hash_bucket_init(hp->buckets[ht_i]);

	for(int ht_j = 0; ht_j < bf_number; ht_j++)
	{
		hp->bloom[ht_j] = 0;
	}
		
	
	hp->bucket_number = bucket_number;
	hp->bf_number = bf_number; 
	
	ht = hp;
	return 0;
}

int hash_table_insert(Hash_Table_P ht[], Name_Prefix_P name_list[], int line, Hash_Bucket_P addition[], int identity[][PP_MAX_LENGTH + 1])
{						//???ǰ׺hi_
	int hi_length, hi_bucket;          
	int hi_temp1, hi_temp2, hi_temp3;            //?洢???????׺????
	int hi_overflow = 0;                         //??????????Ŀ 
	int hi_number = 0;                           //?????????,????Ϊreturn?Ĳ?? 
	int hi_addition = 0;                         //??????????? 
	// int hi_ht[line / 10] = {0};                  //???????????Ĺ?ϣ??
	int *hi_ht = new int[line/10];

	for(int hi_temp = 0; hi_temp < line / 10; hi_temp++)
		addition[hi_temp] = NULL;
	for(int i = 0; i < line / 10;i ++)
		for(int j = 0; j < PP_MAX_LENGTH + 1; j++ )
			identity[i][j] = -1;
	for(int hi_i = 1; hi_i < line; hi_i++)//bug bug bug hi_i==lineʱ??omponentΪ0,ht[0]?????????ȡ??????
	{
		hi_length = name_list[hi_i]->component;                                           //??ǰ׺????Ϊ???ȣ??????ϣ??? 
		hi_bucket = (name_list[hi_i]->fp & 0xfffff) % ht[hi_length]->bucket_number;       //??ǰ׺ָ?ȡ??0λ???????ϣͰ?? 
		int hi_flag = 0;                                                                  //??ұ??λ??Ϊ0??????׺?????ڹ?ϣ???
		if(ht[hi_length]->buckets[hi_bucket]->collided == 0)                              //???Ͱ???ղ?
		{
			int hi_k = HT_BUCKET_ENTRY_NUMBER; 
			for(int hi_j = 0; hi_j < HT_BUCKET_ENTRY_NUMBER; hi_j++)
			{
				if(ht[hi_length]->buckets[hi_bucket]->entry[hi_j] != NULL)
				{
					if(ht[hi_length]->buckets[hi_bucket]->entry[hi_j]->fp == (name_list[hi_i]->fp & 0xfffff))
					{
						if(ht[hi_length]->buckets[hi_bucket]->entry[hi_j]->addr == uint64_t(& (name_list[hi_i]->name)))   //?????У??????
						{
							hi_flag = 1;                                       //??????׺?ȫ?ͬ???׺?????????????????????׺λ?
							hi_temp1 = hi_length;
							hi_temp2 = hi_bucket;
							hi_temp3 = hi_j;
							break;
						}
					}
				}
				else  
				{
					if(hi_k > hi_j)
						hi_k = hi_j;
				}
			}
			if(hi_flag == 0)                                                 //????ڲ???׺????????
			{
				ht[hi_length]->buckets[hi_bucket]->entry[hi_k] = new Hash_Entry();
				ht[hi_length]->buckets[hi_bucket]->entry[hi_k]->fp = name_list[hi_i]->fp & 0xfffff;
				ht[hi_length]->buckets[hi_bucket]->entry[hi_k]->addr = (uint64_t) &(name_list[hi_i]->name);
				hi_flag = 2;                                                                //????? 
				if(hi_k == HT_BUCKET_ENTRY_NUMBER - 1)
					ht[hi_length]->buckets[hi_bucket]->collided = 1;
			}
		//hi_number++;		                             //??Ҵ????                        	
		}//end if(ht[hi_length]->buckets[hi_bucket]->collided == 0)
		else //Ͱ?û??ղ?
		{
			for(int hi_j = 0; hi_j < HT_BUCKET_ENTRY_NUMBER; hi_j++)
			{
				if(ht[hi_length]->buckets[hi_bucket]->entry[hi_j]->fp == (name_list[hi_i]->fp & 0xfffff))
				{
					if(ht[hi_length]->buckets[hi_bucket]->entry[hi_j]->addr == uint64_t(& (name_list[hi_i]->name)))   //?????У??????
					{
						hi_flag = 1;                                                    //??????׺?ȫ?ͬ???׺?????????????????????׺λ? 
						hi_temp1 = hi_length;
						hi_temp2 = hi_bucket;
						hi_temp3 = hi_j;
						break;
					}
				}
			}
			if(hi_flag == 0)											//????ڲ???׺?????????????? 
			{
				//printf("length = %d, bucket = %d\n", hi_length, hi_bucket);
				//hi_ht[hi_overflow] = hi_length;
	            //identity[hi_length][hi_bucket] = hi_overflow;
				if((identity[hi_bucket][hi_length] >= 0) && (hi_ht[identity[hi_bucket][hi_length]] == hi_length)) //?ͬ????¶???????Ѿ??????˶??? 
				{
					int hi_m = HT_BUCKET_ENTRY_NUMBER;
					for(int hi_n = 0; hi_n < HT_BUCKET_ENTRY_NUMBER; hi_n++)
					{
						if(addition[identity[hi_bucket][hi_length]]->entry[hi_n] != NULL)
						{
							if(addition[identity[hi_bucket][hi_length]]->entry[hi_n]->fp == (name_list[hi_i]->fp & 0xfffff))
							{
								if(addition[identity[hi_bucket][hi_length]]->entry[hi_n]->addr == uint64_t(& (name_list[hi_i]->name)))   //?????У??????
								{
									hi_flag = 1;                                       //??????׺?ȫ?ͬ???׺?????????????????????׺λ?
									hi_temp1 = 0;
									hi_temp2 = identity[hi_bucket][hi_length];
									hi_temp3 = hi_n;
									break;
								}
							}
						}
						else
						{
							if(hi_m > hi_n)
								hi_m = hi_n;
						}
					}
					if(hi_flag == 0)                                                 //????ڲ???׺????????
					{
						addition[identity[hi_bucket][hi_length]]->entry[hi_m] = new Hash_Entry();
						addition[identity[hi_bucket][hi_length]]->entry[hi_m]->fp = name_list[hi_i]->fp & 0xfffff;
						addition[identity[hi_bucket][hi_length]]->entry[hi_m]->addr = (uint64_t) &(name_list[hi_i]->name);
						//if(hi_k == HT_BUCKET_ENTRY_NUMBER)
						//	ht[hi_length]->buckets[hi_bucket]->collided = 1;
						hi_flag = 3;                                                 //??????????  
					}		        
				}
				else
				{
					identity[hi_bucket][hi_length] = hi_overflow;
					hi_overflow++;                                       //???????? 
					addition[identity[hi_bucket][hi_length]] = new Hash_Bucket();
					addition[identity[hi_bucket][hi_length]]->entry[0] = new Hash_Entry();
					addition[identity[hi_bucket][hi_length]]->entry[0]->fp = name_list[hi_i]->fp & 0xfffff;
					addition[identity[hi_bucket][hi_length]]->entry[0]->addr = (uint64_t) &(name_list[hi_i]->name);
					hi_ht[identity[hi_bucket][hi_length]] = hi_length;
					hi_flag = 3;                                         //??????????  
				} 
			}	
		hi_number++;		
		}//if(ht[hi_length]->buckets[hi_bucket]->collided == 1)
		if(hi_flag == 0)
			printf("name %d insert failed!\n", hi_i);
		else if(hi_flag == 1)
			printf("name %d insert already!\n", hi_i);
	//	else if(hi_flag == 2)
	//		printf("%d insert succeed!\n", hi_i);
	//	else if(hi_flag == 3)
	//		printf("name %d insert addition!\n",hi_i);
	}      
/*	for(int i = 0; i< line/10; i++)
	{
		for(int j=0; j< 33;j++)
		{
			if(identity[i][j] != -1)
				printf("identity[%d][%d] = %d\n", i, j, identity[i][j]);
		}
	}   */
	ht[0] = new Hash_Table();
	ht[0]->bucket_number = line / 10;
	ht[0]->buckets = addition;                           
	return hi_overflow;//??????hi_overflow????߲?Ҵ??hi_number
}

int hash_table_lookup(Hash_Table_P ht[], Name_Prefix_P name_list, int identity[][PP_MAX_LENGTH + 1])
{						//???ǰ׺hl_
	int hl_low, hl_high, hl_middle, hl_flag, hl_time, hl_length, hl_len, hl_number;
	hl_low = 1;
	hl_high = 32;
	hl_length = name_list->component; 
    hl_time = 0;
    hl_len = 0;
    hl_number = 0;
	int hl_entry = 0;
	int hl_bucket = 0;
	int hl_table = 0;
    while(hl_low <= hl_high)
	{   
		hl_middle = (hl_low + hl_high) / 2;
		//printf("%d\n",hl_middle);
		if((hl_length < hl_middle) || (ht[hl_middle] == NULL))
		{		
			hl_high = hl_middle - 1;
			hl_number++;
		}
		else if((hl_length >= hl_length) && (ht[hl_middle] != NULL))
		{
			hl_flag = 0;
			char hl_name[500];
			uint64_t hl_temp, hl_hash;
		 	hl_len = cut_string(name_list->name, hl_middle);
			memcpy(hl_name, name_list->name, hl_len);
			hl_name[hl_len] = '\0';
			hl_temp = murmurHash64B(hl_name, strlen(hl_name), 0xEE6B27EB) & 0xfffff;
			hl_hash = hl_temp % ht[hl_middle]->bucket_number;		
			for(int hl_j = 0; hl_j < HT_BUCKET_ENTRY_NUMBER; hl_j++)      //??????Ͱ 
			{
				if(ht[hl_middle]->buckets[hl_hash]->entry[hl_j] != NULL)
				{
					if(ht[hl_middle]->buckets[hl_hash]->entry[hl_j]->fp == hl_temp)
					{
						if(memcmp((char *) ht[hl_middle]->buckets[hl_hash]->entry[hl_j]->addr, hl_name, hl_len) == 0)
						{
						    hl_low = hl_middle + 1;
							hl_table = hl_middle;
							hl_bucket = hl_hash;
							hl_entry = hl_j;
							hl_flag++;
							hl_time++;
							break;
						}
					}
				}
			}
			if((ht[hl_middle]->buckets[hl_hash]->collided == 1) && (hl_flag == 0))      //?????ܣ??????????Ϣ 
			{
				if(identity[hl_hash][hl_middle] != -1)
				{
					for(int hl_i = 0; hl_i < HT_BUCKET_ENTRY_NUMBER;hl_i++)
					{
						if(ht[0]->buckets[identity[hl_hash][hl_middle]]->entry[hl_i] != NULL)
						{
							if(ht[0]->buckets[identity[hl_hash][hl_middle]]->entry[hl_i]->fp == hl_temp)
							{
								if(memcmp((char *) ht[0]->buckets[identity[hl_hash][hl_middle]]->entry[hl_i]->addr, hl_name, hl_len) == 0)
								{
								    hl_low = hl_middle + 1;
									hl_table = 0;
									hl_bucket = identity[hl_hash][hl_middle];
									hl_entry = hl_i;
									hl_flag++;
									hl_time++;
									break;
								}
							}
						}
					}
				} 
			}
			if(hl_flag == 0)
				hl_high = hl_middle - 1;
			hl_number++;	
			if((hl_flag !=0) && (hl_length == hl_middle))
				break;
		}
	}
	FILE *hl_result;                                                                         //??ҽ?????? 
	hl_result = fopen("result.txt","a+");
	if(hl_time != 0)
	{
//		printf("The lookup result is entry[%d] in bucket[%d] in hash table[%d]!\n", hl_entry, hl_bucket, hl_table);  
		fprintf(hl_result, "name: %s, lookup time: %d, location ht[%d]->buckets[%d]->entry[%d]!\n", name_list->name, hl_number, hl_table, hl_bucket, hl_entry);
		fclose(hl_result);
		return 1;
	}
	else
	{
//		printf("The name %s lookup Failed!\n",name_list->name);
		fprintf(hl_result, "name: %s, lookup time: %d, match failed!\n", name_list->name, hl_number);
		fclose(hl_result);
		return -1;
	}
}

int hash_table_delete(Hash_Table_P ht[], Name_Prefix_P name_list, int identity[][PP_MAX_LENGTH + 1])
{						//???ǰ׺hd_ 
	int hd_flag = 0;
	int hd_hash;
	hd_hash = (name_list->fp & 0xfffff) % ht[name_list->component]->bucket_number;
	for(int hd_j = 0; hd_j < HT_BUCKET_ENTRY_NUMBER; hd_j++)         //??????Ͱ 
	{
		if(ht[name_list->component]->buckets[hd_hash]->entry[hd_j] != NULL)//??ϵ?ǰ???ı????
		{
			if(ht[name_list->component]->buckets[hd_hash]->entry[hd_j]->fp == (name_list->fp & 0xfffff))//??ϱ????ȡ???׺֮???????? 
			{
				if(memcmp((char *) ht[name_list->component]->buckets[hd_hash]->entry[hd_j]->addr, name_list->name, strlen(name_list->name)) == 0)//????????ȵı?ɾ??
				{
					hd_flag++;
					ht[name_list->component]->buckets[hd_hash]->entry[hd_j] = NULL;
					break;
				}
			}
		}
	}
//	if((ht[name_list->component]->buckets[hd_hash]->collided == 1) && (hd_flag != 0))
//		ht[name_list->component]->buckets[hd_hash]->collided = 0;
	if((ht[name_list->component]->buckets[hd_hash]->collided == 1) && (hd_flag == 0))//???hash??????????ԭͰ?û????ƥ????????ȥ?????Ͱ 
	{
		if(identity[hd_hash][name_list->component] != -1)//??????Ͱ?ǿ?
		{
			for(int hd_i = 0; hd_i < HT_BUCKET_ENTRY_NUMBER;hd_i++)//??????Ͱ,???Ͱ???t[0]???
			{
				if(ht[0]->buckets[identity[hd_hash][name_list->component]]->entry[hd_i] != NULL) 
				{
					if(ht[0]->buckets[identity[hd_hash][name_list->component]]->entry[hd_i]->fp == (name_list->fp & 0xfffff))
					{
						if(memcmp((char *) ht[0]->buckets[identity[hd_hash][name_list->component]]->entry[hd_i]->addr, name_list->name, strlen(name_list->name)) == 0)
						{
							hd_flag++;
							ht[name_list->component]->buckets[hd_hash]->entry[hd_i] = NULL;
							break;
						}
					}
				}
			}
		}
	}
	if(hd_flag == 0)//???ԭͰ???Ͱ??û????????????ǰ??ǰ׺?????hash????
		printf("The delete name %s is not in the hash table!\n", name_list->name);
	return 0;
} 

int hash_table_output(Hash_Table_P ht)
{
	if(ht != NULL)
		for(int i = 0; i < ht->bucket_number ;i++)
		{
			if(ht->buckets[i] != NULL)
				for(int j = 0; j < HT_BUCKET_ENTRY_NUMBER; j++)
				{
					if(ht->buckets[i]->entry[j] != NULL)
						printf("The ht->bucket[%d]->entry[%d]->fp is: %llu, full name is %s\n", i, j, ht->buckets[i]->entry[j]->fp, (char *) ht->buckets[i]->entry[j]->addr);	
				//	if(ht->buckets[i]->collided ==1)
				//		printf("The ht->bucket[%d] %llu\n", i);
				}
		}
	return 0;
}

int write_ht_information(char name[], Hash_Table_P ht[], Hash_Bucket_P addition[])
{	//???ǰ׺wh_ 
	FILE *wh_out;
	wh_out = fopen(name, "w");
	for(int wh_i = 0; wh_i < PP_MAX_LENGTH + 1; wh_i++)
	{
		if(ht[wh_i] != NULL)
			for(int wh_j = 0; wh_j < ht[wh_i]->bucket_number; wh_j ++)
			{
				if(ht[wh_i]->buckets[wh_j] != NULL)
					for(int wh_k = 0; wh_k < HT_BUCKET_ENTRY_NUMBER; wh_k++)
					{
						if(ht[wh_i]->buckets[wh_j]->entry[wh_k] != NULL)
							fprintf(wh_out, "name: %s, location is ht[%d]->buckets[%d]->entry[%d]\n", (char *)ht[wh_i]->buckets[wh_j]->entry[wh_k]->addr, wh_i, wh_j, wh_k);
						
					}
			}
	}
} 
