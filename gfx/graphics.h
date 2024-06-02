// #include <iostream>
#include "shader.h"
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "../nms/stb_image.h"
#include "../events.h"
#include "../multiplayer.h"

namespace gfx
{
    event_handler ehandler;

    const float width = 1280.0f;
    const float height = 720.0f;
    const float near = 0.01f;
    const float far = 100.0f;

    float cube[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

    void framebuffer_size_callback(GLFWwindow *_win, int _w, int _h)
    {
        glViewport(0, 0, _w, _h);
    }
    void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        ehandler.key_callback(window, key, scancode, action, mods);
    }

    glm::vec3 playerPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 playerVel = glm::vec3(0.0f);
    glm::vec3 camRotation = glm::vec3(-90.0f, 0.0f, 0.0f);

    glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 prevCamFront = glm::vec3(0.0f, 0.0f, -1.0f);
    const glm::vec3 camUp = glm::vec3(0.0, 1.0, 0.0);
    bool firstMouseInteraction = true;

    float prevMx = 640, prevMy = 360, mouseSen = 0.1f;
    void mouse_callback(GLFWwindow *window, double xpos, double ypos)
    {
        if (firstMouseInteraction)
        {
            prevMx = xpos;
            prevMy = ypos;
            firstMouseInteraction = false;
        }

        float xoffset = xpos - prevMx;
        float yoffset = prevMy - ypos;
        prevMx = xpos;
        prevMy = ypos;

        xoffset *= mouseSen;
        yoffset *= mouseSen;

        camRotation.x += xoffset;
        camRotation.y += yoffset;

        if (camRotation.y > 89.9f)
            camRotation.y = 89.9f;
        if (camRotation.y < -89.9f)
            camRotation.y = -89.9f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(camRotation.x)) * cos(glm::radians(camRotation.y));
        direction.y = sin(glm::radians(camRotation.y));
        direction.z = sin(glm::radians(camRotation.x)) * cos(glm::radians(camRotation.y));

        camFront = glm::normalize(direction);
    }

    GLFWwindow *mainWindow;
    int windowInit()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        mainWindow = glfwCreateWindow(width, height, "basket engine alpha", NULL, NULL);
        if (mainWindow == NULL)
        {
            std::cerr << "GLFW failed to create a window." << std::endl;
            glfwTerminate();
            return 1;
        }
        glfwMakeContextCurrent(mainWindow);

        if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
        {
            std::cerr << "GLAD failed to load." << std::endl;
            return 1;
        }

        glViewport(0, 0, width, height);
        glfwSetFramebufferSizeCallback(mainWindow, framebuffer_size_callback);
        glfwSetKeyCallback(mainWindow, key_callback);

        glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(mainWindow, mouse_callback);

        return 0;
    }

    void processInput()
    {
        if (glfwGetKey(mainWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(mainWindow, true);
    }

    server *hostServer;
    client *mClient;

    void mainLoop()
    {
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
        float delta_time = 0.0f;

        glm::mat4 proj = glm::mat4(1.0f);

        proj = glm::perspective(glm::radians(45.0f), width / height, near, far);

        std::vector<glm::vec3> playerPositions;

        glEnable(GL_DEPTH_TEST);

        bool clientonline = false, serveronline = false, noserverduplicates = false, noclientduplicates = false;
        std::thread serverThread, clientThread;

        while (!glfwWindowShouldClose(gfx::mainWindow))
        {
            prev_time = time;
            time = glfwGetTime();
            delta_time = time - prev_time;

            // processInput();
            ehandler.poll();
            glfwPollEvents();

            playerVel.x = 0.0f;
            playerVel.z = 0.0f;

            // if (clientonline && camFront != prevCamFront)
            // {
            //     mClient->UpdateRotation(camFront);
            // }
            prevCamFront = camFront;

            if (ehandler.requestKeyState(GLFW_KEY_A))
            {
                // playerVel.x = -1.5f;
                playerPos -= 4.2f * glm::cross(camFront, camUp) * delta_time;

                if (clientonline)
                    mClient->UpdatePosition(playerPos.x, playerPos.y, playerPos.z);
            }
            if (ehandler.requestKeyState(GLFW_KEY_D))
            {
                // playerVel.x = 1.5f;
                playerPos += 4.2f * glm::cross(camFront, camUp) * delta_time;

                if (clientonline)
                    mClient->UpdatePosition(playerPos.x, playerPos.y, playerPos.z);
            }
            if (ehandler.requestKeyState(GLFW_KEY_S))
            {
                // playerVel.z = 1.5f;
                playerPos -= 4.2f * glm::vec3(camFront.x, 0.0f, camFront.z) * delta_time;

                if (clientonline)
                    mClient->UpdatePosition(playerPos.x, playerPos.y, playerPos.z);
            }
            if (ehandler.requestKeyState(GLFW_KEY_W))
            {
                // playerVel.z = -1.5f;
                playerPos += 4.2f * glm::vec3(camFront.x, 0.0f, camFront.z) * delta_time;

                if (clientonline)
                    mClient->UpdatePosition(playerPos.x, playerPos.y, playerPos.z);
            }
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

            if (serveronline && !noserverduplicates)
            {
                serverThread = std::thread([&serveronline]()
                                           {
                                            static server wizServer(4444);
                                            wizServer.Start();
                                            hostServer = &wizServer;
                                            while (serveronline)
                                            {
                                                wizServer.Update(-1, serveronline);
                                            } });

                noserverduplicates = true;
            }
            if (clientonline && !noclientduplicates)
            {
                clientThread = std::thread([&clientonline, &playerPositions]()
                                           {
                static client cl;
                static uint32_t playerMoved = 0;
                mClient = &cl;

                static std::string ipInput;
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

                            while (playerPositions.size() < playerMoved + 1)
                            {
                                playerPositions.push_back(glm::vec3(0.0f));
                            }

                            msg >> playerPositions[playerMoved].z >> playerPositions[playerMoved].y >> playerPositions[playerMoved].x;
                            
                            std::cout << playerMoved << " updated position as " << playerPositions[playerMoved].x << "\n";

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

            if (ehandler.requestKeyState(GLFW_KEY_ESCAPE) == 2)
            {
                glfwSetWindowShouldClose(mainWindow, true);

                serveronline = false;
                hostServer->Stop();
                noserverduplicates = false;

                if (serverThread.joinable())
                    serverThread.join();
            }

            playerPos += playerVel * delta_time;

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
            for (unsigned int i = 0; i < playerPositions.size(); ++i)
            {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, playerPositions[i]);
                // float angle = 20.0f * i;
                float angle = 1.0f;
                // model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0, 0.3, 0.5));
                // model = glm::rotate(model, glm::radians(cubeRotations[i]), glm::vec3(1.0f, 0.0f, 0.0f));

                int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            glfwSwapBuffers(gfx::mainWindow);
            glfwPollEvents();
        }

        if (serverThread.joinable())
            serverThread.join();
    }
};