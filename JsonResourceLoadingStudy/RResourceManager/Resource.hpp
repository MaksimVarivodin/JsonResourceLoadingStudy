#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <format>
#include <optional>
#include <concepts>
#include <vector>
#include <map>
#include "ISerializable.hpp"

namespace journalist
{
    using namespace std;
    using namespace nlohmann;

    template <std::size_t N>
    struct FixedString
    {
        char strValue[N];

        constexpr FixedString(const char (&str)[N])
        {
            std::copy_n(str, N, strValue);
        }
    };

    template <FixedString Key>
    struct ResourceTypeMap;

    template <FixedString key, ResourceType T = typename ResourceTypeMap<key>::type>
    struct Resource
    {
        optional<T> data = {};

        Resource& toJson(json& j)
        {
            if constexpr (std::is_same_v<T, json>)
            {
                j[key.strValue] = data;
            }
            else if constexpr (std::derived_from<T, ISerializable>)
            {
                if (data.has_value())
                {
                    // Для вложенных сериализуемых объектов сохраняем как объект под ключом.
                    // Это соответствует fromJson(), который читает из j.at(key).
                    json nested;
                    data.value().toJson(nested);
                    j[key.strValue] = std::move(nested);
                }
            }
            else
            {
                // Для примитивов (числа/строки) сохраняем как соответствующий JSON-тип,
                // а не как строку (иначе fromJson() для чисел сломается).
                j[key.strValue] = data.value_or(T{});
            }
            return *this;
        }

        Resource& fromJson(const json& j)
        {
            
            if constexpr (std::is_same_v<T, json>)
            {
                data = j.at(key.strValue);
            }
            else if constexpr (std::derived_from<T, ISerializable>)
            {
                if (j.contains(key.strValue))
                {
                    data.emplace().fromJson(j.at(key.strValue));
                }
            }

            else
            {
                data = j.at(key.strValue).template get<T>();
            }
            return *this;
        }

        Resource& operator()(const T& value)
        {
            data = value;
            return *this;
        }

        optional<T>& operator()()
        {
            return data;
        }

        const optional<T>& operator()() const
        {
            return data;
        }

        operator optional<T>() const
        {
            return data;
        }
    };

    template <FixedString key, typename T>
    struct Resource<key, std::vector<T>>
    {
        optional<vector<T>> data = {};

        Resource& toJson(json& j)
        {
            if (data.has_value())
            {
                if constexpr (std::derived_from<T, ISerializable>)
                {
                    json array = json::array();
                    for (auto& item : data.value())
                    {
                        json itemJson;
                        item.toJson(itemJson);
                        array.push_back(itemJson);
                    }
                    j[key.strValue] = array;
                }
                else
                {
                    j[key.strValue] = data.value();
                }
            }
            return *this;
        }

        Resource& fromJson(const json& j)
        {
            if (j.contains(key.strValue))
            {
                if constexpr (std::derived_from<T, ISerializable>)
                {
                    const json& array = j.at(key.strValue);
                    if (array.is_array())
                    {
                        data.emplace();
                        for (const auto& itemJson : array)
                        {
                            T item;
                            item.fromJson(itemJson);
                            data.value().push_back(std::move(item));
                        }
                    }
                }
                else
                {
                    data = j.at(key.strValue).template get<vector<T>>();
                }
            }
            return *this;
        }

        Resource& operator()(const vector<T>& value)
        {
            data = value;
            return *this;
        }

        optional<vector<T>>& operator()()
        {
            return data;
        }

        const optional<vector<T>>& operator()() const
        {
            return data;
        }

        operator optional<vector<T>>() const
        {
            return data;
        }
    };

    // Resource<key, std::map<K,V>>
    // Примечание: JSON object напрямую поддерживает только string-ключи.
    // Для остальных K используем массив объектов {"key":..., "value":...}.
    template <FixedString key, typename K, typename V, typename Compare, typename Alloc>
    struct Resource<key, std::map<K, V, Compare, Alloc>>
    {
        using map_type = std::map<K, V, Compare, Alloc>;

        optional<map_type> data = {};

