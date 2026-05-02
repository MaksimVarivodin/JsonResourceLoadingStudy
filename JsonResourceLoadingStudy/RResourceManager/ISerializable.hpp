#pragma once
#include <concepts>
#include <string>
#include <nlohmann/json.hpp>
#include <vector>

#include "../JournalistApi.hpp"

namespace journalist
{
    using namespace std;
    using namespace nlohmann;
    
   
    
    class JOURNALIST_API ISerializable
    {
    public:
        virtual ISerializable& toJson(json &j) = 0;
        virtual ISerializable& fromJson(const json &j) = 0;
        virtual ~ISerializable() = default;
    };

    class JOURNALIST_API ILoadableResource : public ISerializable
    {
    public:
        virtual void load() = 0;
    };

    template<typename T>
    concept Serializable = std::derived_from<T, ISerializable>;

    template<typename T>
    concept Primitive = std::is_arithmetic_v<T> || std::is_same_v<T, std::string> || std::is_same_v<T, json>;

    // Контейнеры бывают разные:
    //  - sequence (vector/list/...) имеют value_type
    //  - associative (map/...) имеют mapped_type (и value_type == pair<const K, V>)
    template<typename T>
    concept ResourceTypeContainer =
        // map-like: проверяем mapped_type
        (requires {
            typename T::mapped_type;
        } && (Primitive<typename T::mapped_type> || Serializable<typename T::mapped_type>))
        ||
        // sequence-like: проверяем value_type
        (requires {
            typename T::value_type;
        } && (Primitive<typename T::value_type> || Serializable<typename T::value_type>));

    template<typename T>
    concept ResourceType = Primitive<T> || Serializable<T> || ResourceTypeContainer<T>;
}
