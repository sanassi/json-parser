#pragma once

#include <cstddef>
#include <ostream>
#include <string>
#include <map>
#include <utility>
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
        token() = default;
        token(token_type type, std::string lexeme);
        token(token_type type);
        token(token_type type, double value);

        const token_type get_type() const;
        const std::string get_lexeme() const;
        double get_value() const;

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
        token look_next_token();

        token lex_string();
        token lex_number();
        token lex_alpha();

        char get_next_char();
        char look_next_char();

        std::vector<token> tokens;
        size_t current;

        std::string input;
        std::stringstream input_stream;
    };


    class json_value
    {
    public:
        json_value() = default;
        json_value(std::variant<std::vector<json_value>, std::map<std::string, json_value>, double, bool, std::nullptr_t, std::string>);

        std::variant<std::vector<json_value>, std::map<std::string, json_value>, double, bool, std::nullptr_t, std::string> types_;
    };

    class parser
    {
    public:
        parser(lexer &lexer);

        using json_pair = std::pair<std::string, json_value>;
        using map_type = std::map<std::string, json_value>;

        json_value parse_json();
        json_value parse_value();
        json_value parse_obj();
        json_value parse_array();
        json_pair parse_pair();

        lexer& lexer_get();

    private:
        lexer& lexer_;
    };

    class json_obj : public json_value
    {
    public:
        json_obj(parser::map_type map);

        json_value& operator[](std::string);

    protected:
        parser::map_type map_;
    };

} // ! jsonparse
