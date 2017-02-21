#include "stdafx.h"
#include "AssetLoader.h"
#include "Media.h"

AssetLoader::AssetLoader()
{
}

AssetLoader::~AssetLoader()
{
}

Object3dInfo * AssetLoader::loadObject3dInfoFile(string source)
{
    void * cached = Media::checkCache(source);
    if (cached != nullptr) {
        return (Object3dInfo*)cached;
    }
    unsigned char* bytes;
    int bytescount = Media::readBinary(source, &bytes);
    GLfloat * floats = (GLfloat*)bytes;
    int floatsCount = bytescount / 4;
    vector<GLfloat> flo(floats, floats + floatsCount);

    auto o = new Object3dInfo(flo);
    Media::saveCache(source, o);
    return o;
    
}

Material * AssetLoader::loadMaterialString(string source)
{
    vector<string> materialLines;
    splitByLines(materialLines, source);
    Material *material = new Material();
    MaterialNode *node = nullptr;
    for (int i = 0; i < materialLines.size(); i++) {
        vector<string> words;
        splitBySpaces(words, materialLines[i]);
        if (words[0] == "diffuse") {
            if (words.size() == 2) {
                material->diffuseColor = glm::vec3(atof(words[1].c_str()));
            }
            if (words.size() == 4) {
                material->diffuseColor = glm::vec3(
                    atof(words[1].c_str()),
                    atof(words[2].c_str()),
                    atof(words[3].c_str())
                );
            }
        }
        if (words[0] == "roughness") {
            if (words.size() == 2) {
                material->roughness = atof(words[1].c_str());
            }
        }
        if (words[0] == "metalness") {
            if (words.size() == 2) {
                material->metalness = atof(words[1].c_str());
            }
        }
        // now nodes section
        if (words[0] == "node") {
            if (node != nullptr) {
                if (node->target == NODE_TARGET_DIFFUSE) {
                    material->diffuseColorTex = node->texture;
                    material->diffuseColorTexScale = node->uvScale;
                }
                if (node->target == NODE_TARGET_NORMAL) {
                    material->normalTex = node->texture;
                    material->normalTexScale = node->uvScale;
                }
                if (node->target == NODE_TARGET_BUMP) {
                    material->bumpTex = node->texture;
                    material->bumpTexScale = node->uvScale;
                }
                if (node->target == NODE_TARGET_ROUGHNESS) {
                    material->roughnessTex = node->texture;
                    material->roughnessTexScale = node->uvScale;
                }
                if (node->target == NODE_TARGET_METALNESS) {
                    material->metalnessTex = node->texture;
                    material->metalnessTexScale = node->uvScale;
                }
            }
            node = new MaterialNode();
        }
        if (words[0] == "uvscale") {
            if (node != nullptr) {
                if (words.size() == 2) {
                    node->uvScale = glm::vec2(atof(words[1].c_str()));
                }
                if (words.size() == 3) {
                    node->uvScale = glm::vec2(
                        atof(words[1].c_str()),
                        atof(words[2].c_str())
                    );
                }
            }
        }
        if (words[0] == "data") {
            if (node != nullptr) {
                if (words.size() == 2) {
                    node->data = glm::vec4(atof(words[1].c_str()), 0, 0, 0);
                }
                if (words.size() == 3) {
                    node->data = glm::vec4(
                        atof(words[1].c_str()),
                        atof(words[2].c_str()),
                        0, 0
                    );
                }
                if (words.size() == 4) {
                    node->data = glm::vec4(
                        atof(words[1].c_str()),
                        atof(words[2].c_str()),
                        atof(words[3].c_str()),
                        0
                    );
                }
                if (words.size() == 5) {
                    node->data = glm::vec4(
                        atof(words[1].c_str()),
                        atof(words[2].c_str()),
                        atof(words[3].c_str()),
                        atof(words[4].c_str())
                    );
                }
            }
        }
        if (words[0] == "color") {
            if (node != nullptr) {
                if (words.size() == 2) {
                    float f = atof(words[1].c_str());
                    node->color = glm::vec4(f, f, f, 1.0);
                }
                if (words.size() == 4) {
                    node->color = glm::vec4(
                        atof(words[1].c_str()),
                        atof(words[2].c_str()),
                        atof(words[3].c_str()),
                        1.0
                    );
                }
                if (words.size() == 5) {
                    node->color = glm::vec4(
                        atof(words[1].c_str()),
                        atof(words[2].c_str()),
                        atof(words[3].c_str()),
                        atof(words[4].c_str())
                    );
                }
            }
        }

        if (words[0] == "texture") {
            if (node != nullptr) {
                if (words.size() >= 2) {
                    stringstream ss;
                    for (int a = 1; a < words.size(); a++)ss << (a == 1 ? "" : " ") << words[a];
                    void * cached = Media::checkCache(ss.str());
                    if (cached != nullptr) {
                        node->texture = (Texture2d*)cached;
                    }
                    else {
                        node->texture = new Texture2d(ss.str());
                        Media::saveCache(ss.str(), node->texture);
                    }
                }
            }
        }
        if (words[0] == "mix") {
            if (node != nullptr) {
                if (words.size() >= 2) {
                    node->mixingMode = replaceEnum(words[1]);
                }
            }
        }
        if (words[0] == "target") {
            if (node != nullptr) {
                if (words.size() >= 2) {
                    node->target = replaceEnum(words[1]);
                }
            }
        }
        if (words[0] == "source") {
            if (node != nullptr) {
                if (words.size() >= 2) {
                    node->source = replaceEnum(words[1]);
                }
            }
        }
        if (words[0] == "modifier") {
            if (node != nullptr) {
                if (words.size() >= 2) {
                    node->modifierflags |= replaceEnum(words[1]);
                }
            }
        }
    }
    if (node != nullptr) {
        if (node->target == NODE_TARGET_DIFFUSE) {
            material->diffuseColorTex = node->texture;
            material->diffuseColorTexScale = node->uvScale;
        }
        if (node->target == NODE_TARGET_NORMAL) {
            material->normalTex = node->texture;
            material->normalTexScale = node->uvScale;
        }
        if (node->target == NODE_TARGET_BUMP) {
            material->bumpTex = node->texture;
            material->bumpTexScale = node->uvScale;
        }
        if (node->target == NODE_TARGET_ROUGHNESS) {
            material->roughnessTex = node->texture;
            material->roughnessTexScale = node->uvScale;
        }
        if (node->target == NODE_TARGET_METALNESS) {
            material->metalnessTex = node->texture;
            material->metalnessTexScale = node->uvScale;
        }
    }
    return material;
}

