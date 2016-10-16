#pragma once
#define SHADER_VARIABLE_TYPE_FLOAT 1
#define SHADER_VARIABLE_TYPE_INT 2
#define SHADER_VARIABLE_TYPE_VEC2 3
#define SHADER_VARIABLE_TYPE_IVEC2 4
#define SHADER_VARIABLE_TYPE_VEC3 5
#define SHADER_VARIABLE_TYPE_IVEC3 6
#define SHADER_VARIABLE_TYPE_VEC4 7
#define SHADER_VARIABLE_TYPE_IVEC4 8 
#include <unordered_map>
class ShaderProgram
{
public:
    class ShaderProgramVariable {
    public:
        int type = SHADER_VARIABLE_TYPE_FLOAT;
        float var_float = 1.0f;
        int var_int = 1;
        glm::vec2 var_vec2 = glm::vec2(1);
        glm::ivec2 var_ivec2 = glm::ivec2(1);
        glm::vec3 var_vec3 = glm::vec3(1);
        glm::ivec3 var_ivec3 = glm::ivec3(1);
        glm::vec4 var_vec4 = glm::vec4(1);
        glm::ivec4 var_ivec4 = glm::ivec4(1);
    };
    ShaderProgram(string vertex, string fragment, string geometry = "", string tesscontrol = "", string tesseval = "");
    ShaderProgram(string compute);
    ~ShaderProgram();
    map<string, ShaderProgramVariable*> shaderVariables = {};
    void recompile();
    void use();
    void dispatch(GLuint groups_x, GLuint groups_y, GLuint groups_z);
    void setUniform(const string &name, const GLint &value);
    void setUniform(const string &name, const GLuint &value);

    void setUniform(const string &name, const float &value);
    void setUniform(const string &name, const bool &value);

    void setUniform(const string &name, const glm::vec2 &value);
    void setUniform(const string &name, const glm::vec3 &value);
    void setUniform(const string &name, const glm::vec4 &value);

    void setUniform(const string &name, const glm::mat3 &value);
    void setUniform(const string &name, const glm::mat4 &value);
    void setUniform(const string &name, const glm::quat &value);

    void setUniformVector(const string &name, const vector<GLint> &value);
    void setUniformVector(const string &name, const vector<GLuint> &value);

    void setUniformVector(const string &name, const vector<float> &value);

    void setUniformVector(const string &name, const vector<glm::vec2> &value);
    void setUniformVector(const string &name, const vector<glm::vec3> &value);
    void setUniformVector(const string &name, const vector<glm::vec4> &value);

    void setUniformVector(const string &name, const vector<glm::mat3> &value);
    void setUniformVector(const string &name, const vector<glm::mat4> &value);
    void setUniformVector(const string &name, const vector<glm::quat> &value);

    static ShaderProgram *current;

private:

    bool generated = false;
    GLuint handle;
    string vertexFile;
    string fragmentFile;
    string geometryFile;
    string tessControlFile;
    string tessEvalFile;
    string computeFile;
    std::unordered_map<string, GLint> uniformLocationsMap;

    GLint getUniformLocation(const string &name);
    void compile();
    string resolveIncludes(string source);
    string resolveVariables(string source);
    GLuint compileSingleShader(GLenum type, string filename, string source);
};
