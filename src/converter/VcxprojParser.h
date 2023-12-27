#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>

class VcxprojParser
{
public:
    enum class LanguageStandard
    {
        CXX_11,
        CXX_14,
        CXX_17,
        CXX_20,
    };

    enum class ConditionType
    {
        RELEASE_X86,
        RELEASE_X64,
        DEBUG_X86,
        DEBUG_X64,
    };

    struct ConditionData
    {
        std::string targetName;

        std::string runtimeLibrary;
        std::vector<std::string> includeDirectories;
        std::vector<std::string> preprocessorDefinitions;
        LanguageStandard languageStandard;

        std::vector<std::string> libraryDirectories;
        std::vector<std::string> dependencies;
    };

public:
    VcxprojParser() = default;

    void parse(const std::filesystem::path& path);

    const std::string& getConfigurationType() const noexcept;
    const std::filesystem::path& getOutDir() const noexcept;
    const std::vector<std::string>& getSourceFiles() const noexcept;
    const std::unordered_map<ConditionType, ConditionData>& getConditions() const noexcept;

private:
    static ConditionType parseConditionString(const std::string& condition);

    static LanguageStandard parseLanguageStandard(const std::string& languageStandard);

private:
    std::string m_configurationType;
    std::filesystem::path m_outDir;

    std::vector<std::string> m_sourceFiles;

    std::unordered_map<ConditionType, ConditionData> m_conditions;
};