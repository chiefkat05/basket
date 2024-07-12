// #include "../gfx/graphics.h"
#include "world.h"
#include "events.h"
#include "../multiplayer.h"
#include "collision.h"
#include "game.h"

#include "../nms/stb_image.h"

#include <math.h>

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
extern glm::vec3 camTrueRight;
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
    std::cout << "version alpha0.1\n\n";
}

server *hostServer;
client *mClient;
bool playerRunning = false, flashlight = true;

bool clientonline = false, serveronline = false, noserverduplicates = false, noclientduplicates = false;
float clientvalidated = 0.0f, msgUpdate = 0.0f;
float delta_time = 0.0f;
std::thread serverThread, clientThread;
float doubleTapTimer = 0.0f;
object *objectLookingAt = nullptr, *objectHolding = nullptr;
unsigned int objectHoldingID = 0;
glm::vec3 playerVel = glm::vec3(0.0f);

struct player
{
    // glm::vec3 position;
    // glm::vec3 rotation;
    // object *obj;
    uint32_t id = 0;
    unsigned int objID;
};
#define PLAYER_COUNT 5
player players[PLAYER_COUNT];
unsigned int playerObjIndex = 5;

world level1;

void playerInput()
{
    if (ehandler.requestKeyState(GLFW_KEY_A))
    {
        playerVel.x = -1.5f;
        level1.objects[players[0].objID].position -= 4.2f * camTrueRight * delta_time;
    }
    if (ehandler.requestKeyState(GLFW_KEY_D))
    {
        playerVel.x = 1.5f;
        level1.objects[players[0].objID].position += 4.2f * camTrueRight * delta_time;
    }
    if (ehandler.requestKeyState(GLFW_KEY_S))
    {
        playerVel.z = 1.5f;
        level1.objects[players[0].objID].position -= 4.2f * glm::vec3(camFrontAlign.x, 0.0f, camFrontAlign.z) * delta_time;
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

        level1.objects[players[0].objID].position += (4.2f + (8.4f * playerRunning)) * glm::vec3(camFrontAlign.x, 0.0f, camFrontAlign.z) * delta_time;
    }
    else
        playerRunning = false;

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

    if (ehandler.requestKeyState(GLFW_KEY_SPACE) == 2 && level1.objects[players[0].objID].position.y == 0.0f)
    {
        playerVel.y = 10.0f;
    }

    if (ehandler.requestKeyState(GLFW_KEY_F) == 2)
        flashlight = !flashlight;

    if (objectHolding != nullptr)
    {
        objectHolding->position = level1.objects[players[0].objID].position + camFront * 2.2f - camUp * 0.6f;
        // if (camFrontAlign.z < 0.0f)
        //     objectHolding->rotation.y = glm::degrees(AI_MATH_PI - glm::asin(camFrontAlign.x)) - 90.0f;
        // else
        //     objectHolding->rotation.y = glm::degrees(glm::asin(camFrontAlign.x)) - 90.0f;
        objectHolding->rotation.y = glm::degrees(atan2f(camFrontAlign.x, camFrontAlign.z)) - 90.0f;

        objectHolding->rotation.z = -glm::degrees(glm::acos(camFront.y));
        // objectHolding->rotation.z = glm::degrees(atan2f(camFront.y, camFront.z));

        flashlight = false;
        objectLookingAt = objectHolding;

        if (clientonline && !serveronline && clientvalidated <= 0.0f && msgUpdate <= 0.0f)
        {
            mClient->UpdateObject(objectHoldingID, objectHolding->position, objectHolding->rotation);
        }
    }
    if (objectLookingAt != nullptr && ehandler.requestKeyState(GLFW_KEY_E) == 2)
    {
        if (objectHolding == nullptr)
        {
            objectHolding = objectLookingAt;
            objectHolding->beingHeld = 0;
        }
        else // think about how to make taking objects from other clients work
        {
            objectHolding->beingHeld = -1;
            objectHolding = nullptr;
            if (clientonline && clientvalidated <= 0.0f)
                mClient->CancelObjectUpdates(objectHoldingID);
        }
    }

    if (ehandler.requestKeyState(GLFW_KEY_I) == 2)
    {
        level1.PlaceObject("../gfx/models/walls/W-o1/w-01.obj",
                           glm::vec3(level1.objects[players[0].objID].position.x, -2.0f,
                                     level1.objects[players[0].objID].position.z) +
                               camFrontAlign * 8.0f,
                           glm::vec3(1.0f), glm::vec3(0.0f, glm::degrees(asin(camFrontAlign.x)), 0.0f), glm::vec2(1.0f), false, true, true);
    }
    if (ehandler.requestKeyState(GLFW_KEY_O) == 2)
    {
        level1.PlaceObject("../gfx/models/items/carrot.obj",
                           glm::vec3(level1.objects[players[0].objID].position.x, -2.0f,
                                     level1.objects[players[0].objID].position.z) +
                               camFrontAlign * 8.0f,
                           glm::vec3(1.0f),
                           glm::vec3(0.0f, glm::degrees(asin(camFrontAlign.x)), 0.0f), glm::vec2(1.0f), true, true, false, 32.0f);
    }
    if (ehandler.requestKeyState(GLFW_KEY_P) == 2)
    {
        level1.PlaceObject("../gfx/models/walls/cube/cube.obj",
                           glm::vec3(level1.objects[players[0].objID].position.x,
                                     0.0f, level1.objects[players[0].objID].position.z) +
                               camFrontAlign * 8.0f,
                           glm::vec3(1.0f),
                           glm::vec3(0.0f, glm::degrees(asin(camFrontAlign.x)), 0.0f), glm::vec2(1.0f), false, true, true);
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

    if (clientvalidated > 0.0f && clientvalidated < 10.0f)
    {
        clientvalidated -= 10.0f * delta_time;
    }
    if (clientonline && clientvalidated <= 0.0f && msgUpdate <= 0.0f) // multiplayer player update loop (what data gets sent)
    {
        mClient->UpdatePlayer(level1.objects[players[0].objID].position, camFrontAlign);
        msgUpdate = 0.01f;
    }
    if (clientonline && msgUpdate > 0.0f)
    {
        msgUpdate -= 1.0f * delta_time;
    }
}

int strangeCounter = 0; // for debug

void mainLoop()
{
    srand(time(0));

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

    // std::vector<player> players;
    // player players[PLAYER_COUNT] = {
    //     {glm::vec3(0.0f, -100.0f, 0.0f), glm::vec3(0.0f), 0},
    //     {glm::vec3(0.0f, -100.0f, 0.0f), glm::vec3(0.0f), 0},
    //     {glm::vec3(0.0f, -100.0f, 0.0f), glm::vec3(0.0f), 0},
    //     {glm::vec3(0.0f, -100.0f, 0.0f), glm::vec3(0.0f), 0},
    //     {glm::vec3(0.0f, -100.0f, 0.0f), glm::vec3(0.0f), 0}};

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(4.3f, 0.0f, -28.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)};

    gfx::model playerModel("../gfx/models/player/player.obj");

    level1.PlaceObject("../gfx/models/terrain/simple/plane.obj", glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(800.0f, 0.0f, 800.0f), glm::vec3(1.0f), glm::vec2(160.0f, 160.0f), true, true, true);
    level1.PlaceObject("../gfx/models/terrain/simple/plane.obj", glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(800.0f, 0.0f, 800.0f), glm::vec3(1.0f, 1.0f, 180.0f), glm::vec2(160.0f, 160.0f), false, true, true);
    // too bloated
    level1.PlaceObject("../gfx/models/items/carrot.obj", glm::vec3(0.8f, -1.8f, 0.0f), glm::vec3(1.0f),
                       glm::vec3(0.0f, 160.0f, 0.0f), glm::vec2(1.0f), true, true, false, 32.0f);
    level1.PlaceObject("../gfx/models/items/carrot.obj", glm::vec3(-0.4f, -1.8f, 0.8f), glm::vec3(1.0f),
                       glm::vec3(0.0f, 20.0f, 0.0f), glm::vec2(1.0f), true, true, false, 32.0f);
    level1.PlaceObject("../gfx/models/items/carrot.obj", glm::vec3(0.5f, -1.8f, 3.0f), glm::vec3(1.0f),
                       glm::vec3(0.0f, -60.0f, 0.0f), glm::vec2(1.0f), true, true, false, 32.0f);

    level1.PlaceObject("../gfx/models/enemies/smoosh/smooshman.obj", glm::vec3(4.0f, -2.0f, 0.0f), glm::vec3(1.0f), glm::vec3(0.0f), glm::vec2(1.0f), true, true, true);
    unsigned int deathCubeID = level1.objects.size() - 1;

    for (int i = 0; i < 128; ++i)
    {
        float randX = static_cast<float>(rand() % 2400 - 1200) * 0.1f;
        float randZ = static_cast<float>(rand() % 2400 - 1200) * 0.1f;
        level1.PlaceObject("../gfx/models/terrain/bush-1/bush.obj", glm::vec3(randX, -2.0f, randZ));
        level1.objects.back().includesTransparency = true;
    }

    // glm::vec3 randPosition = glm::vec3(float(rand() % 6 + 1) * 2.2f, -2.0f, float(rand() % 13 + 3) * 2.2f);
    glm::vec3 randPosition = glm::vec3(4.0f, -2.0f, -8.0f);
    for (int i = 0; i < 24; ++i)
    {
        float yRot = (rand() % 8) * 45.0f;
        glm::vec3 pos = randPosition + (static_cast<float>(i + 1) * 4.0f) + (sin(yRot) + cos(yRot));
        // glm::vec3 pos = randPosition * static_cast<float>(i + 1) * 4.0f;
        // glm::vec3 pos = glm::vec3(0.0f);

        level1.PlaceObject("../gfx/models/walls/W-o1/w-01.obj", glm::vec3(pos.x * (i % 4 - 1), -2.0f, pos.z), glm::vec3(1.0f),
                           glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), true, true, true);
        // level1.PlaceObject("../gfx/models/walls/cube/cube.obj", glm::vec3(pos.x * (i % 4 - 1), -1.0f, pos.z), glm::vec3(1.0f),
        //                    glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f), false, true);
    }

    // level1.objects[players[0].objID].obtainable = true;
    for (int i = 0; i < PLAYER_COUNT; ++i)
    {
        level1.PlaceObject("../gfx/models/player/player.obj",
                           glm::vec3(-3.0f, -100.0f, -5.0f), glm::vec3(1.0f),
                           glm::vec3(1.0f), glm::vec2(1.0f),
                           false, true, false, 18.0f);

        players[i].objID = level1.objects.size() - 1;
        level1.objects[players[i].objID].invisible = true;
        level1.objects[players[i].objID].collisionMeshID = 0;
        level1.objects[players[i].objID].obtainable = true;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::shared_ptr<connection> ptrClient;

    while (!glfwWindowShouldClose(mainWindow))
    {
        prev_time = time;
        time = glfwGetTime();
        delta_time = time - prev_time;

        glClearColor(0.03, 0.03, 0.05, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        playerInput();
        ehandler.poll();
        glfwPollEvents();

        playerVel.x = 0.0f;
        playerVel.z = 0.0f;

        prevCamFront = camFront;

        glm::vec3 distToPlayer = glm::vec3(level1.objects[deathCubeID].position.x - level1.objects[players[0].objID].position.x,
                                           0.0f, level1.objects[deathCubeID].position.z - level1.objects[players[0].objID].position.z);
        glm::vec3 normDistToPlayer = glm::normalize(distToPlayer);
        if (std::abs(distToPlayer.x * distToPlayer.z) > 1.0f && std::abs(distToPlayer.x * distToPlayer.z) < 45.0f)
            level1.objects[deathCubeID].position -= delta_time * 3.0f * normDistToPlayer;

        // level1.objects[deathCubeID].rotation.y = atan2()

        if (level1.objects[players[0].objID].position.y > 0.0f)
            playerVel.y -= 40.0f * delta_time;
        if (level1.objects[players[0].objID].position.y < 0.0f)
        {
            level1.objects[players[0].objID].position.y = 0.0f;
            playerVel.y = 0.0f;
        }
        level1.objects[players[0].objID].position += playerVel * delta_time;

        if (serveronline && !noserverduplicates)
        {
            serverThread = std::thread([]()
                                       {
                                            static server wizServer(4444);
                                            wizServer.Start();
                                            hostServer = &wizServer;

                                            float serverUpdateMsg = 0.0f;
                                            while (serveronline)
                                            {
                                                if (serverUpdateMsg < 0.0f)
                                                {
                                                    serverUpdateMsg = 0.04f;
                                                }
                                                serverUpdateMsg -= 1.0f * delta_time;

                                                wizServer.Update(-1, serveronline, delta_time, level1, serverUpdateMsg);
                                            }
                                            wizServer.Stop(); });

            noserverduplicates = true;
        }
        if (clientonline && !noclientduplicates)
        {
            clientThread = std::thread([]()
                                       {
                        static client cl;
                        mClient = &cl;

                        static std::string ipInput;
                        uint32_t playerMoved = 0;
                        unsigned int objectMoved = 0;
                        std::cout << "please enter the ip you would like to connect to (in xxx.xxx.x.xxx format)\n";
                        std::cin >> ipInput;

                        cl.Connect(ipInput, "4444");
                        clientvalidated = 2.0f;

                        while (clientonline)
                        {
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
                                    msg >> playerMoved; // who cares
                                    playerMoved -= 399; // + 1 since self client is first player

                                    if (playerMoved >= PLAYER_COUNT)
                                        playerMoved = PLAYER_COUNT;
                                    msg >> level1.objects[players[playerMoved].objID].rotation >> level1.objects[players[playerMoved].objID].position;


                                    break;
                                case 2:
                                    msg >> eh;
                                    msg >> objectMoved;

                                    msg >> level1.objects[objectMoved].position >> level1.objects[objectMoved].rotation >> level1.objects[objectMoved].beingHeld;
                                    break;
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

        glm::vec3 disco = glm::vec3(1.0f, 1.0f, 1.0f);

        glUseProgram(lightShader.ID);
        // lightShader.setInt("pointLightCount", 4);
        // lightShader.setInt("spotLightCount", 1);
        if (flashlight)
        {
            lightShader.setVec3("sLights[0].specular", 1.0, 1.0, 1.0);
            lightShader.setVec3("sLights[0].position", level1.objects[players[0].objID].position + camRight * 0.3f);
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
            lightShader.setVec3("pLights[" + std::to_string(i) + "].specular", glm::vec3(2.0f));
            lightShader.setVec3("pLights[" + std::to_string(i) + "].diffuse", glm::vec3(1.0f));
            lightShader.setVec3("pLights[" + std::to_string(i) + "].ambient", glm::vec3(0.5f));
            lightShader.setDouble("pLights[" + std::to_string(i) + "].constant", 1.0f);
            lightShader.setDouble("pLights[" + std::to_string(i) + "].linear", 0.09f);
            lightShader.setDouble("pLights[" + std::to_string(i) + "].quadratic", 0.032f);
        }
        lightShader.setDouble("material.shininess", 32.0);
        lightShader.setVec3("dLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        lightShader.setVec3("dLight.specular", glm::vec3(1.0));
        lightShader.setVec3("dLight.ambient", glm::vec3(0.3f));
        lightShader.setVec3("dLight.diffuse", glm::vec3(0.5f));

        glm::mat4 view = glm::mat4(1.0f);
        glm::vec3 plPos = level1.objects[players[0].objID].position;
        view = glm::lookAt(plPos, plPos + camFront, camUp);

        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", proj);
        lightShader.setDouble("near", nearView);
        lightShader.setDouble("far", farView);

        // glm::vec3 playPos = level1.objects[players[0].objID].position;
        // std::cout << playPos.x << ", " << playPos.y << ", " << playPos.z << "\n";
        // std::cout << players[0].objID << ", " << level1.objects[6].position.z << " 0\n";
        // std::cout << players[1].objID << ", " << level1.objects[7].position.z << " 1\n"; figure out how to make the world model update with the client players
        for (unsigned int i = 0; i < PLAYER_COUNT; ++i)
        {
            plPos = level1.objects[players[i].objID].position + glm::vec3(0.0f, -2.0f, 0.0f);
            glm::vec3 plRot = level1.objects[players[i].objID].rotation;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, plPos);
            model = glm::rotate(model, atan2f(plRot.x, plRot.z), glm::vec3(0.0f, 1.0f, 0.0f));
            lightShader.setMat4("model", model);

            if (i > 0)
                playerModel.draw(lightShader);
        }

        for (int i = 0; i < 4; ++i)
        {
            pointLightPositions[i] = level1.objects[i].position;
        }

        level1.Render(lightShader, proj, view, level1.objects[players[0].objID].position, camFront, objectLookingAt, objectHolding, objectHoldingID, -2.0f, delta_time);

        glfwSwapBuffers(mainWindow);
        glfwPollEvents();
    }

    terminate();
    if (serveronline && serverThread.joinable())
        serverThread.join();
}

void terminate()
{
    glfwTerminate();
}