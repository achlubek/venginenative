#pragma once
namespace VEngine
{
    namespace FileSystem
    {
        class Media // TODO this is the last singleton, refactor
        {
        public:
            static void loadFileMap(std::string path);
            static std::string readString(std::string key);
            static void saveFile(std::string path, int size, const void* data);
            static std::string getPath(std::string key);
            static bool exists(std::string key);
            static int readBinary(std::string key, unsigned char** out_bytes);
            static void saveCache(std::string key, void* data);
            static void* checkCache(std::string key);

        private:

            static std::map<std::string, std::string> mediaMap;
            static void searchRecursive(std::string path);
            static std::map<std::string, void*> cache;
        };
    }
}