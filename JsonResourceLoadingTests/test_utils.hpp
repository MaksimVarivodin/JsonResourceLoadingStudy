#pragma once

#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

#include <nlohmann/json.hpp>

#include "../JsonResourceLoadingStudy/RResourceManager/ISerializable.hpp"
#include "../JsonResourceLoadingStudy/RResourceManager/JsonLoadable.hpp"
#include "../JsonResourceLoadingStudy/RResourceManager/ParserFactory.hpp"

namespace test_support
{
    namespace fs = std::filesystem;
    using nlohmann::json;

    class TempDir
    {
        fs::path root_;

    public:
        TempDir()
        {
            const auto stamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            root_ = fs::temp_directory_path() / ("JsonResourceLoadingTests_" + std::to_string(stamp));
            fs::create_directories(root_);
        }

        TempDir(const TempDir&) = delete;
        TempDir& operator=(const TempDir&) = delete;

        ~TempDir()
        {
            std::error_code ec;
            fs::remove_all(root_, ec);
        }

        const fs::path& path() const { return root_; }

        fs::path writeJson(const std::string& fileName, const json& j) const
        {
            fs::path p = root_ / fileName;
            std::ofstream out(p, std::ios::binary);
            out << j.dump(2);
            return p;
        }
    };

    struct DummyData final : journalist::ISerializable
    {
        journalist::Resource<"value", int> value{};

        ISerializable& toJson(json& j) override
        {
            value.toJson(j);
            return *this;
        }

        ISerializable& fromJson(const json& j) override
        {
            value.fromJson(j);
            return *this;
        }

        int getValue() const { return value.data.value_or(0); }
    };

    using DummyResource = journalist::JsonLoadable<DummyData>;

    inline void registerDummyParser(const std::string& typeName = "dummy")
    {
        journalist::ParserFactory::instance().registerParser(typeName, []() {
            return std::make_unique<DummyResource>();
        });
    }
}

