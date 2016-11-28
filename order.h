#ifndef ORDER_H
#define ORDER_H

#include "stdio.h"
#include "stdlib.h" 
#include "string.h"
#include "inttypes.h"
#include "assert.h"

#include "preprocess.h"
#include "hash_table.h"

int hash_table_lookup_order(Hash_Table_P ht[], Name_Prefix_P name_list, int identity[][PP_MAX_LENGTH + 1]);     //π˛œ£±Ì≤È’“


#endif
