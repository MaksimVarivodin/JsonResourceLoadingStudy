#pragma once
#include "ISerializable.hpp"
#include "ParserFactory.hpp"
#include <memory>
#include <string>

namespace journalist
{
    class PolymorphicResource : public ILoadableResource
    {
    public:
        std::unique_ptr<ILoadableResource> resource;

        PolymorphicResource() = default;
        PolymorphicResource(const PolymorphicResource&) = delete;
        PolymorphicResource& operator=(const PolymorphicResource&) = delete;

        // Важно: из-за виртуального деструктора в базовых интерфейсах move-операции
        // могут не генерироваться автоматически. Явно делаем тип перемещаемым,
        // чтобы его можно было хранить в std::map и перемещать при вставке.
        PolymorphicResource(PolymorphicResource&& other) noexcept
            : resource(std::move(other.resource))
        {
        }

        PolymorphicResource& operator=(PolymorphicResource&& other) noexcept
        {
            if (this != &other)
            {
                resource = std::move(other.resource);
            }
            return *this;
        }

        ~PolymorphicResource() override = default;

        ISerializable& toJson(json& j) override
        {
            if (resource)
            {
                resource->toJson(j);
            }
            return *this;
        }

        ISerializable& fromJson(const json& j) override
        {
            if (j.contains("type"))
            {
                std::string type = j.at("type").get<std::string>();
                resource = ParserFactory::instance().createParser(type);
                if (resource)
                {
                    resource->fromJson(j);
                }
            }
            return *this;
        }

        void load() override
        {
            if (resource)
            {
                resource->load();
            }
        }

        template <typename T>
        T* as()
        {
            if (resource)
            {
                return dynamic_cast<T*>(resource.get());
            }
            return nullptr;
        }

        template <typename T>
        const T* as() const
        {
            if (resource)
            {
                return dynamic_cast<const T*>(resource.get());
            }
            return nullptr;
        }
    };
}
