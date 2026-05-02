#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
// Adjust include path based on project structure
#include "../../JsonResourceLoadingStudy/JournalistLib.hpp"
#include "../../JsonResourceLoadingStudy/RResourceManager/RResourceManager.hpp"

using namespace std;
using namespace journalist;
using namespace nlohmann;

#include <vector>
#include <string>

namespace journalist
{
    struct CharData : ISerializable
    {
        Resource<"char", string> charCode = {};
        Resource<"path", string> path = {};

        ISerializable& toJson(json& j) override
        {
            charCode.toJson(j);
            path.toJson(j);
            return *this;
        }

        ISerializable& fromJson(const json& j) override
        {
            charCode.fromJson(j);
            path.fromJson(j);
            return *this;
        }
    };

    class FontData : public ISerializable
    {
    public:
        Resource<"name", string> name = {};
        Resource<"sizeX", int> sizeX = {};
        Resource<"sizeY", int> sizeY = {};
        Resource<"chars", vector<CharData>> chars = {};

        ISerializable& toJson(json& j) override
        {
            name.toJson(j);
            sizeX.toJson(j);
            sizeY.toJson(j);
            chars.toJson(j);
            return *this;
        }

        ISerializable& fromJson(const json& j) override
        {
            name.fromJson(j);
            sizeX.fromJson(j);
            sizeY.fromJson(j);
            chars.fromJson(j);
            return *this;
        }
    };

    class FontResource : public JsonLoadable<FontData>
    {
    public:
        // We can add specific logic here if needed.
        // For example, register itself in constructor or static init.

        static std::unique_ptr<ILoadableResource> create()
        {
            return std::make_unique<FontResource>();
        }
    };
}


json loadJson(const string& path)
{
    ifstream file(path);
    json j;
    if (file.is_open())
    {
        file >> j;
        file.close();
    }
    else
    {
        cout << "Could not open /'"<< path << "/'" << endl;
    }
    return j;
}

void fu()
{

    auto j = loadJson("example/resources.json");

    RResourceManager::instance()
        .registerParsers({
            {"font", []() { return FontResource::create(); }}
        })
        .fromJson(j);
    cout << RResourceManager::instance().getRType().getType() << endl;
    auto* font = RResourceManager::instance().tryLoad<FontResource>("font");
}

int main(int argc, char* argv[])
{
    fu();
    system("pause");
    return 0;
}
