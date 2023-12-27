#include "VcxprojConverter.h"

#include <fmt/format.h>

#include <stdexcept>

VcxprojConverter::VcxprojConverter(const std::filesystem::path& pathToVcxprojFile)
    : m_path(pathToVcxprojFile)
{
}

VcxprojConverter::~VcxprojConverter()
{
}

void VcxprojConverter::convert()
{
    if (!std::filesystem::exists(m_path))
    {
        throw std::runtime_error(fmt::format("File {} doesn't exist", m_path.string()));
    }
}
