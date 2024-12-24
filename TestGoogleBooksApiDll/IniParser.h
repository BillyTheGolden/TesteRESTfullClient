#pragma once
/**
 * @file IniParser.h
 * 
 * This file contains the declaration of the IniParser class. 
 * The source was based on IniParser class from https://github.com/90th/iniParser
 */

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <stdexcept>

namespace NSIniParser
{
    using namespace std;

    class IniParser
    {
    private:
        unordered_map<string, unordered_map<string, string>> m_iniData;

        string trimWhitespace(const string& str) const
        {
            auto isNotSpace = [](char c) { return !isspace(static_cast<uint8_t>(c)); };
            auto first = find_if(str.begin(), str.end(), isNotSpace);
            auto last = find_if(str.rbegin(), str.rend(), isNotSpace).base();

            return (first == str.end() || last == str.begin()) ? string{} : string(first, last);
        }

    public:
        string getValue(const string& section, const string& key) const
        {
            auto sectionIt = m_iniData.find(section);
            if (sectionIt == m_iniData.end())
                throw runtime_error("Section not found " + section);

            auto keyIt = sectionIt->second.find(key);
            if (keyIt == sectionIt->second.end())
                throw runtime_error("Key not found in section : " + key);

            return keyIt->second;
        }

        void addSection(const string& section)
        {
            auto sectionIt = m_iniData.find(section);
            if (sectionIt == m_iniData.end())
                m_iniData[section] = {};

            sectionIt->second.clear();
        }

        void removeSection(const string& section)
        {
            m_iniData.erase(section);
        }

        void removekey(const string& section, const string& key)
        {
            auto sectionIt = m_iniData.find(section);
            if (sectionIt != m_iniData.end())
                sectionIt->second.erase(key);
        }

        inline void parseFromFile(const string& filename)
        {
            ifstream file(filename);
            if (!file.is_open())
                throw runtime_error("Failed to open file " + filename);

            string line;
            string currentSection;
            while (getline(file, line))
            {
                line = trimWhitespace(line);
                if (line.empty() || line[0] == ';')
                    continue;

                if (line[0] == '[' && line.back() == ']')
                {
                    currentSection = line.substr(1, line.size() - 2);

                    if (currentSection.empty())
                        throw runtime_error("Empty section name encountered.");

                    continue;
                }

                auto delimiter = line.find('=');
                if (delimiter == string::npos)
                    throw runtime_error("Invalid line format: " + line);

                auto key = trimWhitespace(line.substr(0, delimiter));
                auto value = trimWhitespace(line.substr(delimiter + 1));

                if (key.empty() || value.empty())
                    throw runtime_error("Invalid key/value pair: " + line);

                m_iniData[currentSection][key] = value;
            }
        }

        inline void parseFromString(const string& data)
        {
            istringstream iss(data);
            string line;
            string currentSection;
            
            while (getline(iss, line))
            {
                line = trimWhitespace(line);
                if (line.empty() || line[0] == ';')
                    continue;

                if (line[0] == '[' && line.back() == ']')
                {
                    currentSection = line.substr(1, line.size() - 2);
                    if (currentSection.empty())
                        throw runtime_error("Empty section name encountered.");

                    continue;
                }

                auto delimiter = line.find('=');
                if (delimiter == string::npos)
                    throw runtime_error("Invalid line format: " + line);

                auto key = trimWhitespace(line.substr(0, delimiter));
                auto value = trimWhitespace(line.substr(delimiter + 1));
                
                if (key.empty() || value.empty())
                    throw runtime_error("Invalid key/value pair: " + line);
                
                m_iniData[currentSection][key] = value;
            }
        }

        inline void setvalue(const string& section, const string& key, const string& value)
        {
            auto trimmedKey = trimWhitespace(key);
            auto trimmedValue = trimWhitespace(value);

            auto sectionIt = m_iniData.find(section);
            if (sectionIt == m_iniData.end())
                m_iniData[section][trimmedKey] = value;
            else
            {
                auto& sectionMap = sectionIt->second;
                auto keyIt = sectionMap.find(trimmedKey);

                if (keyIt == sectionMap.end())
                    sectionMap[trimmedKey] = trimmedValue;
                else
                    keyIt->second = trimmedValue;
            }
        }

        inline void saveToFile(const string& filename)
        {
            ofstream file(filename);
            if (!file.is_open())
                throw runtime_error("Failed to open file " + filename);

            for (const auto& section : m_iniData)
            {
                file << "[" << section.first << "]" << endl;
                for (const auto& key : section.second)
                    file << trimWhitespace(key.first) << " = " << trimWhitespace(key.second) << endl;

                file << endl;
            }
        }
    };
}
