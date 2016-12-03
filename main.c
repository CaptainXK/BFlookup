#include "preprocess.h"
#include "hash_table.h"
#include "order.h"
#include "bloom_filter.h"
#include "time.h"

Name_Prefix_P name_list[240000];
int length[PP_MAX_NUMBER + 1] = {0};
int main(int argc ,char *argv[])
{
	char name2[] = "pre.txt";
	if(argc < 3){
		printf("please input all data file needed\n");
		exit(1);
	}

	char *name1 = argv[1];
	// char *name2 = argv[2];
	Hash_Table_P ht[PP_MAX_LENGTH + 1];
	int line=0;
	line = process_middle_prefix(name1,name2);
	printf("%d prefixes in total\n",line);
	load_prefixes(name2, name_list, length, line);

	for(int i = 1; i <= PP_MAX_LENGTH; i++)
	{
		if(length[i] != 0)
			// hash_table_init(ht[i], i, length[i] / 3);
			hash_table_withBF_init(ht[i], i, length[i]/3, 1000);//default bf_number is 1000
		else
			ht[i] = NULL;
	}

	for(int i=1; i<=line; i++){
		calculate_Bloom_Filter(ht, name_list[i]);
	}

	Hash_Bucket_P *addition1 = new Hash_Bucket_P[line/10];
	int identity1[10000][33];
	for(int i = 0; i < PP_MAX_LENGTH + 1;i ++)
		for(int j = 0; j < line / 10; j++ )
			identity1[i][j] = -1;

	int tp = hash_table_insert(ht, name_list, line, addition1, identity1);
	printf("%d\n",tp);
	merge_Bloom_Filter(ht, name_list, line);
	
	// //hash lookup test (without BF)
	// FILE *test_in;
	// test_in = fopen(argv[2],"r");
	// if(test_in == NULL){
	// 	printf("test_in file open error!\n");
	// 	exit(1);
	// }
	// char ti_temp[5000] = "";
	// int test_line=0;
	// Name_Prefix_P test_name = new Name_Prefix();
	// while(!feof(test_in)){
	// 	fgets(ti_temp,5000,test_in);
	// 	sscanf(ti_temp,"%s", test_name->name);//name buf clr
	// 	test_name->component = count_component(test_name->name);//nubmber of component initial
	// 	test_name->fp = murmurHash64B(test_name->name, strlen(test_name->name), 0xEE6B27EB);//footprint initial
	// 	hash_table_lookup(ht, test_name, identity1);
	// 	clr_Name_Prefix(test_name);
	// }

	// fclose(test_in);

	return 0;
}
