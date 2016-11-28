#include "bloom_filter.h"

int calculate_Bloom_Filter(Hash_Table_P ht[], Name_Prefix_P name_list)
{					//参数前缀cb_ 
	//uint64_t bloom[BLOOM_SIZE];
	//for(int i = 0; i < BLOOM_SIZE; i++)
	//	bloom[i] = 0;
	uint64_t cb_hash[HASH_TIME];
	const uint32_t cb_seed[3] = {0x5F5E0F5, 0x3D14D13, 0x18EE243};
	for(int cb_j = 0; cb_j < HASH_TIME; cb_j++) 
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
{					//参数前缀cb_ 
	const uint32_t cb_seed[3] = {0x5F5E0F5, 0x3D14D13, 0x18EE243};
	for(int cb_j = 0; cb_j < HASH_TIME; cb_j++) 
	{
		loc[cb_j] = murmurHash64B(name_list->name, strlen(name_list->name), cb_seed[cb_j]) % (32 * ht[name_list->component]->bf_number);
	}
	return 0; 
}

int merge_Bloom_Filter(Hash_Table_P ht[], Name_Prefix_P name_list[], int line)
{					//参数前缀mb_ 
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
{					//参数前缀lb_ 
	int lb_low, lb_high, lb_middle, lb_length, lb_len, lb_flag, lb_back, lb_real, lb_bf, lb_time, lb_temp;
	int lb_left[BF_DEEP] = {0};
	int lb_right[BF_DEEP] = {0};
	lb_low = 1;
	lb_high = 32;
	lb_length = name_list->component; 
	//printf("length = %d\n", length);
	int lb_entry = 0;
	int lb_bucket = 0;
	int lb_table = 0;
	lb_real = 0;
	lb_back = 0;
	uint64_t loc[HASH_TIME];
	
    while(lb_low <= lb_high)
	{   
		lb_middle = (lb_low + lb_high) / 2;	
		//printf("middle = %d\n",lb_middle);
		if(ht[lb_middle] == NULL)
		{
			lb_high = lb_middle - 1;
			continue;
		}
		if(lb_length < lb_middle)	
		{
			lb_high = lb_middle - 1;
			if((lb_low > lb_high) && (lb_back == 0) && (lb_real > 0))    //二分结束一阶段后，哈希查找失败，但曾经出现过BF查找成功 
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
			
// ************************************************************************ 
// 产生对应长度的命名前缀 
			Name_Prefix_P lb_tp = new Name_Prefix();
			lb_tp->bloom[BLOOM_SIZE] = {0};
		 	lb_len = cut_string(name_list->name, lb_middle);
			lb_tp->component = lb_middle;
			memcpy(lb_tp->name, name_list->name, lb_len);
			lb_tp->name[lb_len] = '\0';
			lb_tp->fp = murmurHash64B(lb_tp->name, strlen(lb_tp->name), 0xEE6B27EB) & 0xfffff;
			if(ht[lb_middle]->bf_number != 0)
			{
//				calculate_Bloom_Filter(ht, lb_tp); //计算完整的布鲁姆过滤器			
				
				calculate_Bloom_Loc(ht, lb_tp, loc);//modify modify modify				
			
// ************************************************************************             
			
			
// ************************************************************************ 
// BF过滤 
				lb_time = 0;                                                 //3次哈希都匹配，布鲁姆匹配才成功 
				lb_temp = 0;
	//*************************************************************************
	//BF过滤 original 
	//				for(int lb_i = 0; lb_i < ht[lb_middle]->bf_number; lb_i++)
	//				{                                                            //3次匹配会只有2个值吗？ 
	//					if(lb_tp->bloom[lb_i] !=0)
	//					{
	//						lb_temp++;
	//						if((lb_tp->bloom[lb_i] & ht[lb_middle]->bloom[lb_i]) != 0)
	//					        lb_time++;
	//					}
	//					if((lb_time != lb_temp) || (lb_time == HASH_TIME))
	//						break;
	//				}
	//end ori
	//*************************************************************************

	//*************************************************************************
	//modify modify modify BF过滤  
					for(int lb_i = 0; lb_i < HASH_TIME; lb_i++)//三次哈希对应bloom过滤器位置作比较 
					{
						lb_temp++;
						if( (((uint32_t)1<<(loc[lb_i]%32)) & ht[lb_middle]->bloom[loc[lb_i]/32]) != 0)
						    lb_time++;						
						if( (lb_time != lb_temp) || (lb_time == HASH_TIME) )
							break;
					}
	//end modify
	//*************************************************************************
				if((lb_time == lb_temp) && (lb_time <= HASH_TIME))                                      //BF匹配成功，继续二分查找 
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
// **************************************************************************** 

// ************************************************************************ 
// 哈希查找 
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
							lb_flag = 1; 
							lb_back++;
							lb_low = lb_middle + 1;                      
							break;
						}
					}
				}
			}
			if((lb_flag == 0) && (ht[lb_middle]->buckets[lb_hash]->collided == 1))
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
			 
// ************************************************************************	
//			printf("flag = %d, bf = %d, real = %d\n", flag, bf, real);
			
// ************************************************************************		
//BF与哈希查找结果
			if((lb_flag !=0) && (lb_length == lb_middle))
				break;
			else if(lb_flag != 0)                                                            //哈希查找成功 
				lb_low = lb_middle + 1; 
			else if((lb_flag == 0) && (lb_bf != 0))                                        //哈希查找失败，BF查找成功                       
				lb_low = lb_middle + 1;
			else if((lb_flag == 0) && (lb_bf == 0))                                        //哈希查找失败, BF查找失败 
				lb_high = lb_middle - 1;

			if((lb_low > lb_high) && (lb_back == 0) && (lb_real > 0))                           //二分结束一阶段后，哈希查找失败，但曾经出现过BF查找成功 
			{
				lb_low = lb_left[lb_real];
				lb_high = lb_right[lb_real];
				lb_real--;
			}

		}
	}//while(low <= high)
// ************************************************************************	
	
	if(lb_back != 0)		
	{
//		printf("Lookup the name %s succeed, it is in hash[%d]->buckets[%d]->entry[%d]!\n", name_list->name, table, bucket, entry);
		return 1;
	}
	else
	{
//		printf("Lookup the name %s failed!\n", name_list->name);
		return -1;
	}		
}
