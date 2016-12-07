#include "preprocess.h"

int count_component(char name[])			//��¼���š�/�����ֵĴ�����Ҳ���Է�����������ǰ׺�ĳ���cc_length
{                   //����ǰ׺Ϊcc_
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

uint64_t murmurHash64B (const void * key, int len, uint32_t seed)             //��Դ������http://blog.csdn.net/wisage/article/details/7104866
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
{                                     //����ǰ׺Ϊlp_
	FILE *lp_in, *lp_out;
	int lp_line=1;
	char lp_temp[5000];
	lp_in = fopen(name,"r");
	length[0] = 0;                    //��ʼΪ1
	if(lp_in == NULL){
		printf("read file open error!\n");
		exit(1);
	}

	while(feof(lp_in) == 0)
	{
		memset(lp_temp,0,5000*sizeof(char));
		fgets(lp_temp, 5000, lp_in);
		lp_temp[strlen(lp_temp)-1]='\0';
		if(strlen(lp_temp) == 0)//avoid loading the last line that is empty
			break;

		name_list[lp_line] = new Name_Prefix();
// ************************************************************************
// Ԥ��������ǰ׺���洢����������Ϣ�������ֿ���������ָ�ƣ�����BFֵ
		sscanf(lp_temp, "%s", name_list[lp_line]->name);
		name_list[lp_line]->component = count_component(name_list[lp_line]->name);
		name_list[lp_line]->fp = murmurHash64B(name_list[lp_line]->name, strlen(name_list[lp_line]->name), 0xEE6B27EB);
	//	calculate_Bloom_Filter(name_list[line]);
// ************************************************************************
		length[name_list[lp_line]->component]++;
		if(length[name_list[lp_line]->component] > length[0])                           //length[0]��¼����ֵ
			length[0] = length[name_list[lp_line]->component];
		lp_line++;
	}
	fclose(lp_in);

	return lp_line - 1;              
}

int process_middle_prefix(char input[], char output[])                   //��������ǰ׺���ڶ��ֲ��ҵ��м��ڵ����ɶ�Ӧ���ȵ�����ǰ׺ 
{									//����ǰ׺Ϊpm_
	FILE *pm_in, *pm_out;
	char pm_read[500];
	char pm_write[500];
	char pm_temp[500];
	int pm_low, pm_high, pm_middle, pm_length, pm_len;
	int pm_pre_num=0;
	pm_in = fopen(input, "r");
	pm_out = fopen(output, "w");

	if(pm_in==NULL || pm_out==NULL){
		printf("in or out file open error!\n");
		exit(1);
	}

	memset(pm_write,0,500*sizeof(char));

	while(feof(pm_in) == 0)
	{
		pm_low = 1;
		pm_high = 32;
		pm_len = 0;
		fgets(pm_read, 500, pm_in);//������Ҫȥ���ո��Ĳ���������ÿһ������name�����ɱ���ǰ׺��������һ������pre.txt��ǰ׺ĩβ��������\n\n�����º���prefix load������һ�����У������ڲ�ѯ��ʱ����������ָ�루���г���Ϊ0����ȥ����ht[0]�����Ǵ�����ht[0]������Ͱ������û�з���������������Ͱ�ǿ�ָ�룬�����ֶδ�����
		pm_read[strlen(pm_read)-1]='\0';//ȥ�����з�

		pm_length = count_component(pm_read);//ͳ�Ƶ�ǰ������name�ֿ���
	    while(pm_low <= pm_high)
	    {
	    	pm_middle = (pm_low + pm_high) / 2;
	    	if(pm_length < pm_middle)//���ֿ���С�ڵ�ǰ����(pm_low~pm_high)������ֵ,���������ұ߽�-1
	    		pm_high = pm_middle - 1;//������������
	    	else//�ֿ������ڵ��ڵ�ǰ��������ֵ
	    	{
	    		pm_len = cut_string(pm_read, pm_middle);
	    		memcpy(pm_write, pm_read, pm_len);//��ȡ��ǰnameǰpm_middle���ֿ�
	    		//if(pm_length != pm_middle)                    //������ֻ�����м��ڵ㣬���θþ䣬���õ�ԭ�нڵ����м��ڵ�
	    		if( pm_write[0]!='\n' || pm_write[0]!=0 ){
	    			if(pm_len < strlen(pm_read)){
	    				fprintf(pm_out,"%s\n",pm_write);
	    				pm_pre_num+=1;
	    			}
	    		}

	    		memset(pm_write,0,500*sizeof(char));
	    		pm_low = pm_middle + 1;//������������
			}
		}
		memset(pm_read, 0, 500*sizeof(char));
	}
	fclose(pm_in);
	fclose(pm_out);
	return pm_pre_num;
}

int cut_string(char name[], int n)			//��¼ǰn�������ĳ���,name��ʽΪ/a/b/c/d...
{							//����ǰ׺cs_
	char *cs_p;
	cs_p = name;
	int cs_number = 0;
	int cs_length = 0;
	while(*cs_p != NULL)
//	while(*cs_p)
	{
		if(*cs_p == 47)//��б��'/'
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

int clr_Name_Prefix(Name_Prefix_P item)
{
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
