#pragma once
#include "../JournalistApi.hpp"
#include "ISerializable.hpp"
#include "Resource.hpp"
namespace journalist
{
    class JOURNALIST_API RType : public ISerializable
    {
        Resource<"type", string> resourceType = {};
        
    public:
        
        RType& toJson(json& j) override
        {
            resourceType.toJson(j);
            return *this;
        }
        RType& fromJson(const json& j) override
        {
            resourceType.fromJson(j);
            return *this;
        }
        string getType() const { return resourceType.data.value_or({}); }
        
        ~RType() override {}
    };
}

