#include <iostream>
#include <sstream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "camera/camera.hpp"
#include "renderer/shader.hpp"
#include "world/terrain.hpp"

#define POLYGON_MODE false

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
const char* glfwErrorName(int error);
void glfwErrorCallback(int error, const char* description);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
unsigned int loadTexture(char const * path);

constexpr int WIDTH = 800;
constexpr int HEIGHT = 450;

float lastX = 400;
float lastY = 225;

bool firstMouse = true;
bool flashLightEnable = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

constexpr float vertices[] = {
//   positions             normal vectors        texture coordinates
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f, 1.0f,    0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f
};

glm::vec3 pointLightPositions[] = {
    glm::vec3( 0.7f,  5.2f,  2.0f),
    glm::vec3( 2.3f,  5.3f, -4.0f),
    glm::vec3(-4.0f,  5.0f, -12.0f),
    glm::vec3( 10.0f,  5.0f, -3.0f)
};

int main() {
    glfwSetErrorCallback(glfwErrorCallback);

    if (!glfwInit())
        return EXIT_FAILURE;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Venturers", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGL(glfwGetProcAddress)) {
        glfwTerminate();
        std::cerr << "GLAD Error: Failed to initialize GLAD" << std::endl;
        return EXIT_FAILURE;
    }

    glEnable(GL_DEPTH_TEST);

#if POLYGON_MODE
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);

    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    const Shader shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
    const Shader lightingShader("shaders/lighting.vert", "shaders/lighting.frag");

    const std::vector<glm::vec3> blockPositions = generateTerrain(200);

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int lightVAO;

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    const unsigned int air = loadTexture("assets/textures/air.png");
    const unsigned int dirt = loadTexture("assets/textures/dirt.png");
    const unsigned int diffuseMap = loadTexture("assets/textures/container2.png");
    const unsigned int specularMap = loadTexture("assets/textures/container2_specular.png");
    const unsigned int emissionMap = loadTexture("assets/textures/matrix.jpg");

    shader.use();
    shader.setInt("material.diffuse", 0);
    shader.setInt("material.specular", 1);
    shader.setInt("material.emission", 2);

    while (!glfwWindowShouldClose(window)) {
        const float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::vec3 center(0.0f, 1.0f, 0.0f);
        glm::vec3 offset(0.0f, 0.0f, 10.0f);
        const float t = static_cast<float>(glfwGetTime());
        glm::mat4 m(1.0f);
        m = glm::translate(m, center);
        m = glm::rotate(m, t, glm::vec3(0,1,0));
        m = glm::translate(m, offset);
        glm::vec3 lightPos = glm::vec3(m * glm::vec4(0,0,0,1));

        shader.setVec3("viewPos", camera.Position);
        shader.setFloat("material.shininess", 32.0f);

        // directional light
        shader.setVec3("dirLight.direction", 0.2f, 11.0f, 0.3f);
        shader.setVec3("dirLight.ambient", 0.15f, 0.15f, 0.15f);
        shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        shader.setVec3("dirLight.lightColor", 1.0f, 1.0f, 1.0f);
        // point light 1
        shader.setVec3("pointLights[0].position", lightPos + pointLightPositions[0]);
        shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLights[0].constant", 1.0f);
        shader.setFloat("pointLights[0].linear", 0.09f);
        shader.setFloat("pointLights[0].quadratic", 0.032f);
        shader.setVec3("pointLights[0].lightColor", 1.0f, 1.0f, 1.0f);
        // point light 2
        shader.setVec3("pointLights[1].position", lightPos + pointLightPositions[1]);
        shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLights[1].constant", 1.0f);
        shader.setFloat("pointLights[1].linear", 0.09f);
        shader.setFloat("pointLights[1].quadratic", 0.032f);
        shader.setVec3("pointLights[1].lightColor", 1.0f, 1.0f, 1.0f);
        // point light 3
        shader.setVec3("pointLights[2].position", lightPos + pointLightPositions[2]);
        shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLights[2].constant", 1.0f);
        shader.setFloat("pointLights[2].linear", 0.09f);
        shader.setFloat("pointLights[2].quadratic", 0.032f);
        shader.setVec3("pointLights[2].lightColor", 1.0f, 1.0f, 1.0f);
        // point light 4
        shader.setVec3("pointLights[3].position", lightPos + pointLightPositions[3]);
        shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("pointLights[3].constant", 1.0f);
        shader.setFloat("pointLights[3].linear", 0.09f);
        shader.setFloat("pointLights[3].quadratic", 0.032f);
        shader.setVec3("pointLights[3].lightColor", 1.0f, 1.0f, 1.0f);
        // spotLight
        shader.setBool("spotLightEnable", flashLightEnable);
        shader.setVec3("spotLight.position", camera.Position);
        shader.setVec3("spotLight.direction", camera.Front);
        shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("spotLight.constant", 1.0f);
        shader.setFloat("spotLight.linear", 0.09f);
        shader.setFloat("spotLight.quadratic", 0.032f);
        shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 0.1f, 1000.0f);
        shader.setMat4("projection", projection);

        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, air);

        glBindVertexArray(VAO);
        for (const auto& pos : blockPositions) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            shader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        lightingShader.use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 5.0f, 0.0f));
        shader.setMat4("model", model);
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightVAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, lightPos + pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            lightingShader.setMat4("model", model);
            lightingShader.setVec3("lightColor", glm::vec3(i / 4 + 0.5f, i / 4 + 0.5f, i / 4 + 0.5f));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();

    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);

    static bool rWasDown = false;
    const bool rDown = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
    if (rDown && !rWasDown) {
        flashLightEnable = !flashLightEnable;
    }
    rWasDown = rDown;

}

