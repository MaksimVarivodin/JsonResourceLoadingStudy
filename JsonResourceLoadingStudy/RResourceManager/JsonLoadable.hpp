#pragma once
#include <nlohmann/json.hpp>
#include <fstream>
#include "Resource.hpp"

#include "ISerializable.hpp"
namespace journalist
{
    using namespace nlohmann;

    template <Serializable T>
    class JsonLoadable : public ILoadableResource
    {
        Resource<"path", string> path = {};
        optional<T> data_ = {};

    public:
        json loadJson();
        JsonLoadable& loadDataFromJson();
        T getData() const;

        void load() override
        {
            loadDataFromJson();
        }

        ISerializable& toJson(json& j) override;
        ISerializable& fromJson(const json& j) override;

        ~JsonLoadable() override
        {
        }
    };
}

#include "JsonLoadable.inl"
