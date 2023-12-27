#pragma once

#include <string>
#include <filesystem>

class VcxprojConverter
{
public:
    VcxprojConverter() = delete;

    VcxprojConverter(const std::filesystem::path& pathToVcxprojFile);
    ~VcxprojConverter();

    void convert();

private:
    std::filesystem::path m_path;
};