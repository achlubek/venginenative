#pragma once
#include <string>

namespace VEngine
{
    namespace FileSystem
    {
        class MediaInterface
        {
        public:
            virtual void scanDirectory(std::string path) = 0;
            virtual std::string readString(std::string key) = 0;
            virtual void saveFile(std::string path, int size, const void* data) = 0;
            virtual std::string getPath(std::string key) = 0;
            virtual bool exists(std::string key) = 0;
            virtual int readBinary(std::string key, unsigned char** out_bytes) = 0;
        };
    }
}