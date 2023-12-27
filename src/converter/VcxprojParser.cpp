#include "VcxprojParser.h"

#include "StringUtils.hpp"

#include <fmt/format.h>

#include <pugixml.hpp>

#include <stdexcept>

void VcxprojParser::parse(const std::filesystem::path& path)
{
    pugi::xml_document doc;
    auto result = doc.load_file(path.c_str());
    if (!result)
    {
        throw std::runtime_error(fmt::format("Error load_file {}: {}, {}", path.string(), static_cast<size_t>(result.status), result.description()));
    }

    const auto project = doc.child("Project");
    for (const auto& propertyGroup : project.children("PropertyGroup"))
    {
        if (std::string("Configuration") == propertyGroup.attribute("Label").value())
        {
            m_configurationType = propertyGroup.child("ConfigurationType").child_value();
        }
        else
        {
            m_outDir = propertyGroup.child("OutDir").child_value();
            const auto targetName = propertyGroup.child("TargetName");
            m_conditions[parseConditionString(targetName.attribute("Condition").value())].targetName = targetName.child_value();
        }
    }

    {
        const auto itemDefinitionGroup = project.child("ItemDefinitionGroup");

        const auto conditionType = parseConditionString(itemDefinitionGroup.attribute("Condition").value());
        auto& condition = m_conditions[conditionType];

        const auto clCompile = itemDefinitionGroup.child("ClCompile");
        condition.runtimeLibrary = clCompile.child("RuntimeLibrary").child_value();
        for (const auto& dir : StringUtils::Split(std::string(clCompile.child("AdditionalIncludeDirectories").child_value()), ';'))
        {
            if (dir.find("%(AdditionalIncludeDirectories)") == std::string::npos)
            {
                continue;
            }

            condition.includeDirectories.emplace_back(dir);
        }
        for (const auto& def : StringUtils::Split(std::string(clCompile.child("PreprocessorDefinitions").child_value()), ';'))
        {
            if (def.find("%(PreprocessorDefinitions)") == std::string::npos)
            {
                continue;
            }

            condition.preprocessorDefinitions.emplace_back(def);
        }
        condition.languageStandard = parseLanguageStandard(clCompile.child("LanguageStandard").child_value());

        const auto lib = itemDefinitionGroup.child("Lib");
        for (const auto& dir : StringUtils::Split(std::string(clCompile.child("AdditionalLibraryDirectories").child_value()), ';'))
        {
            if (dir.find("%(AdditionalLibraryDirectories)") == std::string::npos)
            {
                continue;
            }

            condition.libraryDirectories.emplace_back(dir);
        }
        for (const auto& dep : StringUtils::Split(std::string(clCompile.child("AdditionalDependencies").child_value()), ';'))
        {
            if (dep.find("%(AdditionalDependencies)") == std::string::npos)
            {
                continue;
            }

            condition.dependencies.emplace_back(dep);
        }
    }

    for (const auto& itemGroup : project.children("ItemGroup"))
    {
        for (const auto& cl : itemGroup.children("ClCompile"))
        {
            m_sourceFiles.emplace_back(cl.attribute("Include").value());
        }
    }
}

const std::string& VcxprojParser::getConfigurationType() const noexcept
{
    return m_configurationType;
}

const std::filesystem::path& VcxprojParser::getOutDir() const noexcept
{
    return m_outDir;
}

const std::vector<std::string>& VcxprojParser::getSourceFiles() const noexcept
{
    return m_sourceFiles;
}

const std::unordered_map<VcxprojParser::ConditionType, VcxprojParser::ConditionData>& VcxprojParser::getConditions() const noexcept
{
    return m_conditions;
}

VcxprojParser::ConditionType VcxprojParser::parseConditionString(const std::string& condition)
{
    if (condition.find("$(Configuration)|$(Platform)") == std::string::npos)
    {
        throw std::runtime_error(fmt::format("Error parsing condition ({})", condition));
    }

    const auto configurePlatform = StringUtils::Split<char>(condition, std::string("==")).back();
    const auto splittedConfigurePlatform = StringUtils::Split(configurePlatform, '|');

    const auto& configure = splittedConfigurePlatform.front();
    const auto& platform = splittedConfigurePlatform.back();

    if (configure == "Release" && platform == "x86")
    {
        return ConditionType::RELEASE_X86;
    }
    else if (configure == "Release" && platform == "x64")
    {
        return ConditionType::RELEASE_X64;
    }
    else if (configure == "Debug" && platform == "x86")
    {
        return ConditionType::DEBUG_X86;
    }
    else if (configure == "Debug" && platform == "x64")
    {
        return ConditionType::DEBUG_X64;
    }

    throw std::runtime_error(fmt::format("Unknown combination of configure|platform ({})", configurePlatform));
}

VcxprojParser::LanguageStandard VcxprojParser::parseLanguageStandard(const std::string& languageStandard)
{
    if (languageStandard == "stdcpp11")
    {
        return LanguageStandard::CXX_11;
    }
    else if (languageStandard == "stdcpp14")
    {
        return LanguageStandard::CXX_14;
    }
    else if (languageStandard == "stdcpp17")
    {
        return LanguageStandard::CXX_17;
    }
    else if (languageStandard == "stdcpp20")
    {
        return LanguageStandard::CXX_20;
    }

    throw std::runtime_error(fmt::format("Unknown language standard ({})", languageStandard));
}
