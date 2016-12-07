#include "bloom_filter.h"

int calculate_Bloom_Filter(Hash_Table_P ht[], Name_Prefix_P name_list)
{					//local variable prefix cb_
	//uint64_t bloom[BLOOM_SIZE];
	//for(int i = 0; i < BLOOM_SIZE; i++)
	//	bloom[i] = 0;
	uint64_t cb_hash[HASH_TIME];
	const uint32_t cb_seed[3] = {0x5F5E0F5, 0x3D14D13, 0x18EE243};
	for(int cb_j = 0; cb_j < HASH_TIME; cb_j++)//calculate bloom values for every prefix in name_list
	{
		cb_hash[cb_j] = murmurHash64B(name_list->name, strlen(name_list->name), cb_seed[cb_j]) % (32 * ht[name_list->component]->bf_number);
		int cb_m = cb_hash[cb_j] / 32;
		int cb_n = cb_hash[cb_j] % 32;
	//	printf("hash[%d] = %u, m = %d, n = %d\n", j, hash[j], m, n);
		name_list->bloom[cb_m] = name_list->bloom[cb_m] | (1 << cb_n);
	}
	return 0;
}

int calculate_Bloom_Loc(Hash_Table_P ht[], Name_Prefix_P name_list, uint64_t loc[])
{					//local variable prefix cb_
	const uint32_t cb_seed[3] = {0x5F5E0F5, 0x3D14D13, 0x18EE243};
	for(int cb_j = 0; cb_j < HASH_TIME; cb_j++)
	{
		loc[cb_j] = murmurHash64B(name_list->name, strlen(name_list->name), cb_seed[cb_j]) % (32 * ht[name_list->component]->bf_number);
	}
	return 0;
}

int merge_Bloom_Filter(Hash_Table_P ht[], Name_Prefix_P name_list[], int line)
{					//local variable prefix mb_
	for(int mb_i = 1; mb_i <= line; mb_i++)
	{
		for(int mb_j = 0; mb_j < ht[name_list[mb_i]->component]->bf_number; mb_j++)
		{
			if(name_list[mb_i]->bloom[mb_j] != 0)
			{
				ht[name_list[mb_i]->component]->bloom[mb_j] = ht[name_list[mb_i]->component]->bloom[mb_j] | name_list[mb_i]->bloom[mb_j];
			//	printf("i = %d, j=  %d\n",mb_i, mb_j);
			}
		}
	}
	return 0;
}

