#include "pch.h"

#include <filesystem>

#include "../JsonResourceLoadingStudy/RResourceManager/PolymorphicResource.hpp"
#include "test_utils.hpp"

namespace
{
    using nlohmann::json;
}

TEST(ParserFactory, UnknownTypeReturnsNullptr)
{
    auto res = journalist::ParserFactory::instance().createParser("__definitely_unknown_type__");
    EXPECT_EQ(res, nullptr);
}

TEST(PolymorphicResource, CreatesConcreteResourceViaFactoryAndLoads)
{
    test_support::TempDir tmp;

    const auto dataPath = tmp.writeJson("dummy_resource.json", json::object({ {"value", 321} }));

    test_support::registerDummyParser("dummy");

    json def = json::object({
        {"type", "dummy"},
        {"path", dataPath.string()}
    });

    journalist::PolymorphicResource poly;
    poly.fromJson(def);

    auto* resource = poly.as<test_support::DummyResource>();
    ASSERT_NE(resource, nullptr);

    poly.load();

    EXPECT_EQ(resource->getData().getValue(), 321);
}

TEST(PolymorphicResource, UnknownTypeDoesNotCrashAndStaysNull)
{
    json def = json::object({ {"type", "__unknown__"} });

    journalist::PolymorphicResource poly;
    poly.fromJson(def);

    EXPECT_EQ(poly.resource, nullptr);

    // load() должен быть безопасным, даже если конкретный ресурс не создан.
    EXPECT_NO_THROW(poly.load());
}

