#pragma once

#include <cstddef>
#include <ostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <variant>

namespace jsonparse
{
    enum class token_type
    {
        STRING,
        NUMBER,
        TRUE,
        FALSE,
        NONE, // NULL
        LBRACE,
        RBRACE,
        LBRACKET,
        RBRACKET,
        COMMA,
        COLON,

        END
    };

    class token
    {
    public:
        token(token_type type, std::string lexeme);
        token(token_type type);
        token(token_type type, double value);

        const token_type get_type() const;
        const std::string get_lexeme() const;

        friend std::ostream& operator<<(std::ostream& os, const token& token);

    private:
        token_type type_;
        std::string lexeme_;
        double value_;
    };

    class lexer
    {
    public:
        lexer(std::string input);

        token next_token();
        token lex_string();
        token lex_number();
        token lex_alpha();

        char get_next_char();
        char look_next_char();

        std::string input;
        std::stringstream input_stream;
    };

    class ast
    {};

    class json_obj : public ast
    {
    public:
        json_obj();
        ast operator [](std::string key);

    private:
        std::map<std::string, ast*> values_;
    };

    class json_array : public ast
    {
    public:
        json_array();
        ast operator [](int index);

    private:
        std::vector<ast*> values_;
    };

    class json_literal : public ast
    {
    public:
        json_literal();
        std::variant<int, bool, std::string, std::nullptr_t> value_;
    };

} // ! jsonparse
