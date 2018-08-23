#include "stdafx.h"
#include "Media.h"
#include "tinydir.h"

namespace VEngine
{
    namespace FileSystem
    {
        using namespace std;

        static int fsize(FILE* fh) {
            int prev = ftell(fh);
            fseek(fh, 0L, SEEK_END);
            int sz = ftell(fh);
            fseek(fh, prev, SEEK_SET);
            return sz;
        }

        char* get_file_contents(const char* path) {
            FILE* fh = fopen(path, "r");
            int size = fsize(fh);
            char* content = (char*)calloc(size + 1, sizeof(char));
            size_t sz = fread(content, sizeof(char), size, fh);
            content = (char*)realloc(content, sizeof(char) * sz + 1);
            content[sz] = 0;
            fclose(fh);
            return content;
        }

        int get_file_contents_binary(unsigned char** out_bytes, const char* path) {
            FILE* fh = fopen(path, "rb");
            int size = fsize(fh);
            unsigned char* content = (unsigned char*)calloc(size, 1);
            size_t sz = fread(content, 1, size, fh);
            fclose(fh);
            *out_bytes = content;
            return size;
        }

        int put_file_contents_binary(const void* out_bytes, int size, const char* path) {
            FILE* fh = fopen(path, "wb+");
            fwrite(out_bytes, 1, size, fh);
            fclose(fh);
            return size;
        }

        Media::Media()
            : mediaMap({})
        {
        }

        Media::~Media()
        {
        }

        void Media::scanDirectory(string path)
        {
            searchRecursive(path);
        }
        
        string Media::readString(string key)
        {
            return string(get_file_contents(getPath(key).c_str()));
        }

        void Media::saveFile(string path, int size, const void * data)
        {
            put_file_contents_binary(data, size, path.c_str());
        }

        string Media::getPath(string key)
        {
            if (mediaMap.find(key) == mediaMap.end()) {
                throw std::runtime_error("Could find media file " + key);
            }
            return mediaMap.at(key);
        }

        bool Media::exists(string key)
        {
            if (mediaMap.find(key) == mediaMap.end())
                return false;
            return true;
        }

        int Media::readBinary(string key, unsigned char** out_bytes)
        {
            return get_file_contents_binary(out_bytes, getPath(key).c_str());
        }

        void Media::searchRecursive(string path)
        {
            vector<string> foundDirs;
            tinydir_dir dir;
            int i;
            tinydir_open_sorted(&dir, path.c_str());

            for (i = 0; i < dir.n_files; i++)
            {
                tinydir_file file;
                tinydir_readfile_n(&dir, &file, i);

                if (file.name[0] != '.' && (file.name[0] != '/' && file.name[1] != '.')) {
                    if (file.is_dir)
                    {
                        foundDirs.push_back(file.path);
                    }
                    else {
                        string s = file.path;
                        size_t c = s.length() - 1;
                        while (s[c] != '/' && c > 0) c--;
                        c--;
                        if (c < 0) c = 0;
                        while (s[c] != '/' && c > 0) c--; // twice
                        c++;
                        s = s.substr(c);
                        mediaMap[s] = file.path;
                        mediaMap[file.name] = file.path;
                    }
                }
            }
            tinydir_close(&dir);
            for (int i = 0; i < foundDirs.size(); i++)
                searchRecursive(foundDirs[i]);
            foundDirs.clear();
        }

    }
}