void framebufferSizeCallback(GLFWwindow* window, const int width, const int height) {
    glViewport(0, 0, width, height);
}

const char* glfwErrorName(const int error) {
    switch (error) {
        case GLFW_NO_ERROR:              return "GLFW_NO_ERROR";
        case GLFW_NOT_INITIALIZED:       return "GLFW_NOT_INITIALIZED";
        case GLFW_NO_CURRENT_CONTEXT:    return "GLFW_NO_CURRENT_CONTEXT";
        case GLFW_INVALID_ENUM:          return "GLFW_INVALID_ENUM";
        case GLFW_INVALID_VALUE:         return "GLFW_INVALID_VALUE";
        case GLFW_OUT_OF_MEMORY:         return "GLFW_OUT_OF_MEMORY";
        case GLFW_API_UNAVAILABLE:       return "GLFW_API_UNAVAILABLE";
        case GLFW_VERSION_UNAVAILABLE:   return "GLFW_VERSION_UNAVAILABLE";
        case GLFW_PLATFORM_ERROR:        return "GLFW_PLATFORM_ERROR";
        case GLFW_FORMAT_UNAVAILABLE:    return "GLFW_FORMAT_UNAVAILABLE";
        case GLFW_NO_WINDOW_CONTEXT:     return "GLFW_NO_WINDOW_CONTEXT";
        case GLFW_CURSOR_UNAVAILABLE:    return "GLFW_CURSOR_UNAVAILABLE";
        case GLFW_FEATURE_UNAVAILABLE:   return "GLFW_FEATURE_UNAVAILABLE";
        case GLFW_FEATURE_UNIMPLEMENTED: return "GLFW_FEATURE_UNIMPLEMENTED";
        case GLFW_PLATFORM_UNAVAILABLE:  return "GLFW_PLATFORM_UNAVAILABLE";
        default:                         return "UNKNOWN_GLFW_ERROR";
    }
}

void glfwErrorCallback(const int error, const char* description) {
    std::cerr << "[GLFW] "
              << glfwErrorName(error)
              << " (" << error << "): "
              << description << std::endl;
}

void mouseCallback(GLFWwindow* window, const double xpos, const double ypos)
{
    const float mouseX = static_cast<float>(xpos);
    const float mouseY = static_cast<float>(ypos);

    if (firstMouse) {
        lastX = mouseX;
        lastY = mouseY;
        firstMouse = false;
    }

    const float xoffset = mouseX - lastX;
    const float yoffset = lastY - mouseY;

    lastX = mouseX;
    lastY = mouseY;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

unsigned int loadTexture(char const * path) {
    unsigned int textureID;

    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = GL_ZERO;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        std::cout << "Failed to load texture: " << path << std::endl;
    }

    stbi_image_free(data);
    return textureID;
}
