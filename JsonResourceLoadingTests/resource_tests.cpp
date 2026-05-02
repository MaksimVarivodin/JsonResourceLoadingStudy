#include "pch.h"

#include <map>
#include <vector>

#include "../JsonResourceLoadingStudy/RResourceManager/Resource.hpp"
#include "test_utils.hpp"

namespace
{
    using nlohmann::json;
}

TEST(Resource, PrimitiveToJsonWritesNativeJsonType)
{
    journalist::Resource<"value", int> r{};
    r(42);

    json j;
    r.toJson(j);

    ASSERT_TRUE(j.contains("value"));
    EXPECT_TRUE(j.at("value").is_number_integer());
    EXPECT_EQ(j.at("value").get<int>(), 42);
}

TEST(Resource, PrimitiveFromJsonReadsValue)
{
    json j = { {"value", 7} };

    journalist::Resource<"value", int> r{};
    r.fromJson(j);

    ASSERT_TRUE(r.data.has_value());
    EXPECT_EQ(r.data.value(), 7);
}

TEST(Resource, SerializableScalarIsNestedUnderKey)
{
    test_support::DummyData d;
    d.value(123);

    journalist::Resource<"data", test_support::DummyData> r{};
    r(d);

    json j;
    r.toJson(j);

    ASSERT_TRUE(j.contains("data"));
    EXPECT_TRUE(j.at("data").is_object());
    EXPECT_EQ(j.at("data").at("value").get<int>(), 123);

    journalist::Resource<"data", test_support::DummyData> r2{};
    r2.fromJson(j);

    ASSERT_TRUE(r2.data.has_value());
    EXPECT_EQ(r2.data->getValue(), 123);
}

TEST(Resource, VectorOfSerializableRoundTrip)
{
    test_support::DummyData a;
    a.value(1);
    test_support::DummyData b;
    b.value(2);

    journalist::Resource<"items", std::vector<test_support::DummyData>> r{};
    r(std::vector<test_support::DummyData>{a, b});

    json j;
    r.toJson(j);

    ASSERT_TRUE(j.contains("items"));
    ASSERT_TRUE(j.at("items").is_array());
    ASSERT_EQ(j.at("items").size(), 2);
    EXPECT_EQ(j.at("items")[0].at("value").get<int>(), 1);
    EXPECT_EQ(j.at("items")[1].at("value").get<int>(), 2);

    journalist::Resource<"items", std::vector<test_support::DummyData>> r2{};
    r2.fromJson(j);

    ASSERT_TRUE(r2.data.has_value());
    ASSERT_EQ(r2.data->size(), 2);
    EXPECT_EQ((*r2.data)[0].getValue(), 1);
    EXPECT_EQ((*r2.data)[1].getValue(), 2);
}

TEST(Resource, MapStringKey_ObjectFormat)
{
    using Map = std::map<std::string, test_support::DummyData>;
    journalist::Resource<"things", Map> r{};

    json j;
    j["things"] = json::object({
        {"a", json::object({{"value", 10}})},
        {"b", json::object({{"value", 20}})}
    });

    r.fromJson(j);

    ASSERT_TRUE(r.data.has_value());
    EXPECT_EQ(r.data->at("a").getValue(), 10);
    EXPECT_EQ(r.data->at("b").getValue(), 20);
}

TEST(Resource, MapStringKey_ArrayBackwardCompatibleFormat)
{
    using Map = std::map<std::string, test_support::DummyData>;
    journalist::Resource<"things", Map> r{};

    json j;
    j["things"] = json::array({
        json::object({{"type", "a"}, {"value", 100}}),
        json::object({{"name", "b"}, {"value", 200}})
    });

    r.fromJson(j);

    ASSERT_TRUE(r.data.has_value());
    EXPECT_EQ(r.data->at("a").getValue(), 100);
    EXPECT_EQ(r.data->at("b").getValue(), 200);
}

