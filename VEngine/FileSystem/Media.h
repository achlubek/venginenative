#pragma once
namespace VEngine
{
    namespace FileSystem
    {
        class Media
        {
        public:
            Media();
            ~Media();
            void scanDirectory(std::string path);
            std::string readString(std::string key);
            void saveFile(std::string path, int size, const void* data);
            std::string getPath(std::string key);
            bool exists(std::string key);
            int readBinary(std::string key, unsigned char** out_bytes);

        private:
            std::map<std::string, std::string> mediaMap;
            void searchRecursive(std::string path);
        };
    }
}