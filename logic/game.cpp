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

extern float cube[180];

extern glm::vec3 camRotation;

extern glm::vec3 camFront;
extern glm::vec3 prevCamFront;
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
        playerPos -= 4.2f * glm::cross(camFront, camUp) * delta_time;

        if (clientonline)
            mClient->UpdatePosition(playerPos.x, playerPos.y, playerPos.z);
    }
    if (ehandler.requestKeyState(GLFW_KEY_D))
    {
        playerVel.x = 1.5f;
        playerPos += 4.2f * glm::cross(camFront, camUp) * delta_time;

        if (clientonline)
            mClient->UpdatePosition(playerPos.x, playerPos.y, playerPos.z);
    }
    if (ehandler.requestKeyState(GLFW_KEY_S))
    {
        playerVel.z = 1.5f;
        playerPos -= 4.2f * glm::vec3(camFront.x, 0.0f, camFront.z) * delta_time;

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

        playerPos += (4.2f + (8.4f * playerRunning)) * glm::vec3(camFront.x, 0.0f, camFront.z) * delta_time;

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
}

void mainLoop()
{
    if (gfx::windowInit() != 0)
    {
        std::cerr << "There was an error, and the program could not be run." << std::endl;
        return;
    }

    glfwSetKeyCallback(mainWindow, key_callback);

    Shader shaderProgram("../gfx/shaders/vertex.shader", "../gfx/shaders/fragment.shader");

    unsigned int VBO, VAO;
    // unsigned int EBO;
    glGenBuffers(1, &VBO);
    // glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    // glEnableVertexAttribArray(2);

    int texWidth, texHeight, nrChannels;
    unsigned int texture, happytexture;

    stbi_set_flip_vertically_on_load(true);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    unsigned char *data = stbi_load("../textures/container.jpg", &texWidth, &texHeight, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    glGenTextures(2, &happytexture);
    glBindTexture(GL_TEXTURE_2D, happytexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    data = stbi_load("../textures/awesomeface.png", &texWidth, &texHeight, &nrChannels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    glUseProgram(shaderProgram.ID);
    shaderProgram.setInt("texture1", 0);
    shaderProgram.setInt("texture2", 1);

    double mixA = 0.3;
    float time = 0.0f;
    float prev_time = time;

    glm::mat4 proj = glm::mat4(1.0f);

    proj = glm::perspective(glm::radians(45.0f), width / height, nearView, farView);

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)};

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
            playerVel.y -= 1.0f;
        if (playerPos.y <= 0.0f)
        {
            playerPos.y = 0.0f;
            playerVel.y = 0.0f;
        }
        if (ehandler.requestKeyState(GLFW_KEY_SPACE) == 2 && playerPos.y == 0.0f)
        {
            playerVel.y = 10.0f;
        }

        playerPos += playerVel * delta_time;

        if (ehandler.requestKeyState(GLFW_KEY_ESCAPE) == 2)
        {
            glfwSetWindowShouldClose(mainWindow, true);

            serveronline = false;
            hostServer->Stop();
            noserverduplicates = false;

            if (serverThread.joinable())
                serverThread.join();
        }

        if (serveronline && !noserverduplicates)
        {
            serverThread = std::thread([]()
                                       {
                                            static server wizServer(4444);
                                            wizServer.Start();
                                            hostServer = &wizServer;
                                            while (serveronline)
                                            {
                                                wizServer.Update(-1, serveronline);
                                            }
                                            wizServer.Stop(); });

            noserverduplicates = true;
        }
        if (clientonline && !noclientduplicates)
        {
            clientThread = std::thread([&cubePositions]()
                                       {
                                               static client cl;
                                               mClient = &cl;

                                                  static std::string ipInput;
                                                  uint32_t playerMoved = 0;
                                               std::cout << "please enter the ip you would like to connect to (in xxx.xxx.x.xxx format)\n";
                                                  std::cin >> ipInput;

                                               cl.Connect(ipInput, "4444");

                                               while (clientonline)
                                               {
                                                       message_header eh;
                                                   if (!cl.Incoming().empty())
                                                   {
                                                       auto msg = cl.Incoming().pop_front().msg;

                                                       switch (msg.header.id)
                                                       {
                                                       case 0:
                                                           std::cout << "server sent connection greetings\n";

                                                           // cl.ConnectionGreeting();
                                                           break;
                                                       case 1:
                                                           msg >> eh; // just temporary, please fix the msgtmp_ header thing in multiplayer.cpp
                                                           msg >> playerMoved;
                                                           playerMoved -= 400;

                                                           msg >> cubePositions[playerMoved].z >> cubePositions[playerMoved].y >> cubePositions[playerMoved].x;

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

        glClearColor(0.05, 0.05, 0.1, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(playerPos, playerPos + camFront, camUp);

        glUseProgram(shaderProgram.ID);
        shaderProgram.setDouble("mixamount", mixA);
        int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        int projLoc = glGetUniformLocation(shaderProgram.ID, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

        // glUniform4f(colorLocation, cValue, 0.5f, cValue, 1.0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, happytexture);
        glBindVertexArray(VAO);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        for (unsigned int i = 0; i < 10; ++i)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            // float angle = 20.0f * i;
            float angle = 1.0f;
            // model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0, 0.3, 0.5));
            // model = glm::rotate(model, glm::radians(cubeRotations[i]), glm::vec3(1.0f, 0.0f, 0.0f));

            int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(mainWindow);
        glfwPollEvents();
    }

    if (serverThread.joinable())
        serverThread.join();
}

void terminate()
{
    glfwTerminate();
}