        Resource& toJson(json& j)
        {
            if (!data.has_value())
                return *this;

            if constexpr (std::is_same_v<K, std::string>)
            {
                json obj = json::object();

                for (auto& [mk, mv] : data.value())
                {
                    if constexpr (std::derived_from<V, ISerializable>)
                    {
                        json vj;
                        mv.toJson(vj);
                        obj[mk] = std::move(vj);
                    }
                    else
                    {
                        obj[mk] = mv;
                    }
                }

                j[key.strValue] = std::move(obj);
            }
            else
            {
                json arr = json::array();

                for (auto& [mk, mv] : data.value())
                {
                    json item;
                    item["key"] = mk;

                    if constexpr (std::derived_from<V, ISerializable>)
                    {
                        json vj;
                        mv.toJson(vj);
                        item["value"] = std::move(vj);
                    }
                    else
                    {
                        item["value"] = mv;
                    }

                    arr.push_back(std::move(item));
                }

                j[key.strValue] = std::move(arr);
            }

            return *this;
        }

        Resource& fromJson(const json& j)
        {
            if (!j.contains(key.strValue))
                return *this;

            const json& src = j.at(key.strValue);
            data.emplace();
            data->clear();

            if constexpr (std::is_same_v<K, std::string>)
            {
                // Новый формат: объект { "id": {resource}, ... }
                if (src.is_object())
                {
                    for (auto it = src.begin(); it != src.end(); ++it)
                    {
                        const std::string& mk = it.key();
                        const json& vj = it.value();

                        if constexpr (std::derived_from<V, ISerializable>)
                        {
                            V mv{};
                            mv.fromJson(vj);
                            data->emplace(mk, std::move(mv));
                        }
                        else
                        {
                            data->emplace(mk, vj.get<V>());
                        }
                    }
                    return *this;
                }

                // Обратная совместимость: массив ресурсов [ {"type":"font", ...}, ...]
                // Ключ берём из поля "type" (если есть), иначе из "name".
                if (src.is_array())
                {
                    for (const auto& item : src)
                    {
                        if (!item.is_object())
                            continue;

                        std::string mk;
                        if (item.contains("type"))
                            mk = item.at("type").get<std::string>();
                        else if (item.contains("name"))
                            mk = item.at("name").get<std::string>();
                        else
                            continue;

                        if constexpr (std::derived_from<V, ISerializable>)
                        {
                            V mv{};
                            mv.fromJson(item);
                            (*data)[mk] = std::move(mv);
                        }
                        else
                        {
                            (*data)[mk] = item.get<V>();
                        }
                    }
                    return *this;
                }

                return *this;
            }
            else
            {
                // Для не-string ключей поддерживаем только массив {key,value}
                if (!src.is_array())
                    return *this;

                for (const auto& item : src)
                {
                    if (!item.is_object() || !item.contains("key") || !item.contains("value"))
                        continue;

                    K mk = item.at("key").get<K>();

                    if constexpr (std::derived_from<V, ISerializable>)
                    {
                        V mv{};
                        mv.fromJson(item.at("value"));
                        data->emplace(std::move(mk), std::move(mv));
                    }
                    else
                    {
                        data->emplace(std::move(mk), item.at("value").get<V>());
                    }
                }
            }

            return *this;
        }

        Resource& operator()(const map_type& value)
        {
            data = value;
            return *this;
        }

        optional<map_type>& operator()()
        {
            return data;
        }

        const optional<map_type>& operator()() const
        {
            return data;
        }

        operator optional<map_type>() const
        {
            return data;
        }

        // Удобный доступ: создаёт map при необходимости и создаёт элемент по operator[] у std::map.
        // Важно: это поведение как у std::map::operator[] (может добавить новый ключ).
        V& operator[](const K& k)
        {
            if (!data.has_value())
                data.emplace();
            return (*data)[k];
        }

        const V* tryGet(const K& k) const
        {
            if (!data.has_value())
                return nullptr;
            auto it = data->find(k);
            return (it == data->end()) ? nullptr : std::addressof(it->second);
        }

        V* tryGet(const K& k)
        {
            if (!data.has_value())
                return nullptr;
            auto it = data->find(k);
            return (it == data->end()) ? nullptr : std::addressof(it->second);
        }

        const V& at(const K& k) const
        {
            if (!data.has_value())
                throw std::out_of_range("Resource map is empty");
            return data->at(k);
        }

        V& at(const K& k)
        {
            if (!data.has_value())
                throw std::out_of_range("Resource map is empty");
            return data->at(k);
        }
    };

    template <FixedString key>
    struct Resource<key, ISerializable>;
}
