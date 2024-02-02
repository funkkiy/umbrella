#include "UmbrellaApplication.h"

#include <unordered_map>
#include <vector>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <tiny_obj_loader.h>

#include "gfx/ShaderProgram.h"
#include "util/File.h"
#include "util/Framework.h"

namespace Umbrella {

InitializeResult UmbrellaApplication::Initialize()
{
    if (!glfwInit()) {
        return InitializeResult::GLFWInitFail;
    }

    // Create a window with a OpenGL 4.6 Core context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    constexpr int defaultWidth = 640;
    constexpr int defaultHeight = 480;
    m_window = glfwCreateWindow(
        defaultWidth, defaultHeight, "Umbrella", nullptr, nullptr);
    if (!m_window) {
        Stop();
        return InitializeResult::WindowCreationFail;
    }
    glfwMakeContextCurrent(m_window);

    m_windowWidth = defaultWidth;
    m_windowHeight = defaultHeight;

    // Resize the Viewport, in case the window size changes
    glfwSetWindowUserPointer(m_window, this);
    glfwSetWindowSizeCallback(
        m_window, [](GLFWwindow* window, int width, int height) {
            auto app = static_cast<UmbrellaApplication*>(
                glfwGetWindowUserPointer(window));
            app->m_windowWidth = width;
            app->m_windowHeight = height;
            glViewport(0, 0, width, height);
        });

    if (!gladLoadGL(glfwGetProcAddress)) {
        Stop();
        return InitializeResult::GLADLoaderFail;
    }

    glfwSwapInterval(1);

    return InitializeResult::InitializeOk;
}

PrepareResult UmbrellaApplication::Prepare()
{
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(
        [](GLenum source, GLenum type, GLuint id, GLenum severity,
            GLsizei length, const GLchar* msg,
            const void* userParam) {
                spdlog::warn("{}", msg);
        },
        nullptr);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    std::optional<std::string> vertexSrc, fragSrc;
    vertexSrc = Umbrella::Util::ReadFile("shaders/VertexShader.glsl");
    fragSrc = Umbrella::Util::ReadFile("shaders/FragShader.glsl");
    if (!vertexSrc || !fragSrc) {
        return PrepareResult::SourceReadFail;
    }

    std::optional<GLuint> shaderProgram
        = Umbrella::Gfx::CompileProgram(*vertexSrc, *fragSrc);
    if (shaderProgram) {
        m_shaderProgram = *shaderProgram;
    } else {
        return PrepareResult::ShaderBuildFail;
    }

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string objWarn, objError;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &objWarn, &objError,
            "meshes/suzanne_smooth.obj", "meshes/")) {
        if (!objError.empty()) {
            spdlog::error("TinyObjLoader error: {}", objError);
        }
        return PrepareResult::ObjLoadFail;
    }
    if (!objWarn.empty()) {
        spdlog::warn("TinyObjLoader warning: {}", objWarn);
    }

    struct VertexAttributes {
        float x, y, z;
        float u, v;
        float nx, ny, nz;

        bool operator==(const VertexAttributes& other) const
        {
            return x == other.x && y == other.y && z == other.z && u == other.u
                && v == other.v && nx == other.nx && ny == other.ny
                && nz == other.nz;
        }
    };

    struct VertexAttributesHasher {
        size_t operator()(const VertexAttributes& va) const
        {
            return std::hash<float>()(va.x) ^ std::hash<float>()(va.y)
                ^ std::hash<float>()(va.z) ^ std::hash<float>()(va.u)
                ^ std::hash<float>()(va.v) ^ std::hash<float>()(va.nx)
                ^ std::hash<float>()(va.ny) ^ std::hash<float>()(va.nz);
        }
    };

    std::vector<int> vertexIndices;
    std::vector<VertexAttributes> vertexBuffer;
    std::unordered_map<VertexAttributes, size_t, VertexAttributesHasher>
        seenVertices;
    for (auto& shape : shapes) {
        m_numVertices += shape.mesh.indices.size();
        for (auto& i : shape.mesh.indices) {
            bool hasPosition = i.vertex_index != -1;
            bool hasTexCoords = i.texcoord_index != -1;
            bool hasNormals = i.normal_index != -1;

            if (!hasPosition || !hasNormals) {
                // Bail if a Vertex does not have one of the necessary
                // attributes.
                return PrepareResult::ObjMissingAttrib;
            }

            VertexAttributes attribute {
                .x = attrib.vertices[3 * i.vertex_index + 0],
                .y = attrib.vertices[3 * i.vertex_index + 1],
                .z = attrib.vertices[3 * i.vertex_index + 2],
                .u = hasTexCoords ? attrib.texcoords[2 * i.texcoord_index + 0]
                                  : -1.0f,
                .v = hasTexCoords ? attrib.texcoords[2 * i.texcoord_index + 1]
                                  : -1.0f,
                .nx
                = hasNormals ? attrib.normals[3 * i.normal_index + 0] : -1.0f,
                .ny
                = hasNormals ? attrib.normals[3 * i.normal_index + 1] : -1.0f,
                .nz
                = hasNormals ? attrib.normals[3 * i.normal_index + 2] : -1.0f,
            };

            auto seenIt = seenVertices.find(attribute);
            if (seenIt != seenVertices.end()) {
                // An equal vertex attribute was found, this means we already
                // have an index for it.
                vertexIndices.push_back(narrow_into<int>(seenIt->second));
            } else {
                // A new index must be created for this vertex attribute.
                seenVertices[attribute] = vertexBuffer.size();
                vertexIndices.push_back(narrow_into<int>(vertexBuffer.size()));
                vertexBuffer.push_back(attribute);
            }
        }
    }

    // Create VAO, VBO and EBO.
    GLuint VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Upload mesh vertices into the VBO.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        narrow_into<GLsizeiptr>(sizeof(VertexAttributes) * vertexBuffer.size()),
        vertexBuffer.data(), GL_STATIC_DRAW);

    // Upload mesh indices into the EBO.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        narrow_into<GLsizei>(sizeof(int) * vertexIndices.size()),
        vertexIndices.data(), GL_STATIC_DRAW);

    // Declare Position attribute in the VAO.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributes),
        reinterpret_cast<void*>(offsetof(VertexAttributes, x)));

    // Declare UV attribute in the VAO.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttributes),
        reinterpret_cast<void*>(offsetof(VertexAttributes, u)));

    // Declare Normal attribute in the VAO.
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributes),
        reinterpret_cast<void*>(offsetof(VertexAttributes, nx)));

    m_VAO = VAO;

    // Create the texture for the mesh.
    GLuint meshTexture;
    glGenTextures(1, &meshTexture);
    glBindTexture(GL_TEXTURE_2D, meshTexture);

    // Set texture wrapping and filtering parameters.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load the texture for the mesh.
    int width, height, nChannels;
    unsigned char* textureData
        = stbi_load("meshes/capsule.jpg", &width, &height, &nChannels, 4);
    if (textureData) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, textureData);
    } else {
        return PrepareResult::TexLoadFail;
    }
    stbi_image_free(textureData);

    m_meshTexture = meshTexture;

    // Unbind texture before use.
    glBindTexture(GL_TEXTURE_2D, 0);

    // Unbind VAO, VBO and EBO before use.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glEnable(GL_DEPTH_TEST);

    return PrepareResult::PrepareOk;
}

