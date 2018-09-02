#ifndef __JOSN_PARSER_HPP__
#define __JOSN_PARSER_HPP__

#include <cstdlib>
class JsonParser {

public:
    typedef enum { 
        VJ_NULL,
        VJ_FALSE, 
        VJ_TRUE, 
        VJ_NUMBER, 
        VJ_STRING, 
        VJ_ARRAY, 
        VJ_OBJECT,
    } lept_type;

    enum {
        VJ_PARSE_OK = 0,
        VJ_STRINGIFY_OK,
        VJ_PARSE_EXPECT_VALUE,
        VJ_PARSE_INVALID_VALUE,
        VJ_PARSE_ROOT_NOT_SINGULAR,
        VJ_PARSE_NUMBER_TOO_BIG,
        VJ_PARSE_MISS_QUOTATION_MARK,
        VJ_PARSE_INVALID_STRING_ESCAPE,
        VJ_PARSE_INVALID_STRING_CHAR,
        VJ_PARSE_INVALID_UNICODE_HEX,
        VJ_PARSE_INVALID_UNICODE_SURROGATE,
        VJ_PARSE_MISS_COMMA_OR_SQUARE_BRACKET,
        VJ_PARSE_MISS_KEY,
        VJ_PARSE_MISS_COLON,
        VJ_PARSE_MISS_COMMA_OR_CURLY_BRACKET,
        VJ_KEY_NOT_EXIST,
    };

    #define lept_init(v) do { (v)->type = LEPT_NULL; } while(0)

    void lept_free(lept_value* v);
    int lept_parse(lept_value* v, const char* json);
    lept_type lept_get_type(const lept_value* v);

    int lept_get_boolean(const lept_value* v);
    void lept_set_boolean(lept_value* v, int b);


    double lept_get_number(const lept_value* v);
    void lept_set_number(lept_value* v, double n);


    const char* lept_get_string(const lept_value* v);
    size_t lept_get_string_length(const lept_value* v);
    void lept_set_string(lept_value* v, const char* s, size_t len);
        
    size_t lept_get_array_size(const lept_value* v);
    lept_value* lept_get_array_element(const lept_value* v, size_t index);

    size_t lept_get_object_size(const lept_value* v);
    const char* lept_get_object_key(const lept_value* v, size_t index);
    size_t lept_get_object_key_length(const lept_value* v, size_t index);
    lept_value* lept_get_object_value(const lept_value* v, size_t index);
    size_t lept_find_object_index(const lept_value* v, const char* key, size_t klen);
    lept_value* lept_find_object_value(lept_value* v, const char* key, size_t klen);

private:
    struct vj_value {
        union {
            struct { vj_member* m; size_t size; }o; //object
            struct { vj_value* e; size_t size; }a; //array
            struct {char* s; size_t len;}s; // string
            double n; //number
        }u;
        vj_type type;
    };

    struct vj_member {
        char* k; size_t klen;   /* member key string, key string length */
        lept_value v;           /* member value */
    };

    typedef struct {
        const char* json;

        //解析的数据暂时放在栈中
        char* stack;
        size_t size, top;
    }vj_context;


}



#endif