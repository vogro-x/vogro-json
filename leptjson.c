#include "leptjson.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>   
#include <math.h>   

#define ISDIGIT(ch) ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch) ((ch) >= '1' && (ch) <= '9')
#define EXPECT(c, ch) do { assert(*c->json == (ch)); c->json++; } while(0)
#define PUTC(c, ch) do { *(char*)lept_context_push(c, sizeof(char)) = (ch); } while(0)

#ifndef LEPT_PARSE_STACK_INIT_SIZE
#define LEPT_PARSE_STACK_INIT_SIZE 256
#endif

void lept_free(lept_value* v) {
    assert(v != NULL);
    if (v->type == LEPT_STRING)
        free(v->u.s.s);
    v->type = LEPT_NULL;
}


lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}


static void* lept_context_push(lept_context* c, size_t size) {
    void* ret;
    assert(size > 0);
    if (c->top + size >= c->size) {
        if (c->size == 0)
            c->size = LEPT_PARSE_STACK_INIT_SIZE;
        while (c->top + size >= c->size)
            c->size += c->size >> 1;  /* c->size * 1.5 */
        c->stack = (char*)realloc(c->stack, c->size);//扩容
    }
    ret = c->stack + c->top;
    c->top += size;
    return ret;//ret指向top处
}

static void* lept_context_pop(lept_context* c, size_t size) {
    assert(c->top >= size);
    return c->stack + (c->top -= size);//指向弹出之后的top处
}

int lept_get_boolean(const lept_value* v){
    assert(v!=NULL);
    if(v->type==LEPT_FALSE)
        return 0;
    else if(v->type==LEPT_TRUE)
        return 1;
    else{
        assert(0);
        return 0;
    }
}

void lept_set_boolean(lept_value* v, int b){
    assert(v!=NULL);
    if(b==0)
        v->type=LEPT_FALSE;
    else
        v->type=LEPT_TRUE;
}


double lept_get_number(const lept_value* v){
    assert(v!=NULL && v->type==LEPT_NUMBER);
    return v->u.n;
}

void lept_set_number(lept_value* v, double n){
    assert(v!=NULL && v->type==LEPT_NUMBER);
    v->u.n=n;
}


const char* lept_get_string(const lept_value* v){
    assert(v!=NULL && v->type==LEPT_STRING);
    return v->u.s.s;
}

size_t lept_get_string_length(const lept_value* v){
    assert(v!=NULL && v->type==LEPT_STRING);
    return v->u.s.len;

}

void lept_set_string(lept_value* v, const char* s, size_t len){
    assert(v != NULL && (s != NULL || len == 0));
    lept_free(v);
    v->u.s.s = (char*)malloc(len + 1);
    memcpy(v->u.s.s, s, len);
    v->u.s.s[len] = '\0';
    v->u.s.len = len;
    v->type = LEPT_STRING;
}

static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

// parse null/true/false
static int lept_parse_literal(lept_context* c, lept_value* v, char* json, size_t size) {
     EXPECT(c,json[0]);
     int i;
     for(i=0;i<size-1;i++){
         if(c->json[i]!=json[i+1])
            return LEPT_PARSE_INVALID_VALUE;
     }
     c->json += (size-1);
     switch (json[0]){
        case 'n':
            v->type=LEPT_NULL;
            break;
        case 't':
            v->type=LEPT_TRUE;
            break;
        case 'f':
            v->type=LEPT_FALSE;
            break;
     }
     return LEPT_PARSE_OK;
}

