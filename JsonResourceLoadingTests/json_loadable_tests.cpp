#include "pch.h"

#include <filesystem>

#include "test_utils.hpp"

namespace
{
    using nlohmann::json;
}

TEST(JsonLoadable, LoadJsonReadsFile)
{
    test_support::TempDir tmp;
    const auto p = tmp.writeJson("data.json", json::object({ {"value", 11} }));

    test_support::DummyResource loadable;
    loadable.fromJson(json::object({ {"path", p.string()} }));

    const json loaded = loadable.loadJson();

    ASSERT_FALSE(loaded.empty());
    EXPECT_EQ(loaded.at("value").get<int>(), 11);
}

TEST(JsonLoadable, LoadDataFromJsonPopulatesData)
{
    test_support::TempDir tmp;
    const auto p = tmp.writeJson("data.json", json::object({ {"value", 99} }));

    test_support::DummyResource loadable;
    loadable.fromJson(json::object({ {"path", p.string()} }));

    loadable.loadDataFromJson();

    EXPECT_EQ(loadable.getData().getValue(), 99);
}

TEST(JsonLoadable, MissingFileKeepsDefaultData)
{
    test_support::TempDir tmp;
    const auto missing = (tmp.path() / "missing.json").string();

    test_support::DummyResource loadable;
    loadable.fromJson(json::object({ {"path", missing} }));

    loadable.loadDataFromJson();

    EXPECT_EQ(loadable.getData().getValue(), 0);
}

