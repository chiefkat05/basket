#include "../gfx/graphics.h"
#include "events.h"
#include "../multiplayer.h"
#include "collision.h"
#include "game.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../nms/stb_image.h"

extern const float width;
extern const float height;
extern const float nearView;
extern const float farView;

extern float cube[288];
extern float quad[24];

extern glm::vec3 camRotation;

extern glm::vec3 camFront;
extern glm::vec3 prevCamFront;
extern glm::vec3 camFrontAlign;
extern const glm::vec3 camUp;
extern bool firstMouseInteraction;

extern float prevMx, prevMy, mouseSen;

extern GLFWwindow *mainWindow;

event_handler ehandler;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    ehandler.key_callback(window, key, scancode, action, mods);
}

void gameInit()
{
    std::cout << "version alpha0.0\n\n";
}

unsigned int loadTexture(char const *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = GL_RED;
        // if (nrComponents == 1)
        //     format = GL_RED;
        if (nrComponents == 3)
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

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

server *hostServer;
client *mClient;
glm::vec3 playerPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 playerVel = glm::vec3(0.0f, 0.0f, 0.0f);
bool playerRunning = false;

bool clientonline = false, serveronline = false, noserverduplicates = false, noclientduplicates = false;
float delta_time = 0.0f;
std::thread serverThread, clientThread;
float doubleTapTimer = 0.0f;

void playerInput()
{
    if (ehandler.requestKeyState(GLFW_KEY_A))
    {
        playerVel.x = -1.5f;
        playerPos -= 4.2f * glm::cross(camFrontAlign, camUp) * delta_time;

        if (clientonline)
            mClient->UpdatePosition(playerPos.x, playerPos.y, playerPos.z);
    }
    if (ehandler.requestKeyState(GLFW_KEY_D))
    {
        playerVel.x = 1.5f;
        playerPos += 4.2f * glm::cross(camFrontAlign, camUp) * delta_time;

        if (clientonline)
            mClient->UpdatePosition(playerPos.x, playerPos.y, playerPos.z);
    }
    if (ehandler.requestKeyState(GLFW_KEY_S))
    {
        playerVel.z = 1.5f;
        playerPos -= 4.2f * glm::vec3(camFrontAlign.x, 0.0f, camFrontAlign.z) * delta_time;

        if (clientonline)
            mClient->UpdatePosition(playerPos.x, playerPos.y, playerPos.z);
    }
    if (ehandler.requestKeyState(GLFW_KEY_W) == 2 || ehandler.requestKeyState(GLFW_KEY_LEFT_CONTROL))
    {
        if (doubleTapTimer > 0.0f)
        {
            playerRunning = true;
        }
        doubleTapTimer = 1.0f;
    }
    if (ehandler.requestKeyState(GLFW_KEY_W))
    {
        playerVel.z = -1.5f;

        playerPos += (4.2f + (8.4f * playerRunning)) * glm::vec3(camFrontAlign.x, 0.0f, camFrontAlign.z) * delta_time;

        if (clientonline)
            mClient->UpdatePosition(playerPos.x, playerPos.y, playerPos.z);
    }
    else
        playerRunning = false;

    if (playerPos.y > 0.0f && clientonline)
        mClient->UpdatePosition(playerPos.x, playerPos.y, playerPos.z);

    if (ehandler.requestKeyState(GLFW_KEY_1) == 2)
        serveronline = true;
    if (ehandler.requestKeyState(GLFW_KEY_2) == 2)
        clientonline = true;
    if (ehandler.requestKeyState(GLFW_KEY_3) == 2 && serveronline)
    {
        serveronline = false;
        hostServer->Stop();
        noserverduplicates = false;

        if (serverThread.joinable())
            serverThread.join();
    }
    if (ehandler.requestKeyState(GLFW_KEY_3) == 2 && clientonline)
    {
        clientonline = false;

        if (clientThread.joinable())
            clientThread.join();
    }
    if (ehandler.requestKeyState(GLFW_KEY_Z))
        glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if (ehandler.requestKeyState(GLFW_KEY_X))
        glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (doubleTapTimer < 0.0f)
    {
        doubleTapTimer = 0.0f;
    }
    if (doubleTapTimer > 0.0f)
    {
        doubleTapTimer -= 4.0f * delta_time;
    }

    if (ehandler.requestKeyState(GLFW_KEY_SPACE) == 2 && playerPos.y == 0.0f)
    {
        playerVel.y = 10.0f;
    }

    if (ehandler.requestKeyState(GLFW_KEY_ESCAPE) == 2)
    {
        glfwSetWindowShouldClose(mainWindow, true);

        serveronline = false;
        hostServer->Stop();
        noserverduplicates = false;

        if (serverThread.joinable())
            serverThread.join();
    }
}

struct player
{
    glm::vec3 position;
    float xzRotation;
    uint32_t id;
};

void mainLoop()
{
    if (gfx::windowInit() != 0)
    {
        std::cerr << "There was an error, and the program could not be run." << std::endl;
        return;
    }

    glfwSetKeyCallback(mainWindow, key_callback);

    Shader objectShader("../gfx/shaders/vertex.shader", "../gfx/shaders/fragment.shader");
    Shader lightShader("../gfx/shaders/vertex-light.shader", "../gfx/shaders/fragment-light.shader");
    Shader lampShader("../gfx/shaders/vertex-blank.shader", "../gfx/shaders/fragment-blank.shader");
    Shader playerShader("../gfx/shaders/vertex.shader", "../gfx/shaders/fragment.shader");

    unsigned int VBO, VAO, lightVAO;
    glGenBuffers(1, &VBO);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // glGenBuffers(1, &quadVBO); // for quad???
    // glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    stbi_set_flip_vertically_on_load(true);

    unsigned int texture = loadTexture("../textures/container2.png");
    unsigned int specularMap = loadTexture("../textures/container2_specular.png");
    unsigned int emissionMap = loadTexture("../textures/matrix.jpg");
    unsigned int playerTex = loadTexture("../textures/awesomeface.png");

    lightShader.use();
    lightShader.setInt("material.diffuse", 0);
    lightShader.setInt("material.specular", 1);
    lightShader.setInt("material.emission", 2);

    float time = 0.0f;
    float prev_time = time;

    glm::mat4 proj = glm::mat4(1.0f);

    proj = glm::perspective(glm::radians(45.0f), width / height, nearView, farView);

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, -0.5f, 0.0f),
        glm::vec3(2.0f, -0.5f, -15.0f),
        glm::vec3(-1.5f, -0.5f, -2.5f),
        glm::vec3(-3.8f, -0.5f, -12.3f),
        glm::vec3(2.4f, -0.5f, -3.5f),
        glm::vec3(-1.7f, -0.5f, -7.5f),
        glm::vec3(1.3f, -0.5f, -5.5f),
        glm::vec3(1.5f, -0.5f, -2.5f),
        glm::vec3(1.5f, -0.5f, -1.5f),
        glm::vec3(-1.3f, -0.5f, -1.5f)};
    glm::vec3 lampPos(-5.0f, 0.5f, -5.0f);
    std::vector<player> players;

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(mainWindow))
    {
        prev_time = time;
        time = glfwGetTime();
        delta_time = time - prev_time;

        playerInput();
        ehandler.poll();
        glfwPollEvents();

        playerVel.x = 0.0f;
        playerVel.z = 0.0f;

        prevCamFront = camFront;

        if (playerPos.y > 0.0f)
            playerVel.y -= 40.0f * delta_time;
        if (playerPos.y < 0.0f)
        {
            playerPos.y = 0.0f;
            playerVel.y = 0.0f;
        }
        playerPos += playerVel * delta_time;

        if (serveronline && !noserverduplicates)
        {
            serverThread = std::thread([]()
                                       {
                                            static server wizServer(4444);
                                            wizServer.Start();
                                            hostServer = &wizServer;
                                            while (serveronline)
                                            {
                                                wizServer.Update(-1, serveronline, delta_time);
                                            }
                                            wizServer.Stop(); });

            noserverduplicates = true;
        }
        if (clientonline && !noclientduplicates)
        {
            clientThread = std::thread([&players]()
                                       {
                                           static client cl;
                                           mClient = &cl;

                                           static std::string ipInput;
                                           uint32_t playerMoved = 0;
                                           std::cout << "please enter the ip you would like to connect to (in xxx.xxx.x.xxx format)\n";
                                           std::cin >> ipInput;

                                           cl.Connect(ipInput, "4444");

                                           while (clientonline) // positions might be set as vec3(posx, posy, -posz)???
                                           { // learnopengl -> engine planning
                                                // udemy speedrun
                                               message_header eh;
                                               if (!cl.Incoming().empty())
                                               {
                                                   auto msg = cl.Incoming().pop_front().msg;

                                                    bool playerExists = false;
                                                   switch (msg.header.id)
                                                   {
                                                   case 0:
                                                       std::cout << "server sent connection greetings\n";

                                                       //    cl.ConnectionGreeting();
                                                       break;
                                                   case 1:
                                                       msg >> eh; // just temporary, please fix the msgtmp_ header thing in multiplayer.cpp
                                                       msg >> playerMoved;
                                                       playerMoved -= 400;

                                                       for (unsigned int i = 0; i < players.size(); ++i)
                                                       {
                                                        if (players[i].id == playerMoved)
                                                        {
                                                            playerMoved = i;
                                                            playerExists = true;
                                                            break;
                                                        }
                                                       }
                                                        if (!playerExists)
                                                        {
                                                            playerMoved = players.size();
                                                            players.push_back({glm::vec3(0.0f), 0.0f, playerMoved});
                                                        }
                                                       msg >> players[playerMoved].position.z >> players[playerMoved].position.y >> players[playerMoved].position.x;

                                                       break;
                                                   // case 2:
                                                   //     msg >> eh; // just temporary, please fix the msgtmp_ header thing in multiplayer.cpp
                                                   //     msg >> playerMoved;
                                                   //     playerMoved -= 400;
                                                   //     msg >> cubeRotations[playerMoved];
                                                   //     break;
                                                   default:
                                                       std::cout << "received unknown message type: " << msg.header.id << "\n";
                                                       break;
                                                   }
                                               }
                                           }

                                           cl.Disconnect(); });

            noclientduplicates = true;
        }

        // graphics begin here

        glm::vec3 disco = glm::vec3(0.5f, 0.5f, 0.5f);

        glUseProgram(lightShader.ID);
        lightShader.setVec3("light.specular", 1.0, 1.0, 1.0);
        lightShader.setVec3("light.position", lampPos);
        lightShader.setVec3("light.diffuse", disco);
        lightShader.setVec3("light.ambient", disco * 0.2f);
        lightShader.setDouble("material.shininess", 128.0);

        glClearColor(0.03, 0.03, 0.05, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(playerPos, playerPos + camFront, camUp);

        // glUseProgram(objectShader.ID);
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", proj);

        lampPos += glm::vec3(sin(glfwGetTime()) * 20.0f * delta_time, 0.0f, cos(glfwGetTime()) * 20.0f * delta_time);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, emissionMap);
        glBindVertexArray(VAO);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        for (unsigned int i = 0; i < 10; ++i)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i] * 2.0f + glm::vec3(0.0f, 0.5f, 0.0f));
            // model = glm::translate(model, players[i]);
            // model = glm::scale(model, glm::vec3(1.0f, cubePositions[i].y * 4.0f, 1.0f));
            glm::mat3 normalMat = glm::transpose(glm::inverse(model));

            lightShader.setMat4("model", model);
            lightShader.setMat3("normalMat", normalMat);
            lightShader.setBool("speeding", playerRunning);
            lightShader.setVec3("viewPos", playerPos);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        playerShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, playerTex);
        glBindVertexArray(VAO);
        playerShader.setVec3("color", glm::vec3(0.5f, 0.5f, 0.5f));
        playerShader.setMat4("projection", proj);
        playerShader.setMat4("view", view);
        for (unsigned int i = 0; i < players.size(); ++i)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, players[i].position);
            playerShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        lampShader.use();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lampPos);
        model = glm::scale(model, glm::vec3(0.2f));

        lampShader.setMat4("projection", proj);
        lampShader.setMat4("view", view);
        lampShader.setMat4("model", model);
        lampShader.setVec3("inColor", disco);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(mainWindow);
        glfwPollEvents();
    }

    if (serveronline && serverThread.joinable())
        serverThread.join();
}

void terminate()
{
    glfwTerminate();
}