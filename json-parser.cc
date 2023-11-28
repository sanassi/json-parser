#include "json-parser.hh"
#include <cctype>
#include <cstdlib>
#include <ostream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

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
    {
        current = 0;
    }

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


    double token::get_value() const
    {
        return value_;
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
        token res;
        if (current < tokens.size())
        {
            res = tokens[current];
            current += 1;
            return res;
        }

        while (std::isspace(look_next_char()))
            get_next_char();

        char c = input_stream.peek();

        switch (c)
        {
        case '{':
            get_next_char();
            res = token(token_type::LBRACE, "{");
            break;
        case '}':
            get_next_char();
            res = token(token_type::RBRACE, "}");
            break;
        case '[':
            get_next_char();
            res = token(token_type::LBRACKET, "[");
            break;
        case ']':
            get_next_char();
            res = token(token_type::RBRACKET, "]");
            break;
        case ',':
            get_next_char();
            res = token(token_type::COMMA, ",");
            break;
        case ':':
            get_next_char();
            res = token(token_type::COLON, ":");
            break;
        case '\"':
            res = lex_string();
            break;
        case '-':
            res = lex_number();
            break;
        case EOF:
            get_next_char();
            res = token(token_type::END, "");
            break;
        default:
            if (std::isdigit(c))
                res = lex_number();
            else if (std::isalpha(c))
                res = lex_alpha();
            break;
        }

        tokens.push_back(res);
        current += 1;

        return res;
    }
    
    token lexer::look_next_token()
    {
        token res = next_token();

        if (current > 0)
            current -= 1;

        return res;
    }

    parser::parser(lexer &lexer)
        :lexer_(lexer)
    {}

    lexer& parser::lexer_get() { return lexer_; }

    json_value::json_value(std::variant<std::vector<json_value*>, std::map<std::string, json_value*>, double, bool, std::nullptr_t, std::string> types)
        :types_(types)
    {}

    json_value *parser::parse_json()
    {
        return parse_value();
    }

    json_value *parser::parse_array()
    {
        lexer_.next_token();

        std::vector<json_value*> data;

        if (lexer_.look_next_token().get_type() == token_type::RBRACKET)
            return new json_value(data);

        data.push_back(parse_value());

        while (lexer_.look_next_token().get_type() == token_type::COMMA)
        {
            lexer_.next_token();
            data.push_back(parse_value());
        }

        lexer_.next_token();

        return new json_value(data);
    }

    json_value *parser::parse_obj()
    {
        lexer_.next_token();
        return {};
    }

    json_value *parser::parse_value()
    {
        json_value *res = new json_value();

        switch (lexer_.look_next_token().get_type()) 
        {
            case token_type::STRING:
                res->types_ = lexer_.next_token().get_lexeme();
                break;
            case token_type::NUMBER:
                res->types_ = lexer_.next_token().get_value();
                break;
            case token_type::TRUE:
            case token_type::FALSE:
                res->types_ = lexer_.next_token().get_type() == token_type::TRUE;
                break;
            case token_type::LBRACKET:
                return parse_array();
            case token_type::LBRACE:
                return parse_obj();
            default:
                break;
        }

        return res;
    }
}

int main(void)
{
    jsonparse::lexer lexer("[1,2, \"hello\", 4]");

    jsonparse::parser p(lexer);
    auto json = p.parse_json()->types_;

    std::cout << std::get<std::vector<jsonparse::json_value*>>(json).size() << std::endl;

    return 0;
}
