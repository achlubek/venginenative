#pragma once
#include "../Interface/FileSystem/MediaInterface.h"

namespace VEngine
{
    namespace FileSystem
    {
        class Media : public MediaInterface
        {
        public:
            Media();
            ~Media();
            virtual void scanDirectory(std::string path) override;
            virtual std::string readString(std::string key) override;
            virtual void saveFile(std::string path, int size, const void* data) override;
            virtual std::string getPath(std::string key) override;
            virtual bool exists(std::string key) override;
            virtual int readBinary(std::string key, unsigned char** out_bytes) override;

        private:
            std::map<std::string, std::string> mediaMap;
            void searchRecursive(std::string path);
        };
    }
}