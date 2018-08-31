// test2.c :show usage
#include "leptjson.h"
#include <stdio.h>

int main(void){
    lept_value v1;
    lept_parse(&v1,"null");
    if(lept_get_type(&v1)==LEPT_NULL){
        printf("parse null ok\n");
    }

    lept_value v2;
    lept_parse(&v2,"true");
    if(lept_get_type(&v2)==LEPT_TRUE){
        printf("parse true ok\n");
    }

    lept_value v3;
    lept_parse(&v3,"false");
    if(lept_get_type(&v3)==LEPT_FALSE){
        printf("parse false ok\n");
    }

    lept_value v4;
    lept_parse(&v4,"1.23");
    if(lept_get_type(&v4)==LEPT_NUMBER){
        printf("parse number ok\n");
        double val = lept_get_number(&v4);
        printf("result:%lf\n",val+2.2);
    }

    return 0;
}