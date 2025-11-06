#pragma once
#ifndef LINI_PARSER_HPP
#define LINI_PARSER_HPP

#include <string>
#include <sstream>
#include <unordered_map>

namespace LINI
{
    struct StringData
    {
        std::string data;

        std::string to_string(void);
    };

    struct Section
    {
        std::unordered_map<std::string, StringData> entries;

        void dump(std::stringstream&);
    };

    struct File
    {
        std::unordered_map<std::string, Section> sections;

        void parse(std::stringstream&);
        void dump(std::stringstream&);
    };
};

#endif
