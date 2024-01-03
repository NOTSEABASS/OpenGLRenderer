#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

class Shader
{
public:
    unsigned int ID;
    std::string vertexPath;
    std::string fragmentPath;
    std::string name;
    static std::map<std::string, Shader *> LoadedShaders;
    bool is_editor = false;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(std::filesystem::path _vertexPath, std::filesystem::path _fragmentPath, bool _is_editor = false) : is_editor(_is_editor)
    {
        std::string vert_str = _vertexPath.string();
        std::string frag_str = _fragmentPath.string();
        std::replace(vert_str.begin(), vert_str.end(), '\\', '/');
        std::replace(frag_str.begin(), frag_str.end(), '\\', '/');
        std::cout << "Load Shader From: [vert]"
                  << vert_str << "\n                  [frag]"
                  << frag_str << std::endl;
        vertexPath = vert_str;
        fragmentPath = frag_str;
    }

    Shader(const char *_vertexPath, const char *_fragmentPath, bool _is_editor = false) : is_editor(_is_editor)
    {
        std::string vert_str = _vertexPath;
        std::string frag_str = _fragmentPath;
        std::replace(vert_str.begin(), vert_str.end(), '\\', '/');
        std::replace(frag_str.begin(), frag_str.end(), '\\', '/');
        std::cout << "Load Shader From: [vert]"
                  << vert_str << "\n                  [frag]"
                  << frag_str << std::endl;
        vertexPath = vert_str;
        fragmentPath = frag_str;
    }

    ~Shader()
    {
        DeleteShader();
        LoadedShaders.erase(name);
    }

    void Refresh()
    {
        std::cout << "Refresh Shader:"
                  << "[vert]" << vertexPath << "\n               [frag]" << fragmentPath << std::endl;
        LoadShader(vertexPath.c_str(), fragmentPath.c_str());
    }

    void LoadShader()
    {
        LoadShader(vertexPath.c_str(), fragmentPath.c_str());
    }

    void LoadShader(const char *vertexPath, const char *fragmentPath)
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch (std::ifstream::failure &e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex_shader, fragment_shader;
        // vertex shader
        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vShaderCode, NULL);
        glCompileShader(vertex_shader);
        checkCompileErrors(vertex_shader, "VERTEX");
        // fragment Shader
        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fShaderCode, NULL);
        glCompileShader(fragment_shader);
        checkCompileErrors(fragment_shader, "FRAGMENT");
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex_shader);
        glAttachShader(ID, fragment_shader);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);

        std::string vert_str = vertexPath;
        std::string frag_str = fragmentPath;
        std::string name_s = vert_str.substr(vert_str.find_last_of('/') + 1, vert_str.size());
        std::string name_f = frag_str.substr(frag_str.find_last_of('/') + 1, frag_str.size());
        name = name_f;
        LoadedShaders.insert(std::map<std::string, Shader *>::value_type(name_f, this));
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use()
    {
        glUseProgram(ID);
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

    void DeleteShader()
    {
        std::cout << "Remove Shader: [vert]" << vertexPath << " [frag]" << fragmentPath << std::endl;
        glDeleteProgram(ID);
    }
};