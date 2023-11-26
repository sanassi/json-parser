#include "json-parser.hh"
#include <cctype>
#include <cstdlib>
#include <ostream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <string>

namespace jsonparse
{
    token::token(token_type type, std::string lexeme)
        : type_(type),
          lexeme_(lexeme)
    {}

    token::token(token_type type, double value)
        : type_(type),
          value_(value)
    {}

    token::token(token_type type)
        : type_(type)
    {}

    const std::string token::get_lexeme() const { return lexeme_; }
    const token_type token::get_type() const { return type_; }

    lexer::lexer(std::string input)
        : input(input),
          input_stream(std::stringstream(input))
    {}

    std::ostream& operator<<(std::ostream& os, const token& token)
    {
        switch (token.type_)
        {
        case token_type::STRING:
            os << "\"" << token.get_lexeme() << "\"";
            break;
        case token_type::NUMBER:
            os << token.value_;
            break;
        default:
            os << token.lexeme_;
        }
         return os;
    }

    char lexer::get_next_char()
    {
        char next;
        input_stream.get(next);

        return next;
    }

    char lexer::look_next_char()
    {
        return input_stream.peek();
    }

    token lexer::lex_string()
    {
        std::string res = "";

        get_next_char();

        while (input_stream.peek() != '\"')
            res += get_next_char();

        get_next_char();

        return token(token_type::STRING, res);
    }

    token lexer::lex_alpha()
    {
        std::string res = "";

        while (std::isalpha(look_next_char()))
            res += get_next_char();

        token_type type;

        if (res == "null")
            type = token_type::NONE;
        else if (res == "false")
            type = token_type::FALSE;
        else if (res == "true")
            type = token_type::TRUE;
        else
        {
            // error
        }

        return token(type, res);
    }

    token lexer::lex_number()
    {
        char c;
        std::string res = "";
        if ((c = look_next_char()) == '-') 
        {
            res += get_next_char();

            if (!std::isdigit(look_next_char()))
            {
                // error
            }
        }

        if (look_next_char() == '0')
        {
            res += get_next_char();
            // leading zeros are not allowed
            if (std::isdigit(look_next_char()))
            {
                // error
            }

            if (look_next_char() != '.')
                return token(token_type::NUMBER, 0);
        }

        while (std::isdigit(look_next_char()))
            res += get_next_char();

        if (look_next_char() == '.')
        {
            res += get_next_char();
            while (std::isdigit(look_next_char()))
                res += get_next_char();
        }

        if (look_next_char() == 'e' || look_next_char() == 'E')
        {
            res += get_next_char();
            if (look_next_char() == '+' || look_next_char() == '-')
                res += get_next_char();
            while (std::isdigit(look_next_char()))
                res += get_next_char();
        }

        return token(token_type::NUMBER, atof(res.c_str()));
    }

    token lexer::next_token()
    {
        while (std::isspace(look_next_char()))
            get_next_char();

        char c = input_stream.peek();
        switch (c)
        {
        case '{':
            get_next_char();
            return token(token_type::LBRACE, "{");
        case '}':
            get_next_char();
            return token(token_type::RBRACE, "}");
        case '[':
            get_next_char();
            return token(token_type::LBRACKET, "[");
        case ']':
            get_next_char();
            return token(token_type::RBRACKET, "]");
        case ',':
            get_next_char();
            return token(token_type::COMMA, ",");
        case ':':
            get_next_char();
            return token(token_type::COLON, ":");
        case '\"':
            return lex_string();
        case '-':
            return lex_number();
        default:
            if (std::isdigit(c))
                return lex_number();

            if (std::isalpha(c))
                return lex_alpha();
            break;
        }

        get_next_char();

        return token(token_type::END, "");
    }

}

int main(void)
{
    jsonparse::lexer lexer("{\n \"a\":\"hello\", \n\t \"b\": 3e+4 , \"b\":true    \n}");
    
    while (!lexer.input_stream.eof())
        std::cout << lexer.next_token();
    std::cout << std::endl;

    return 0;
}
