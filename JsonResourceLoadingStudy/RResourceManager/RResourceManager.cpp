#include "RResourceManager.hpp"

namespace journalist
{
    RResourceManager& RResourceManager::instance()
    {
        static RResourceManager staticInstance;

        return staticInstance;
    }

    RResourceManager& RResourceManager::registerParsers(const vector<pair<string, ParserFactory::Creator>>& parsers)
    {
        for (const auto& [type, creator] : parsers)
        {
            ParserFactory::instance().registerParser(type, creator);
        }
        return instance();
    }

    RResourceManager::RResourceManager()
    {
        ParserFactory::instance();
    }


    RResourceManager& RResourceManager::toJson(json& j)
    {
        type_.toJson(j);
        resources_.toJson(j);
        return *this;
    }

    RResourceManager& RResourceManager::fromJson(const json& j)
    {
        type_.fromJson(j);
        resources_.fromJson(j);
        return *this;
    }
}
