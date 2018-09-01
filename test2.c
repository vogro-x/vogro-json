// test2.c :show usage
#include "leptjson.h"
#include <stdio.h>

int main(void){

    lept_value v1;
    lept_init(&v1);
    lept_parse(&v1,"null");
    if(lept_get_type(&v1)==LEPT_NULL){
        printf("parse null ok\n");
    }
    lept_free(&v1);

    lept_value v2;
    lept_init(&v2);
    lept_parse(&v2,"true");
    if(lept_get_type(&v2)==LEPT_TRUE){
        printf("parse true ok\n");
    }
    lept_free(&v1);

    lept_value v3;
    lept_init(&v3);
    lept_parse(&v3,"false");
    if(lept_get_type(&v3)==LEPT_FALSE){
        printf("parse false ok\n");
    }
    lept_free(&v3);

    lept_value v4;
    lept_init(&v4);
    lept_parse(&v4,"1.23");
    if(lept_get_type(&v4)==LEPT_NUMBER){
        printf("parse number ok\n");
        double val = lept_get_number(&v4);
        printf("result:%lf\n",val+2.2);
    }
    lept_free(&v3);

    lept_value v5;
    lept_init(&v5);
    lept_parse(&v5,"[1,2,3]");
    if(lept_get_type(&v5)==LEPT_ARRAY){
        printf("parse array ok\n");
        size_t val = lept_get_array_size(&v5);
        printf("array size:%lu\n",val);

        int ele1=lept_get_array_element(&v5,0)->u.n;
        int ele2=lept_get_array_element(&v5,1)->u.n;
        int ele3=lept_get_array_element(&v5,2)->u.n;
         printf("%d,%d,%d",ele1,ele2,ele3);
    }
    lept_free(&v5);
    return 0;
}