Material * AssetLoader::loadMaterialFile(string source)
{
    return loadMaterialString(Media::readString(source));
}

Mesh3d * AssetLoader::loadMeshString(string source)
{
    vector<string> meshLines;
    splitByLines(meshLines, source);
    Mesh3d *mesh = new Mesh3d();
    Mesh3dLodLevel *lodlevel = nullptr;
    Mesh3dInstance *instance = nullptr;
    for (int i = 0; i < meshLines.size(); i++) {
        vector<string> words;
        splitBySpaces(words, meshLines[i]);
        if (words[0] == "lodlevel") {
            if (lodlevel != nullptr) mesh->addLodLevel(lodlevel);
            lodlevel = new Mesh3dLodLevel();
        }
        if (words[0] == "instance") {
            if (instance != nullptr) mesh->addInstance(instance);
            instance = new Mesh3dInstance();
        }
        if (words[0] == "start") {
            if (lodlevel != nullptr) {
                if (words.size() == 2) {
                    lodlevel->distanceStart = atof(words[1].c_str());
                }
            }
        }
        if (words[0] == "end") {
            if (lodlevel != nullptr) {
                if (words.size() == 2) {
                    lodlevel->distanceEnd = atof(words[1].c_str());
                }
            }
        }
        if (words[0] == "info3d") {
            if (lodlevel != nullptr) {
                if (words.size() >= 2) {
                    stringstream ss;
                    for (int a = 1; a < words.size(); a++)ss << (a == 1 ? "" : " ") << words[a];

                    void * cached = Media::checkCache(ss.str());
                    if (cached != nullptr) {
                        lodlevel->info3d = (Object3dInfo*)cached;
                    }
                    else {
                        unsigned char* bytes;
                        int bytescount = Media::readBinary(ss.str(), &bytes);
                        GLfloat * floats = (GLfloat*)bytes;
                        int floatsCount = bytescount / 4;
                        vector<GLfloat> flo(floats, floats + floatsCount);

                      /*  cout << ss.str() << endl;
                        for (int i = 0; i < floatsCount; i++) {
                            if (i % 12 == 0) cout << endl;
                            cout << flo[i] << " " ;
                        }*/

                        lodlevel->info3d = new Object3dInfo(flo);
                        Media::saveCache(ss.str(), lodlevel->info3d);
                    }
                }
            }
        }
        if (words[0] == "material") {
            if (lodlevel != nullptr) {
                if (words.size() >= 2) {
                    stringstream ss;
                    for (int a = 1; a < words.size(); a++)ss << (a == 1 ? "" : " ") << words[a];

                    lodlevel->material = loadMaterialFile(ss.str());
                }
            }
        }
        if (words[0] == "translate") {
            if (instance != nullptr) {
                if (words.size() == 4) {
                    instance->transformation->translate(glm::vec3(
                        atof(words[1].c_str()),
                        atof(words[2].c_str()),
                        atof(words[3].c_str())
                    ));
                }
            }
        }
        if (words[0] == "scale") {
            if (instance != nullptr) {
                if (words.size() == 2) {
                    instance->transformation->scale(glm::vec3(
                        atof(words[1].c_str())
                    ));
                }
                if (words.size() == 4) {
                    instance->transformation->scale(glm::vec3(
                        atof(words[1].c_str()),
                        atof(words[2].c_str()),
                        atof(words[3].c_str())
                    ));
                }
            }
        }
        if (words[0] == "rotate") {
            if (instance != nullptr) {
                if (words.size() == 5) {
                    glm::quat qt;
                    qt.x = atof(words[1].c_str());
                    qt.y = atof(words[2].c_str());
                    qt.z = atof(words[3].c_str());
                    qt.w = atof(words[4].c_str());
                    instance->transformation->rotate(qt);
                }
            }
        }
        if (words[0] == "rotate_axis") {
            if (instance != nullptr) {
                if (words.size() == 5) {
                    instance->transformation->rotate(glm::angleAxis(
                        (float)deg2rad(atof(words[1].c_str())),
                        glm::vec3(
                            atof(words[2].c_str()),
                            atof(words[3].c_str()),
                            atof(words[4].c_str())
                        )
                    ));
                }
            }
        }
    }
    if (lodlevel != nullptr) mesh->addLodLevel(lodlevel);
    if (instance != nullptr) mesh->addInstance(instance);
    return mesh;
}