void UmbrellaApplication::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Pass MVP into the Vertex Shader.
    GLint projectionIdx = glGetUniformLocation(m_shaderProgram, "uProjection");
    GLint viewIdx = glGetUniformLocation(m_shaderProgram, "uView");
    GLint modelIdx = glGetUniformLocation(m_shaderProgram, "uModel");

    // The Model has to follow the Scale-Rotate-Translate order.
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    model
        = glm::rotate(model, glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
    glm::mat4 view
        = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.5f, -7.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
        static_cast<float>(m_windowWidth) / static_cast<float>(m_windowHeight),
        0.1f, 100.0f);

    glUniformMatrix4fv(modelIdx, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewIdx, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionIdx, 1, GL_FALSE, glm::value_ptr(projection));

    glUseProgram(m_shaderProgram);
    glBindVertexArray(m_VAO);
    glBindTexture(GL_TEXTURE_2D, m_meshTexture);
    glDrawElements(GL_TRIANGLES, narrow_into<GLsizei>(m_numVertices),
        GL_UNSIGNED_INT, nullptr);
}

void UmbrellaApplication::Tick()
{
    Render();
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void UmbrellaApplication::Stop()
{
    spdlog::info("Stopping...");
    glfwTerminate();
}

void UmbrellaApplication::Run()
{
    spdlog::info("Started Umbrella.");

    if (Initialize() != InitializeResult::InitializeOk) {
        spdlog::error("Initialize() != InitializeResult::InitializeOk");
        return;
    }

    if (Prepare() != PrepareResult::PrepareOk) {
        spdlog::error("Prepare() != PrepareResult::PrepareOk");
        return;
    }

    while (!glfwWindowShouldClose(m_window)) {
        Tick();
    }

    Stop();
}

} // namespace Umbrella
