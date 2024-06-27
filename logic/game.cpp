// #include "../gfx/graphics.h"
#include "world.h"
#include "events.h"
#include "../multiplayer.h"
#include "collision.h"
#include "game.h"

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
extern glm::vec3 camRight;
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
bool playerRunning = false, flashlight = true;

bool clientonline = false, serveronline = false, noserverduplicates = false, noclientduplicates = false;
float clientvalidated = 0.0f, msgUpdate = 0.0f;
float delta_time = 0.0f;
std::thread serverThread, clientThread;
float doubleTapTimer = 0.0f;

void playerInput()
{
    if (ehandler.requestKeyState(GLFW_KEY_A))
    {
        playerVel.x = -1.5f;
        playerPos -= 4.2f * camRight * delta_time;
    }
    if (ehandler.requestKeyState(GLFW_KEY_D))
    {
        playerVel.x = 1.5f;
        playerPos += 4.2f * camRight * delta_time;
    }
    if (ehandler.requestKeyState(GLFW_KEY_S))
    {
        playerVel.z = 1.5f;
        playerPos -= 4.2f * glm::vec3(camFrontAlign.x, 0.0f, camFrontAlign.z) * delta_time;
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
    }
    else
        playerRunning = false;

    // if (playerPos.y > 0.0f && clientonline)
    //     mClient->UpdatePosition(playerPos.x, playerPos.y, playerPos.z);

    if (ehandler.requestKeyState(GLFW_KEY_1) == 2)
        serveronline = true;
    if (ehandler.requestKeyState(GLFW_KEY_2) == 2)
    {
        clientonline = true;
        clientvalidated = 20.0f;
    }
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

    if (ehandler.requestKeyState(GLFW_KEY_F) == 2)
        flashlight = !flashlight;

    if (ehandler.requestKeyState(GLFW_KEY_ESCAPE) == 2)
    {
        glfwSetWindowShouldClose(mainWindow, true);

        serveronline = false;
        hostServer->Stop();
        noserverduplicates = false;

        if (serverThread.joinable())
            serverThread.join();
    }

    if (clientvalidated > 0.0f && clientvalidated < 10.0f)
    {
        clientvalidated -= 10.0f * delta_time;
    }
    if (clientonline && msgUpdate > 0.0f)
    {
        msgUpdate -= 1.0f * delta_time;
    }
    if (clientonline && clientvalidated <= 0.0f && msgUpdate <= 0.0f) // if in air or moving or looking
    {
        mClient->UpdatePosition(playerPos, camFrontAlign); // updates get sent too fast and confuse the server I think
        msgUpdate = 0.01f;
    }
}

struct player
{
    glm::vec3 position;
    glm::vec3 rotation;
    uint32_t id;
};

int strangeCounter = 0; // for debug

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

    float time = 0.0f;
    float prev_time = time;

    glm::mat4 proj = glm::mat4(1.0f);

    proj = glm::perspective(glm::radians(45.0f), width / height, nearView, farView);

    // glm::vec3 lampPos(-5.0f, 0.5f, -5.0f);
    // std::vector<player> players;
