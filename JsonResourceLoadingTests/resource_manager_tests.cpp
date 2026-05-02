#include "pch.h"

#include "../JsonResourceLoadingStudy/RResourceManager/RResourceManager.hpp"
#include "test_utils.hpp"

namespace
{
    using nlohmann::json;
}

TEST(RResourceManager, FromJsonAndTryLoad_ArrayFormat)
{
    test_support::TempDir tmp;
    const auto dataPath = tmp.writeJson("dummy_resource.json", json::object({ {"value", 555} }));

    auto& mgr = journalist::RResourceManager::registerParsers({
        {"dummy", []() { return std::make_unique<test_support::DummyResource>(); }}
    });

    json root = json::object({
        {"type", "test_manager"},
        {"managed_resources", json::array({
            json::object({
                {"type", "dummy"},
                {"path", dataPath.string()}
            })
        })}
    });

    mgr.fromJson(root);

    EXPECT_EQ(mgr.getRType().getType(), "test_manager");

    auto* loaded = mgr.tryLoad<test_support::DummyResource>("dummy");
    ASSERT_NE(loaded, nullptr);
    EXPECT_EQ(loaded->getData().getValue(), 555);
}

TEST(RResourceManager, FromJsonAndTryLoad_ObjectFormat)
{
    test_support::TempDir tmp;
    const auto dataPath = tmp.writeJson("dummy_resource.json", json::object({ {"value", 777} }));

    // Можно регистрировать повторно: ParserFactory перезапишет creator для того же ключа.
    journalist::RResourceManager::registerParsers({
        {"dummy", []() { return std::make_unique<test_support::DummyResource>(); }}
    });

    auto& mgr = journalist::RResourceManager::instance();

    json root;
    root["type"] = "test_manager";
    root["managed_resources"] = json::object({
        {"dummy", json::object({
            {"type", "dummy"},
            {"path", dataPath.string()}
        })}
    });

    mgr.fromJson(root);

    auto* loaded = mgr.tryLoad<test_support::DummyResource>("dummy");
    ASSERT_NE(loaded, nullptr);
    EXPECT_EQ(loaded->getData().getValue(), 777);
}

TEST(RResourceManager, TryLoadUnknownReturnsNullptr)
{
    auto& mgr = journalist::RResourceManager::instance();
    EXPECT_EQ(mgr.tryLoad<test_support::DummyResource>("__unknown__"), nullptr);
}

