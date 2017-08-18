#pragma once
#include "Mesh3d.h"
#include "Scene.h"
#include "Material.h"
#include "Object3dInfo.h"
class AssetLoader
{
public:
    AssetLoader();
    ~AssetLoader();

    Object3dInfo *loadObject3dInfoFile(string source);
	/*
	Skeleton *loadSkeletonString(string source);
	Skeleton *loadSkeletonFile(string source);
	*/
    Material *loadMaterialString(string source);
    Material *loadMaterialFile(string source);

    Mesh3d *loadMeshString(string source);
    Mesh3d *loadMeshFile(string source);
	/*
    Light *loadLightString(string source);
    Light *loadLightFile(string source);
	*/
    Scene *loadSceneString(string source);
    Scene *loadSceneFile(string source);

private:
    void splitByLines(vector<string>& output, string src);
	void splitBySpaces(vector<string>& output, string src);
	void splitByChar(vector<string>& output, string src, char c);
    int replaceEnum(string enumstr);
};
