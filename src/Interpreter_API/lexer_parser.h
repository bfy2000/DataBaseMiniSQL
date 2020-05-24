#ifndef LEXER_PARSER_H
#define LEXER_PARSER_H

#include <string>
#include <vector>

#define INPUT_LENGTH 409600


int yylex();

int yyparse();

extern char input_s[INPUT_LENGTH];
extern size_t input_len;

extern std::string filename;


#ifdef YYSTYPE
#undef YYSTYPE
#endif

#ifdef YY_INPUT
#error YY_INPUT defined
#endif

#define YY_INPUT(i_s, i_len, I_LEN) do { std::strcpy(i_s, input_s); i_len = input_len; } while(0);


enum class Operator
{
    GT_OP,
    GE_OP,
    LT_OP,
    LE_OP,
    EQ_OP,
    NE_OP
};

inline Operator flip_operator(Operator op)
{
    switch (op)
    {
        case Operator::GT_OP:
            return Operator::LT_OP;
        case Operator::GE_OP:
            return Operator::LE_OP;
        case Operator ::LT_OP:
            return Operator ::GT_OP;
        case Operator ::LE_OP:
            return Operator ::GE_OP;
    }
    return op;      // = and != need not flip.
}

enum class SqlValueTypeBase {
    Integer,
    String,
    Float
};

struct SqlValueType {
    std::string attrName;
    SqlValueTypeBase type;

    ///following fileds only for attribute type, not a single sql value type.
    bool primary = false;
    size_t charSize; // charSize does not include the terminating zero of string!
    bool unique = false;

    // inline int M() const {
    //     switch (type) {
    //         case SqlValueTypeBase::Integer:
    //             return MINISQL_TYPE_INT;
    //         case SqlValueTypeBase::Float:
    //             return MINISQL_TYPE_FLOAT;
    //         case SqlValueTypeBase::String:
    //             return MINISQL_TYPE_CHAR;
    //     }
    // }

    // inline size_t getSize() const {
    //     switch (M()) {
    //         case MINISQL_TYPE_INT:
    //             return sizeof(int);
    //         case MINISQL_TYPE_FLOAT:
    //             return sizeof(float);
    //         case MINISQL_TYPE_CHAR:
    //             return charSize + 1;
    //     }
    // }

    // inline int getDegree() const {
    //     size_t keySize = getSize();
    //     int degree = BlockSize / (keySize + sizeof(int));

    //     return degree;
    // }
};

struct SqlValue {
    SqlValueType type;
    int i;
    float r;
    std::string str;

    // inline size_t M() const {
    //     switch (type.type) {
    //         case SqlValueTypeBase::Integer:
    //             return MINISQL_TYPE_INT;
    //         case SqlValueTypeBase::Float:
    //             return MINISQL_TYPE_FLOAT;
    //         case SqlValueTypeBase::String:
    //             return MINISQL_TYPE_CHAR;
    //     }
    // }

    // bool operator<(const SqlValue &e) const {
    //     switch (M()) {
    //         case MINISQL_TYPE_INT:
    //             return i < e.i;
    //         case MINISQL_TYPE_FLOAT:
    //             return r < e.r;
    //         case MINISQL_TYPE_CHAR:
    //             return str < e.str;
    //         default:
    //             throw std::runtime_error("Undefined Type!");
    //     }
    // }

    // bool operator==(const SqlValue &e) const {
    //     switch (M()) {
    //         case MINISQL_TYPE_INT:
    //             return i == e.i;
    //         case MINISQL_TYPE_FLOAT:
    //             return r == e.r;
    //         case MINISQL_TYPE_CHAR:
    //             return str == e.str;
    //         default:
    //             throw std::runtime_error("Undefined Type!");
    //     }
    // }

    // bool operator!=(const SqlValue &e) const { return !operator==(e); }

    // bool operator>(const SqlValue &e) const { return !operator<(e) && operator!=(e); }

    // bool operator<=(const SqlValue &e) const { return operator<(e) || operator==(e); }

    // bool operator>=(const SqlValue &e) const { return !operator<(e); }

    // void reset() {
    //     str.clear();
    //     i = 0;
    //     r = 0;
    // }

    // std::string toStr() const {
    //     switch (M()) {
    //         case MINISQL_TYPE_INT:
    //             return std::to_string(i);
    //         case MINISQL_TYPE_FLOAT:
    //             return std::to_string(r);
    //         case MINISQL_TYPE_CHAR:
    //             return this->str;
    //     }
    // }
};

struct Condition {
    std::string name;
    Operator op;
    SqlValue val;
};

typedef struct yystype
{
    int i;
    double r;
    std::string str;
    bool b;
    void *dummy;    //for non-final nodes dummy part in AST.
    Operator op;
    SqlValue val;
    Condition cond;
    std::pair<std::string, SqlValueType> schema;
    std::vector<std::string> str_list;
    std::vector<Condition> cond_list;
    std::vector<SqlValue> val_list;
    std::vector<std::pair<std::string, SqlValueType>> schema_list;
    SqlValueType val_type;
} YYSTYPE;

int yyerror(const char *s);

extern "C" int yywrap();

#endif