Mesh3d * AssetLoader::loadMeshFile(string source)
{
    return loadMeshString(Media::readString(source));
}

Light * AssetLoader::loadLightString(string source)
{
    vector<string> meshLines;
    splitByLines(meshLines, source);
    Light *light = new Light();
    for (int i = 0; i < meshLines.size(); i++) {
        vector<string> words;
        splitBySpaces(words, meshLines[i]);
        if (words[0] == "cutoff") {
            if (words.size() == 2) {
                light->cutOffDistance = atof(words[1].c_str());
            }
        }
        if (words[0] == "angle") {
            if (words.size() == 2) {
                light->angle = atof(words[1].c_str());
            }
        }
        if (words[0] == "type") {
            if (words.size() == 2) {
                if (words[1] == "SPOT")light->type = LIGHT_SPOT;
                if (words[1] == "POINT")light->type = LIGHT_POINT;
                if (words[1] == "AMBIENT")light->type = LIGHT_AMBIENT;
            }
        }
        if (words[0] == "shadowmap") {
            if (words.size() == 2) {
                light->switchShadowMapping(words[1] == "TRUE" ? true : false);
            }
        }
        if (words[0] == "resolution") {
            if (words.size() == 3) {
                light->resizeShadowMap(
                    atof(words[1].c_str()),
                    atof(words[2].c_str())
                );
            }
        }
        if (words[0] == "color") {
            if (words.size() == 2) {
                light->color = glm::vec3(
                    atof(words[1].c_str())
                );
            }
            if (words.size() == 4) {
                light->color = glm::vec3(
                    atof(words[1].c_str()),
                    atof(words[2].c_str()),
                    atof(words[3].c_str())
                );
            }
        }
        if (words[0] == "multiply") {
            if (words.size() == 2) {
                light->color *= glm::vec3(
                    atof(words[1].c_str())
                );
            }
            if (words.size() == 4) {
                light->color *= glm::vec3(
                    atof(words[1].c_str()),
                    atof(words[2].c_str()),
                    atof(words[3].c_str())
                );
            }
        }

        if (words[0] == "translate") {
            if (words.size() == 4) {
                light->transformation->translate(glm::vec3(
                    atof(words[1].c_str()),
                    atof(words[2].c_str()),
                    atof(words[3].c_str())
                ));
            }
        }
        if (words[0] == "rotate") {
            if (words.size() == 5) {
                light->transformation->rotate(glm::quat(
                    atof(words[1].c_str()),
                    atof(words[2].c_str()),
                    atof(words[3].c_str()),
                    atof(words[4].c_str())
                ));
            }
        }
        if (words[0] == "rotate_axis") {
            if (words.size() == 5) {
                light->transformation->rotate(glm::angleAxis(
                    (float)deg2rad(atof(words[1].c_str())),
                    glm::vec3(
                        atof(words[2].c_str()),
                        atof(words[3].c_str()),
                        atof(words[4].c_str())
                    )
                ));
            }
        }
        if (words[0] == "lookat") {
            if (words.size() == 4) {
                light->transformation->setOrientation(glm::quat_cast(glm::lookAt(
                    light->transformation->getPosition(),
                    glm::vec3(
                        atof(words[1].c_str()),
                        atof(words[2].c_str()),
                        atof(words[3].c_str())
                    ),
                    glm::vec3(0, 1, 0)
                )));
            }
        }
    }
    return light;
}

