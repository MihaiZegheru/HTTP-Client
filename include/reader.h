#ifndef READER_H__
#define READER_H__

#include <string>
#include <vector>
#include <unordered_map>

class Reader {
public:
    void ReadParams(const std::vector<std::string>& params);
    void ReadHiddenParams(const std::vector<std::string>& params);

    const std::string& operator[](const std::string& key) const;
    
private:
    std::unordered_map<std::string, std::string> values;
};

#endif // READER_H__
