#pragma once
#include <nlohmann/json.hpp>


namespace journalist
{
    template <Serializable T>
    json JsonLoadable<T>::loadJson()
    {
        if (path.data.has_value())
        {
            std::ifstream file(path.data.value());
            if (file.is_open())
            {
                json j;
                file >> j;
                return j;
            }
        }
        return {};
    }

    template <Serializable T>
    JsonLoadable<T>& JsonLoadable<T>::loadDataFromJson()
    {
        json j = loadJson();
        if (!j.empty())
        {
            data_.emplace();
            data_->fromJson(j);
        }
        return *this;
    }

    template <Serializable T>
    T JsonLoadable<T>::getData() const
    {
        return data_.value_or(T{}); 
    }

    template <Serializable T>
    ISerializable& JsonLoadable<T>::toJson(json& j)
    {
        path.toJson(j);
        if (data_.has_value())
        {
             // Usually we don't save the whole data back to the definition unless we want to embed inline.
             // But if we wanted to save the file itself, that would be a different method.
             // For now, let's keep it simple.
        }
        return *this;
    }

    template <Serializable T>
    ISerializable& JsonLoadable<T>::fromJson(const json& j)
    {
        path.fromJson(j);
        return *this;
    }
}
