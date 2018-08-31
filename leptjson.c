#include "leptjson.h"
#include <stdlib.h>
#include <assert.h>

#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')
#define EXPECT(c, ch) do { assert(*c->json == (ch)); c->json++; } while(0)

lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value* v){
    assert(v!=NULL && v->type==LEPT_NUMBER);
    return v->n;
}

static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

/* null  = "null" */
static int lept_parse_null(lept_context* c, lept_value* v) {
    EXPECT(c, 'n');
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_NULL;
    return LEPT_PARSE_OK;
}

/* ture = "true" */
static int lept_parse_true(lept_context* c, lept_value* v) {
    EXPECT(c, 't');
    if(c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e') 
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_TRUE;
    return LEPT_PARSE_OK;
}

/* false = "false" */
static int lept_parse_false(lept_context* c, lept_value* v) {
    EXPECT(c, 'f');
    if(c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e') 
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 4;
    v->type = LEPT_FALSE;
    return LEPT_PARSE_OK;
}

static int lept_parse_number(lept_context* c, lept_value* v) {
    char* end;
    /* \TODO validate number */

    // 0-9
    if(ISDIGIT(c->json[0])||c->json[0]=='-'){
        if(c->json[0]=='-'){
            c->json+=1;
        }

        if(c->json[0]=='0' && c->json[1]!='.'){
            return LEPT_PARSE_INVALID_VALUE;
        }

        //循环直到小数点
        while(c->json[0] != '.' &&c->json[0]!='E'&&c->json[0]!='e'&&c->json[0]!='\0'){
            if(!ISDIGIT(c->json[0])){
                return LEPT_PARSE_INVALID_VALUE;
            }
            c->json += 1;
        }

        if(c->json[0]=='E'&&c->json[0]=='e'){
            c->json += 1;//跳过E,e
            if(!ISDIGIT(c->json[0]) && c->json[0] != '+' && c->json[0] != '-') {
                return LEPT_PARSE_INVALID_VALUE;
            }

            if(c->json[0] == '+' || c->json[0] == '-')
                c->json += 1;

            while(c->json[0]!='\0'){
                if(!ISDIGIT(c->json[0]))
                    return LEPT_PARSE_INVALID_VALUE;
                c->json += 1;
            }    
        }

        //当前c->json[0]指向小数点
        if(c->json[0]=='.'){
            c->json += 1; //跳过小数点
            while(c->json[0]!='E'&&c->json[0]!='e'&&c->json[0]!='\0'){
                if(!ISDIGIT(c->json[0])){
                    return LEPT_PARSE_INVALID_VALUE;
                }
                c->json += 1;
            }

            //等于\0时表明校验到了结尾，校验成功
            //等于E或者e时表明还需要解析指数部分
            if(c->json[0]=='E'&&c->json[0]=='e'){
                c->json += 1;//跳过E,e
                if(!ISDIGIT(c->json[0]) && c->json[0] != '+' && c->json[0] != '-') {
                    return LEPT_PARSE_INVALID_VALUE;
                }

                if(c->json[0] == '+' || c->json[0] == '-')
                    c->json += 1;

                while(c->json[0]!='\0'){
                    if(!ISDIGIT(c->json[0]))
                        return LEPT_PARSE_INVALID_VALUE;
                    c->json += 1;
                }    
            }
        }
    }else {
        //首字符不为[0-9,-]
        return LEPT_PARSE_INVALID_VALUE;
    }
    

    v->n = strtod(c->json, &end);
    if (c->json == end)
        return LEPT_PARSE_INVALID_VALUE;
    c->json = end;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
}

static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
        case 'n':  return lept_parse_null(c, v);
        case 't':  return lept_parse_true(c, v);
        case 'f':  return lept_parse_false(c,v);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
        default:   return lept_parse_number(c,v);
    }
}

int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    int ret;
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        lept_parse_whitespace(&c);
        if (*c.json != '\0')
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
    }
    return ret;
}