static int lept_parse_number(lept_context* c, lept_value* v) {
    // char* end;

    const char* tmp = c->json;
    // 0-9,-
    if(ISDIGIT(tmp[0])||tmp[0]=='-') {
        if(tmp[0]=='-') {
            tmp += 1;
        }

        if(tmp[0]=='0' && tmp[1]!='.' && tmp[1] != '\0') {
            return LEPT_PARSE_INVALID_VALUE;
        }

        
        while(tmp[0] != '.' && tmp[0] != 'E' && tmp[0] != 'e' && tmp[0] != '\0') {
            if(!ISDIGIT(tmp[0])) {
                return LEPT_PARSE_INVALID_VALUE;
            }
            tmp += 1;
        }

        if(tmp[0]=='\0' && *(tmp-1)=='-') {  // -
            return LEPT_PARSE_INVALID_VALUE;
        }

        if(tmp[0]=='E' || tmp[0]=='e'){
            tmp += 1;//跳过E,e
            if(!ISDIGIT(tmp[0]) && tmp[0] != '+' && tmp[0] != '-' ) {
                return LEPT_PARSE_INVALID_VALUE;
            }

            if(tmp[0]=='\0'){  // 1.2e
                    return LEPT_PARSE_INVALID_VALUE;
                }

            if(tmp[0] == '+' || tmp[0] == '-'){
                tmp += 1;
                if(tmp[0]=='\0'){
                    return LEPT_PARSE_INVALID_VALUE;
                }
            }
                
            while(tmp[0]!='\0'){
                if(!ISDIGIT(tmp[0]))
                    return LEPT_PARSE_INVALID_VALUE;
                tmp += 1;
            }    
        }

        //当前c->json[0]指向小数点
        if(tmp[0]=='.'){
            tmp += 1; //跳过小数点
            while(tmp[0]!='E' && tmp[0]!='e' && tmp[0]!='\0'){
                if(!ISDIGIT(tmp[0])){
                    return LEPT_PARSE_INVALID_VALUE;
                }
                tmp += 1;
            }

            if(*(tmp-1)=='.'){ //小数点后面没有内容，不合法--> 1.
                return LEPT_PARSE_INVALID_VALUE;
            }

            //等于\0时表明校验到了结尾，校验成功
            //等于E或者e时表明还需要解析指数部分
            if(tmp[0]=='E' || tmp[0]=='e') {
                tmp += 1;//跳过E,e
               if(!ISDIGIT(tmp[0]) && tmp[0] != '+' && tmp[0] != '-' ) {
                    return LEPT_PARSE_INVALID_VALUE;
                }

                if(tmp[0]=='\0'){  // 1.2e
                    return LEPT_PARSE_INVALID_VALUE;
                }

                if(tmp[0] == '+' || tmp[0] == '-'){
                    tmp += 1;
                    if(tmp[0]=='\0'){
                        return LEPT_PARSE_INVALID_VALUE;
                    }
                }
                
                while(tmp[0]!='\0'){
                    if(!ISDIGIT(tmp[0]))
                        return LEPT_PARSE_INVALID_VALUE;
                    tmp += 1;
                }    
            }
        }
    }else {
        //首字符不为[0-9,-]
        return LEPT_PARSE_INVALID_VALUE;
    }
    
    errno = 0;
    v->u.n = strtod(c->json, NULL);
    if (errno == ERANGE && (v->u.n == HUGE_VAL || v->u.n == -HUGE_VAL))
        return LEPT_PARSE_NUMBER_TOO_BIG;
    v->type = LEPT_NUMBER;
    c->json = tmp;//此时需要移动json指针的位置到末尾
    return LEPT_PARSE_OK;
}

static int lept_parse_string(lept_context* c, lept_value* v) {
    size_t head = c->top, len;
    const char* p;
    EXPECT(c, '\"');
    p = c->json;
    for (;;) {
        char ch = *p++;
        switch (ch) {
            case '\"':
                len = c->top - head;
                lept_set_string(v, (const char*)lept_context_pop(c, len), len);
                c->json = p;
                return LEPT_PARSE_OK;
            case '\0':
                c->top = head;
                return LEPT_PARSE_MISS_QUOTATION_MARK;
            default:
                PUTC(c, ch);
        }
    }
}


static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
        case 'n':  return lept_parse_literal(c,v,"null",4);
        case 't':  return lept_parse_literal(c,v,"true",4);
        case 'f':  return lept_parse_literal(c,v,"false",5);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
        default:   return lept_parse_number(c,v);
    }
}

int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    assert(v != NULL);
    c.json = json;
    c.stack = NULL;        /* <- */
    c.size = c.top = 0;    /* <- */
    v->type = LEPT_NULL;
    // lept_init(v);
    lept_parse_whitespace(&c);
    int ret;
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        lept_parse_whitespace(&c);
        if (*c.json != '\0')
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
    }
    assert(c.top == 0);    /* <- */
    free(c.stack);     
    return ret;
}

