#pragma once
#include "../JournalistApi.hpp"
#include "ISerializable.hpp"
#include "Resource.hpp"
#include "RResource.hpp"
#include <fstream>
#include <map>
#include "JsonLoadable.hpp"
#include "PolymorphicResource.hpp"
#include "RType.hpp"


namespace journalist
{
    //class PolymorphicResource;

    class JOURNALIST_API RResourceManager : public ISerializable
    {
        RType type_;
        Resource<"managed_resources", map<string, PolymorphicResource>> resources_;
        RResourceManager();

    public:
        static RResourceManager& instance();
        
        static RResourceManager& registerParsers(const vector<pair<string, ParserFactory::Creator>>& parsers); 
    public:
        const RType& getRType() const { return type_; }
        template <class T>
        T* tryLoad(const string& resourceTypeName);

    public:
        RResourceManager& toJson(json& j) override;
        RResourceManager& fromJson(const json& j) override;

    public:
        RResourceManager(const RResourceManager&) = delete;
        RResourceManager& operator=(const RResourceManager&) = delete;
        RResourceManager(RResourceManager&&) = delete;
        RResourceManager& operator=(RResourceManager&&) = delete;
    };

    template <class T>
    T* RResourceManager::tryLoad(const string& resourceTypeName)
    {
        if (auto* poly = resources_.tryGet(resourceTypeName))
        {
            poly->load();
            if (T* loaded = poly->as<T>())
            {
                return loaded;
            }
        }
        return nullptr;
    }
}
