#include "preprocess.h"
#include "hash_table.h"
#include "order.h"
#include "bloom_filter.h"
#include "time.h"
#include <stdlib.h>
#include <string.h>

Name_Prefix_P name_list[240000];
int length[PP_MAX_NUMBER + 1] = {0};

int main(int argc, char *argv[])//input parameters : initial rules_set file, preprocess rules file, test data file
{
	if(argc < 4){
		printf("number of parameters error!\n");
		exit(1);
	}

	Hash_Table_P ht[PP_MAX_LENGTH + 1];
	process_middle_prefix(argv[1], argv[2] );
	int line = load_prefixes(argv[1] ,name_list ,length );
	printf("done!\n");
	for(int i = 1; i <= PP_MAX_LENGTH; i++)
	{
		if(length[i] != 0)
			hash_table_init(ht[i], i, length[i] / 3);		
		else
			ht[i] = NULL;			
	}

	Hash_Bucket_P *addition1 = new Hash_Bucket_P[line/10]; 
	int identity1[10000][33];
	for(int i = 0; i < PP_MAX_LENGTH + 1;i ++)
		for(int j = 0; j < line / 10; j++ )
			identity1[i][j] = -1;
	
	int tp = hash_table_insert(ht, name_list, line, addition1, identity1); 
	printf("%d\n",tp);	

	//hash lookup (without BF) test
	FILE *test_in = fopen(argv[3],"r");
	char ti_temp[5000]="";
	int test_line=0;
	Name_Prefix_P test_name;
	while(!feof(test_in)){
		fgets(ti_temp,5000,test_in);
		test_name = new Name_Prefix();
		sscanf(ti_temp,"%s", test_name->name);//name initial		
		test_name->component = count_component(test_name->name);//number of component initial
		test_name->fp = murmurHash64B(test_name->name, strlen(test_name->name), 0xEE6B27EB);//footprint initial
		hash_table_lookup(ht, test_name, identity1);
		delete test_name;
	}
	return 0;
}