#define PLAYER_COUNT 5
    player players[PLAYER_COUNT] = {
        {glm::vec3(0.0f, -100.0f, 0.0f), glm::vec3(0.0f), 0},
        {glm::vec3(0.0f, -100.0f, 0.0f), glm::vec3(0.0f), 0},
        {glm::vec3(0.0f, -100.0f, 0.0f), glm::vec3(0.0f), 0},
        {glm::vec3(0.0f, -100.0f, 0.0f), glm::vec3(0.0f), 0},
        {glm::vec3(0.0f, -100.0f, 0.0f), glm::vec3(0.0f), 0}};
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(4.3f, 0.0f, -28.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)};

    gfx::model playerModel("../gfx/models/player/player.obj");

    world level1;
    level1.PlaceObject("../gfx/models/terrain/simple/plane.obj", glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(800.0f, 0.0f, 800.0f), glm::vec3(1.0f), glm::vec2(160.0f, 160.0f));
    level1.PlaceObject("../gfx/models/items/carrot.obj", glm::vec3(0.8f, -1.8f, 0.0f), glm::vec3(1.0f), glm::vec3(0.0f, 160.0f, 0.0f));
    level1.PlaceObject("../gfx/models/items/carrot.obj", glm::vec3(-0.4f, -1.8f, 0.8f), glm::vec3(1.0f), glm::vec3(0.0f, 20.0f, 0.0f));
    level1.PlaceObject("../gfx/models/items/carrot.obj", glm::vec3(0.5f, -1.8f, 3.0f), glm::vec3(1.0f), glm::vec3(0.0f, -60.0f, 0.0f));

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
                        // bool playerExists = false;
                        static client cl;
                        mClient = &cl;

                        static std::string ipInput;
                        uint32_t playerMoved = 0;
                        std::cout << "please enter the ip you would like to connect to (in xxx.xxx.x.xxx format)\n";
                        std::cin >> ipInput;

                        cl.Connect(ipInput, "4444");
                        clientvalidated = 2.0f;

                        while (clientonline) // positions might be set as vec3(posx, posy, -posz)???
                        { // learnopengl -> engine planning
                                // udemy speedrun
                            message_header eh;
                            if (!cl.Incoming().empty())
                            {
                                auto msg = cl.Incoming().pop_front().msg;

                                switch (msg.header.id)
                                {
                                case 0:
                                    std::cout << "server sent connection greetings\n" << std::endl;
                                    clientvalidated = 0.0f;

                                    // uint32_t playerID;
                                    // msg >> playerID;

                                    // players.push_back({glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, playerMoved});

                                    //    cl.ConnectionGreeting();
                                    break;
                                case 1:
                                    msg >> eh; // just temporary, please fix the msgtmp_ header thing in multiplayer.cpp
                                    msg >> playerMoved;
                                    playerMoved -= 400; // new player additions should really happen on connection

                                    if (playerMoved >= PLAYER_COUNT)
                                        playerMoved = PLAYER_COUNT;
                                    msg >> players[playerMoved].rotation >> players[playerMoved].position;

                                    break;
                                // case 2:
                                //     msg >> eh; // just temporary, please fix the msgtmp_ header thing in multiplayer.cpp
                                //     msg >> playerMoved;
                                //     playerMoved -= 400; // new player additions should really happen on connection

                                //     if (playerMoved >= PLAYER_COUNT)
                                //         playerMoved = PLAYER_COUNT;
                                //     msg >> players[playerMoved].xzRotation;

                                //     std::cout << playerMoved << " moved head to " << players[playerMoved].xzRotation;

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

        // std::cout << camFront.x << ", " << camFront.y << ", " << camFront.z << " huh\n";

        // graphics begin here

        glm::vec3 disco = glm::vec3(0.5f, 0.5f, 0.5f);

        glUseProgram(lightShader.ID);
        // lightShader.setInt("pointLightCount", 4);
        // lightShader.setInt("spotLightCount", 1);
        if (flashlight)
        {
            lightShader.setVec3("sLights[0].specular", 1.0, 1.0, 1.0);
            lightShader.setVec3("sLights[0].position", playerPos + camRight * 0.3f);
            lightShader.setVec3("sLights[0].direction", camFront + camRight * -0.05f);
            lightShader.setDouble("sLights[0].constant", 1.0f);
            lightShader.setDouble("sLights[0].linear", 0.09f);
            lightShader.setDouble("sLights[0].quadratic", 0.032f);
            lightShader.setDouble("sLights[0].cutOff", glm::cos(glm::radians(12.5f)));
            lightShader.setDouble("sLights[0].outerCutOff", glm::cos(glm::radians(14.5f)));
            lightShader.setVec3("sLights[0].diffuse", disco);
            lightShader.setVec3("sLights[0].ambient", disco * 0.2f);
        }
        if (!flashlight)
        {
            lightShader.setVec3("sLights[0].diffuse", glm::vec3(0.0f));
            lightShader.setVec3("sLights[0].specular", glm::vec3(0.0f));
            lightShader.setVec3("sLights[0].ambient", glm::vec3(0.0f));
        }
        for (int i = 0; i < 4; ++i)
        {
            lightShader.setVec3("pLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
            lightShader.setVec3("pLights[" + std::to_string(i) + "].specular", glm::vec3(1.0f));
            lightShader.setVec3("pLights[" + std::to_string(i) + "].diffuse", glm::vec3(0.5f));
            lightShader.setVec3("pLights[" + std::to_string(i) + "].ambient", glm::vec3(0.1f));
            lightShader.setDouble("pLights[" + std::to_string(i) + "].constant", 1.0f);
            lightShader.setDouble("pLights[" + std::to_string(i) + "].linear", 0.09f);
            lightShader.setDouble("pLights[" + std::to_string(i) + "].quadratic", 0.032f);
        }
        lightShader.setDouble("material.shininess", 32.0);
        lightShader.setVec3("dLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        lightShader.setVec3("dLight.specular", glm::vec3(1.0));
        lightShader.setVec3("dLight.ambient", glm::vec3(0.1f));
        lightShader.setVec3("dLight.diffuse", glm::vec3(0.5f));

        glClearColor(0.03, 0.03, 0.05, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(playerPos, playerPos + camFront, camUp);

        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", proj);

        for (unsigned int i = 0; i < PLAYER_COUNT; ++i)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, players[i].position + glm::vec3(0.0f, -2.0f, 0.0f));
            if (players[i].rotation.z < 0.0f)
                model = glm::rotate(model, 3.0f - glm::asin(players[i].rotation.x), glm::vec3(0.0f, 1.0f, 0.0f));
            else
                model = glm::rotate(model, glm::asin(players[i].rotation.x), glm::vec3(0.0f, 1.0f, 0.0f));
            lightShader.setMat4("model", model);

            playerModel.draw(lightShader);
        }

        // lampShader.use();

        // lampShader.setMat4("projection", proj);
        // lampShader.setMat4("view", view);
        // lampShader.setVec3("inColor", disco);

        for (int i = 0; i < 4; ++i)
        {
            pointLightPositions[i] += glm::vec3(sin(glfwGetTime()) * (2.0f * i - 4.0f) * delta_time, 0.0f, cos(glfwGetTime()) * (2.0f * i - 4.0f) * delta_time);
            // glm::mat4 model = glm::mat4(1.0f);
            // model = glm::translate(model, pointLightPositions[i]);
            // model = glm::scale(model, glm::vec3(0.2f));
            // lampShader.setMat4("model", model);
            // glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        level1.Render(lightShader, proj, view);

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