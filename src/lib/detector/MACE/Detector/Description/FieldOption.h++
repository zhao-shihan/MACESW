#pragma once

#include "Mustard/Detector/Description/DescriptionBase.h++"

#include "envparse/parse.h++"

#include <filesystem>
#include <string>

namespace MACE::Detector::Description {

class FieldOption final : public Mustard::Detector::Description::DescriptionBase<FieldOption> {
    friend Mustard::Env::Memory::SingletonInstantiator;

private:
    FieldOption();
    ~FieldOption() override = default;

public:
    auto UseFast() const -> auto { return fUseFast; }
    auto FieldDataFilePath() const -> const auto& { return fFieldDataFilePath; }
    auto ParsedFieldDataFilePath() const -> std::filesystem::path { return envparse::parse(fFieldDataFilePath); }

    auto UseFast(bool val) -> void { fUseFast = val; }
    auto FieldDataFilePath(std::string val) -> void { fFieldDataFilePath = std::move(val); }

private:
    auto ImportAllValue(const YAML::Node& node) -> void override;
    auto ExportAllValue(YAML::Node& node) const -> void override;

private:
    bool fUseFast;
    std::string fFieldDataFilePath;
};

} // namespace MACE::Detector::Description
