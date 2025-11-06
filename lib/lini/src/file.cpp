#include "file.hpp"

using namespace LINI;

#include <format>

std::string StringData::to_string(void)
{
    const auto size = data.size();
    if(data[0] == data[size - 1] && data[0] == '\"')
        data = data.substr(1, size - 2);
    return data;
}

void Section::dump(std::stringstream& out)
{
    for(auto entry : entries)
        out << entry.first << " = " << entry.second.data << "\n";
}

void File::dump(std::stringstream& out)
{
    for(auto section : sections)
    {
        out << std::format("[{}]", section.first) << "\n";
        section.second.dump(out);
        out << "\n";
    }
}

void File::parse(std::stringstream& in)
{
    std::string line, section_name;

    auto whitespace_trim = [](std::string& s) {
        s.erase(0, s.find_first_not_of(" \t\r\n"));
        s.erase(s.find_last_not_of(" \t\r\n") + 1);
    };

    while(std::getline(in, line))
    {
        whitespace_trim(line);

        // Skip comments
        if (line.empty() || line[0] == ';' || line[0] == '#')
            continue;

        // New section.
        if (line.front() == '[' && line.back() == ']')
        {
            section_name = line.substr(1, line.size() - 2);
            continue;
        }

        // Entry.
        auto pos = line.find('=');
        if (pos == std::string::npos) continue; // TODO: Has no '=', maybe malformed.

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        whitespace_trim(key);
        whitespace_trim(value);

        if(!sections.contains(section_name))
            sections[section_name] = Section();

        sections[section_name].entries[key].data = value;
    }
}