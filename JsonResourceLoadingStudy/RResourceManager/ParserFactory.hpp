#pragma once
#include <map>
#include <functional>
#include <memory>
#include <string>
#include "ISerializable.hpp"
#include <stdexcept>

namespace journalist
{
    using namespace std;

    class ParserFactory
    {
    public:
        using Creator = std::function<std::unique_ptr<ILoadableResource>()>;

        static ParserFactory& instance()
        {
            static ParserFactory inst;
            return inst;
        }

        void registerParser(const std::string& type, Creator creator)
        {
            creators[type] = creator;
        }

        std::unique_ptr<ILoadableResource> createParser(const std::string& type)
        {
            auto it = creators.find(type);
            if (it != creators.end())
            {
                return it->second();
            }
            return nullptr;
        }
        

    private:
        std::map<std::string, Creator> creators;
        ParserFactory() = default;
    };
}

