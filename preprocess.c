#include "preprocess.h"

int count_component(char name[])			//记录符号“/”出现的次数，也可以返回整个命名前缀的长度cc_length 
{                   //参数前缀为cc_ 
	char *cc_p,*cc_q;
	cc_p = name;
	int cc_component = 0;
	int cc_length = 0;
	while(*cc_p != '\0')
	{
		if(*cc_p == 47)
		{
			cc_component++;
		}
		cc_length++;
		cc_p++;
	}
	return cc_component;
} 

uint64_t murmurHash64B (const void * key, int len, uint32_t seed)             //来源于网上http://blog.csdn.net/wisage/article/details/7104866
{
	const uint32_t m = 0x5bd1e995;
	const int r = 24;
 
	uint32_t h1 = seed ^ len;
	uint32_t h2 = 0;
 
	const uint32_t * data = (const uint32_t *)key;
 
	while(len >= 8)
	{
		uint32_t k1 = *data++;
		k1 *= m; k1 ^= k1 >> r; k1 *= m;
		h1 *= m; h1 ^= k1;
		len -= 4;
 
		uint32_t k2 = *data++;
		k2 *= m; k2 ^= k2 >> r; k2 *= m;
		h2 *= m; h2 ^= k2;
		len -= 4;
	}
 
	if(len >= 4)
	{
		uint32_t k1 = *data++;
		k1 *= m; k1 ^= k1 >> r; k1 *= m;
		h1 *= m; h1 ^= k1;
		len -= 4;
	}
 
	switch(len)
	{
	case 3: h2 ^= ((unsigned char*)data)[2] << 16;
	case 2: h2 ^= ((unsigned char*)data)[1] << 8;
	case 1: h2 ^= ((unsigned char*)data)[0];
			h2 *= m;
	};
 
	h1 ^= h2 >> 18; h1 *= m;
	h2 ^= h1 >> 22; h2 *= m;
	h1 ^= h2 >> 17; h1 *= m;
	h2 ^= h1 >> 19; h2 *= m;
 
	uint64_t h = h1;
 
	h = (h << 32) | h2;
 
	return h;
}

int load_prefixes(char name[], Name_Prefix_P name_list[], int length[])
{                                     //参数前缀为lp_ 
	FILE *lp_in, *lp_out;
	int lp_line;
	char lp_temp[5000];
	lp_in = fopen(name,"r");
	length[0] = 0;
	lp_line = 1;                      //初始为1 
	while(feof(lp_in) == 0)
	{
		memset(lp_temp,0,5000*sizeof(char));
		fgets(lp_temp, 5000, lp_in);
		
//		if(lp_temp[0]==0 || lp_temp[0]=='\n')//bug bug bug 读入空行导致错误，需要跳过。空行出现原因尚不清楚 
//			continue; 
			
		name_list[lp_line] = new Name_Prefix();
// ************************************************************************
// 预处理命名前缀，存储完整命名信息，命名字块数，命名指纹，命名BF值 
		sscanf(lp_temp, "%s", name_list[lp_line]->name);
		name_list[lp_line]->component = count_component(name_list[lp_line]->name);
		name_list[lp_line]->fp = murmurHash64B(name_list[lp_line]->name, strlen(name_list[lp_line]->name), 0xEE6B27EB);
	//	calculate_Bloom_Filter(name_list[line]);
// ************************************************************************	
		length[name_list[lp_line]->component]++;
		if(length[name_list[lp_line]->component] > length[0])                           //length[0]记录最大值 
			length[0] = length[name_list[lp_line]->component]; 
		lp_line++;
	}
	fclose(lp_in);
	
// ************************************************************************	
//输出关于字块数的统计结果	
//	for(int lp_i = 1; lp_i <= PP_MAX_LENGTH; lp_i++)
//		printf("The number of length %d name is %d\n", lp_i, length[lp_i]);
// ************************************************************************			
		
	return lp_line - 1;              //初始为1 
}

int process_middle_prefix(char input[], char output[])                   //根据命名前缀，在二分查找的中间节点生成对应长度的命名前缀 
{									//参数前缀为pm_ 
	FILE *pm_in, *pm_out;
	char pm_read[500];
	char pm_write[500];
	char pm_temp[500];
	int pm_low, pm_high, pm_middle, pm_length, pm_len;
	pm_in = fopen(input, "r");
	pm_out = fopen(output, "w");
	
	memset(pm_write,0,500*sizeof(char));
	
	while(feof(pm_in) == 0)
	{
		pm_low = 1; 
		pm_high = 32;
		pm_len = 0;
		fgets(pm_read, 500, pm_in);//这里需要去掉空格的操作，否则每一个读入name再生成标记前缀后，最后一个放入pre.txt的前缀末尾会有两个\n\n，导致后面prefix load会载入一个空行，进而在查询的时候会读到空指针（空行长度为0，会去调用ht[0]，但是代码中ht[0]是溢出桶，如果没有发生溢出，则溢出桶是空指针，会出现段错误） 
		pm_read[strlen(pm_read)-1]='\0';//去掉换行符 
		
		pm_length = count_component(pm_read);//统计当前读入的name字块数 
	    while(pm_low <= pm_high)
	    {
	    	pm_middle = (pm_low + pm_high) / 2;
	    	if(pm_length < pm_middle)//若字块数小于当前区域(pm_low~pm_high)的中心值,则将区域右边界-1
	    		pm_high = pm_middle - 1;//移向左子区间 
	    	else//字块数大于等于当前区域中心值 
	    	{
	    		pm_len = cut_string(pm_read, pm_middle);
	    		memcpy(pm_write, pm_read, pm_len);//截取当前name前pm_middle个字块 
//	    		pm_write[pm_len] = '\0';//接上结束符 
	    		//if(pm_length != pm_middle)                    //不等于只输出中间节点，屏蔽该句，则得到原有节点加中间节点 
	    		if(pm_write[0]!='\n' || pm_write[0]!=0)
	    			fprintf(pm_out, "%s\n", pm_write);//输出到output指向的文件 
//	    		printf("insert:%s\n",pm_write);
//	    		getchar();
	    		memset(pm_write,0,500*sizeof(char));
	    		pm_low = pm_middle + 1;//移向右子区间 
			}
		}
	}
	fclose(pm_in);
	fclose(pm_out);
	return 0;
}

int cut_string(char name[], int n)			//记录前n个组件的长度,name形式为/a/b/c/d...
{							//参数前缀cs_ 
	char *cs_p;
	cs_p = name;
	int cs_number = 0;
	int cs_length = 0;
	while(*cs_p != NULL)
//	while(*cs_p)
	{
		if(*cs_p == 47)//反斜杠'/' 
		{
			if(cs_number < n)
				cs_number++;
			else
				break;
		}
		cs_p++;
		cs_length++;
//		if(cs_number == n)
//			break;
	}
	return cs_length;
} 

int clr_Name_Prefix(Name_Prefix_P item){
	int i=0;
	item->component=0;
	item->fp=0;
	for(i=0; i<1000; i++){
		if(i<500){
			item->name[i]=0;
		}
		item->bloom[i]=0;
	}
} 