Light * AssetLoader::loadLightFile(string source)
{
    return loadLightString(Media::readString(source));
}

Scene * AssetLoader::loadSceneString(string source)
{
    vector<string> meshLines;
    splitByLines(meshLines, source);
    Scene *scene = new Scene();
    for (int i = 0; i < meshLines.size(); i++) {
        vector<string> words;
        splitBySpaces(words, meshLines[i]);
        if (words[0] == "mesh3d") {
            if (words.size() >= 2) {
                stringstream ss;
                for (int a = 1; a < words.size(); a++)ss << (a == 1 ? "" : " ") << words[a];
                scene->addMesh3d(loadMeshFile(ss.str()));
            }
        }
        if (words[0] == "light") {
            if (words.size() >= 2) {
                stringstream ss;
                for (int a = 1; a < words.size(); a++)ss << (a == 1 ? "" : " ") << words[a];
                scene->addLight(loadLightFile(ss.str()));
            }
        }
    }
    return scene;
}

Scene * AssetLoader::loadSceneFile(string source)
{
    return loadSceneString(Media::readString(source));
}

// this looks like c
void AssetLoader::splitByLines(vector<string>& output, string src)
{
    int i = 0, d = 0;
    while (i < src.size()) {
        if (src[i] == '\n') {
            output.push_back(src.substr(d, i - d));
            d = i;
            while (src[i++] == '\n')  d++;
        }
        else {
            i++;
        }
    }
    if (i == src.size() && d < i) {
        output.push_back(src.substr(d, i));
    }
}

// this looks like c
void AssetLoader::splitBySpaces(vector<string>& output, string src)
{
    int i = 0, d = 0;
    while (i < src.size()) {
        if (src[i] == ' ') {
            output.push_back(src.substr(d, i - d));
            d = i;
            while (src[i++] == ' ')  d++;
        }
        else {
            i++;
        }
    }
    if (i == src.size() && d < i) {
        output.push_back(src.substr(d, i));
    }
}

int AssetLoader::replaceEnum(string enumstr)
{
    if (enumstr == "ADD") return NODE_MODE_ADD;
    if (enumstr == "MUL") return NODE_MODE_MUL;
    if (enumstr == "AVERAGE") return NODE_MODE_AVERAGE;
    if (enumstr == "SUB") return NODE_MODE_SUB;
    if (enumstr == "ALPHA") return NODE_MODE_ALPHA;
    if (enumstr == "ONE_MINUS_ALPHA") return NODE_MODE_ONE_MINUS_ALPHA;
    if (enumstr == "REPLACE") return NODE_MODE_REPLACE;
    if (enumstr == "MAX") return NODE_MODE_MAX;
    if (enumstr == "MIN") return NODE_MODE_MIN;
    if (enumstr == "DISTANCE") return NODE_MODE_DISTANCE;

    if (enumstr == "ORIGINAL") return NODE_MODIFIER_ORIGINAL;
    if (enumstr == "NEGATIVE") return NODE_MODIFIER_NEGATIVE;
    if (enumstr == "LINEARIZE") return NODE_MODIFIER_LINEARIZE;
    if (enumstr == "SATURATE") return NODE_MODIFIER_SATURATE;
    if (enumstr == "HUE") return NODE_MODIFIER_HUE;
    if (enumstr == "BRIGHTNESS") return NODE_MODIFIER_BRIGHTNESS;
    if (enumstr == "POWER") return NODE_MODIFIER_POWER;
    if (enumstr == "HSV") return NODE_MODIFIER_HSV;

    if (enumstr == "DIFFUSE") return NODE_TARGET_DIFFUSE;
    if (enumstr == "NORMAL") return NODE_TARGET_NORMAL;
    if (enumstr == "ROUGHNESS") return NODE_TARGET_ROUGHNESS;
    if (enumstr == "METALNESS") return NODE_TARGET_METALNESS;
    if (enumstr == "BUMP") return NODE_TARGET_BUMP;
    if (enumstr == "BUMP_NORMAL") return NODE_TARGET_BUMP_AS_NORMAL;
    if (enumstr == "DISPLACEMENT") return NODE_TARGET_DISPLACEMENT;

    if (enumstr == "COLOR") return NODE_SOURCE_COLOR;
    if (enumstr == "TEXTURE") return NODE_SOURCE_TEXTURE;
    return 0;
}