int lookup_Bloom_Filter(Hash_Table_P ht[], Name_Prefix_P name_list, int identity[][PP_MAX_LENGTH + 1])
{					//local variable prefix lb_
	int lb_low, lb_high, lb_middle, lb_length, lb_len, lb_flag, lb_back, lb_real, lb_bf, lb_time, lb_temp;
	int lb_left[BF_DEEP] = {0};
	int lb_right[BF_DEEP] = {0};
	lb_low = 1;
	lb_high = 32;
	lb_length = name_list->component;
	int lb_entry = 0;
	int lb_bucket = 0;
	int lb_table = 0;
	lb_real = 0;
	lb_back = 0;
	uint64_t loc[HASH_TIME];

	FILE *lb_out = fopen("result.txt","a+");
	if(lb_out == NULL){
		printf("res file open error!\n");
		exit(1);
	}

    while(lb_low <= lb_high)
    {
		lb_middle = (lb_low + lb_high) / 2;

		if(ht[lb_middle] == NULL)//hash table is empty then go left, but what if bloom filter is not empty?
		{
			lb_high = lb_middle - 1;
			continue;
		}
		if(lb_length < lb_middle)
		{
			lb_high = lb_middle - 1;
			if( (lb_low > lb_high) && (lb_back == 0) && (lb_real > 0) )
			{
				lb_low = lb_left[lb_real];
				lb_high = lb_right[lb_real];
				lb_real--;
			}
		}
		else
		{
			lb_flag = 0;
			lb_bf = 0;

//***bloom filter***
			Name_Prefix_P lb_tp = new Name_Prefix();
			lb_tp->bloom[BLOOM_SIZE] = {0};
		 	lb_len = cut_string(name_list->name, lb_middle);
			lb_tp->component = lb_middle;
			memcpy(lb_tp->name, name_list->name, lb_len);
			lb_tp->name[lb_len] = '\0';
			lb_tp->fp = murmurHash64B(lb_tp->name, strlen(lb_tp->name), 0xEE6B27EB) & 0xfffff;
			if(ht[lb_middle]->bf_number != 0)
			{
				calculate_Bloom_Loc(ht, lb_tp, loc);//calculate name's bloom value
				lb_time = 0;//times of check pass                                                 
				lb_temp = 0;//times of check
				for(int lb_i = 0; lb_i < HASH_TIME; lb_i++)//check bloom value
				{
					lb_temp++;
					if( ( ((uint32_t)1<<(loc[lb_i]%32)) & ht[lb_middle]->bloom[loc[lb_i]/32] )!= 0)
					{
						lb_time++;
					}
					if( lb_time != lb_temp )
					{
						break;
					}
				}
				if((lb_time == lb_temp) && (lb_time <= HASH_TIME)) //result of BF check
				{
					lb_real++;
					lb_bf++;
					if(lb_real < BF_DEEP)
					{
						lb_left[lb_real] = lb_low;
						lb_right[lb_real] = lb_middle - 1;
					}
					else
						lb_real--;
					lb_low = lb_middle + 1;
				}
			}

//***hash lookup***
			uint64_t lb_hash;
			lb_hash = lb_tp->fp % ht[lb_middle]->bucket_number;
			for(int lb_j = 0; lb_j < HT_BUCKET_ENTRY_NUMBER; lb_j++)
			{
				if(ht[lb_middle]->buckets[lb_hash]->entry[lb_j] != NULL)
				{
					if(ht[lb_middle]->buckets[lb_hash]->entry[lb_j]->fp == lb_tp->fp)
					{
						if(memcmp((char *) ht[lb_middle]->buckets[lb_hash]->entry[lb_j]->addr, lb_tp->name, lb_tp->component) == 0)
						{
							lb_table = lb_middle;
							lb_bucket = lb_hash;
							lb_entry = lb_j;
							lb_flag = 1;//marking that current hash lookup succeed
							lb_back++;//marking that the back-trip-layer add 1 layer
							lb_low = lb_middle + 1;//go right kid
							break;
						}
					}
				}
			}
			if((lb_flag == 0) && (ht[lb_middle]->buckets[lb_hash]->collided == 1))//if lookup failed, try overflow bucket
			{
				if(identity[lb_hash][lb_middle] != -1)
				{
					for(int lb_k = 0; lb_k < HT_BUCKET_ENTRY_NUMBER;lb_k++)
					{
						if(ht[0]->buckets[identity[lb_hash][lb_middle]]->entry[lb_k] != NULL)
						{
							if(ht[0]->buckets[identity[lb_hash][lb_middle]]->entry[lb_k]->fp == lb_tp->fp)
							{
								if(memcmp((char *) ht[0]->buckets[identity[lb_hash][lb_middle]]->entry[lb_k]->addr, lb_tp->name, lb_tp->component) == 0)
								{
								    lb_low = lb_middle + 1;
									lb_table = 0;
									lb_bucket = identity[lb_hash][lb_middle];
									lb_entry = lb_k;
									lb_flag = 1;
									lb_back++;
									break;
								}
							}
						}
					}
				}
			}

//***result analyse***
			if((lb_flag !=0) && (lb_length == lb_middle))                                  //mark that a exact-match happen
				break;
			else if(lb_flag != 0)                                                          //hash lookup succeed, then go to right kid
				lb_low = lb_middle + 1;
			else if((lb_flag == 0) && (lb_bf != 0))                                        //bloom check pass but hash lookup failed, then go to right kid
				lb_low = lb_middle + 1;
			else if((lb_flag == 0) && (lb_bf == 0))                                        //both hash lookup and bloom check failed, then go to left kid
				lb_high = lb_middle - 1;

			if((lb_low > lb_high) && (lb_back == 0) && (lb_real > 0))                      //���ֽ���һ�׶κ󣬹�ϣ����ʧ�ܣ����������ֹ�BF���ҳɹ�
			{
				lb_low = lb_left[lb_real];
				lb_high = lb_right[lb_real];
				lb_real--;
			}
		}//end else
	}//while(low <= high)
	
	if(lb_back != 0)
	{
		fprintf(lb_out, "Lookup the name %s succeed, it is in hash[%d]->buckets[%d]->entry[%d]!\n", name_list->name, lb_table, lb_bucket, lb_entry);
		fclose(lb_out);
		return 1;
	}
	else
	{
		fprintf(lb_out, "Lookup the name %s failed!\n", name_list->name);
		fclose(lb_out);
		return -1;
	}
}