// test2.c :show usage
#include "leptjson.h"
#include <stdio.h>
#include <assert.h>

int main(void) {

    lept_value v1;
    lept_init(&v1);
    lept_parse(&v1, "null");
    if (lept_get_type(&v1) == LEPT_NULL) {
        printf("parse null ok\n");
    }
    lept_free(&v1);

    lept_value v2;
    lept_init(&v2);
    lept_parse(&v2, "true");
    if (lept_get_type(&v2) == LEPT_TRUE) {
        printf("parse true ok\n");
    }
    lept_free(&v1);

    lept_value v3;
    lept_init(&v3);
    lept_parse(&v3, "false");
    if (lept_get_type(&v3) == LEPT_FALSE) {
        printf("parse false ok\n");
    }
    lept_free(&v3);

    lept_value v4;
    lept_init(&v4);
    lept_parse(&v4, "1.23");
    if (lept_get_type(&v4) == LEPT_NUMBER) {
        printf("parse number ok\n");
        double val = lept_get_number(&v4);
        printf("result:%lf\n", val + 2.2);
    }
    lept_free(&v3);

    lept_value v5;
    lept_init(&v5);
    lept_parse(&v5, "[1,2,3]");
    if (lept_get_type(&v5) == LEPT_ARRAY) {
        printf("parse array ok\n");
        size_t val = lept_get_array_size(&v5);
        printf("array size:%lu\n", val);

        int ele1 = lept_get_array_element(&v5, 0)->u.n;
        int ele2 = lept_get_array_element(&v5, 1)->u.n;
        int ele3 = lept_get_array_element(&v5, 2)->u.n;
        printf("%d,%d,%d\n", ele1, ele2, ele3);
    }
    lept_free(&v5);

    lept_value v6;
    lept_init(&v6);
    lept_parse(&v6, "{\"a\":1,\"b\":[{\"m\":\"andrew\",\"id\":25}]}");
    if (lept_get_type(&v6) == LEPT_OBJECT) {
        printf("parse object ok\n");
        size_t obj_size = lept_get_object_size(&v6);
        printf("%lu\n", obj_size);
        const char *key0 = lept_get_object_key(&v6, 0);
        const char *key1 = lept_get_object_key(&v6, 1);
        lept_value *vp0 = lept_get_object_value(&v6, 0);
        lept_value *vp1 = lept_get_object_value(&v6, 1);
        assert(lept_get_type(vp0) == LEPT_NUMBER);
        assert(lept_get_type(vp1) == LEPT_ARRAY);
        double number = lept_get_number(vp0);
        printf("%s:%lf\n", key0, number);

        size_t array_size = lept_get_array_size(vp1);
        assert(array_size == 1);
        lept_value *vp3 = lept_get_array_element(vp1, 0);
        assert(lept_get_type(vp3) == LEPT_OBJECT);
    }
    lept_free(&v6);
    return 0;
}