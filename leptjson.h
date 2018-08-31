#ifndef __LEPTJSON_H__
#define __LEPTJSON_H__

//联合，一次只能表示一种类型
typedef enum { LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER, LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT } lept_type;

enum {
    LEPT_PARSE_OK = 0,
    LEPT_PARSE_EXPECT_VALUE,
    LEPT_PARSE_INVALID_VALUE,
    LEPT_PARSE_ROOT_NOT_SINGULAR,
    LEPT_PARSE_NUMBER_TOO_BIG
};

typedef struct {
    double n;
    lept_type type;
}lept_value;

typedef struct {
    const char* json;
}lept_context;


lept_type lept_get_type(const lept_value* v);
double lept_get_number(const lept_value* v);

int lept_parse(lept_value* v, const char* json);
    
#endif