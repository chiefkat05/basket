// #include "../gfx/graphics.h"
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

struct player
{
    uint32_t id = 0;
    unsigned int objID;
};
#define PLAYER_COUNT 5
player players[PLAYER_COUNT];

world level1;

void playerInput()
{
    if (ehandler.requestKeyState(GLFW_KEY_A))
    {
        level1.objects[players[0].objID].velocity.x += -16.0f * camTrueRight.x;
        level1.objects[players[0].objID].velocity.z += -16.0f * camTrueRight.z;
    }
    if (ehandler.requestKeyState(GLFW_KEY_D))
    {
        level1.objects[players[0].objID].velocity.x += 16.0f * camTrueRight.x;
        level1.objects[players[0].objID].velocity.z += 16.0f * camTrueRight.z;
    }
    if (ehandler.requestKeyState(GLFW_KEY_S))
    {
        level1.objects[players[0].objID].velocity.x += -16.0f * camFrontAlign.x;
        level1.objects[players[0].objID].velocity.z += -16.0f * camFrontAlign.z;
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
        level1.objects[players[0].objID].velocity.x += 16.0f * camFrontAlign.x;
        level1.objects[players[0].objID].velocity.z += 16.0f * camFrontAlign.z;
    }
    else
        playerRunning = false;

    if (abs(level1.objects[players[0].objID].velocity.x) > 1.0f || abs(level1.objects[players[0].objID].velocity.z) > 1.0f)
    {
        glm::vec2 normWalk = glm::vec2(level1.objects[players[0].objID].velocity.x, level1.objects[players[0].objID].velocity.z);
        normWalk = glm::normalize(normWalk);
        level1.objects[players[0].objID].velocity.x = normWalk.x * 16.0f * (1.0f + playerRunning);
        level1.objects[players[0].objID].velocity.z = normWalk.y * 16.0f * (1.0f + playerRunning); // keep working on this
    }

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

    if (ehandler.requestKeyState(GLFW_KEY_SPACE) == 2 && level1.objects[players[0].objID].onGround)
    {
        level1.objects[players[0].objID].velocity.y = 8.0f + 0.2f * glm::length(level1.objects[players[0].objID].velocity);
        level1.objects[players[0].objID].onGround = false;
    }

    if (ehandler.requestKeyState(GLFW_KEY_F) == 2)
        flashlight = !flashlight;

    if (objectHolding != nullptr)
    {
        objectHolding->position = level1.objects[players[0].objID].position + camFront * 2.2f - camUp * 0.6f;
        objectHolding->rotation.y = glm::degrees(atan2f(camFrontAlign.x, camFrontAlign.z)) - 90.0f;

        objectHolding->rotation.z = -glm::degrees(glm::acos(camFront.y));

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

    if (ehandler.requestKeyState(GLFW_KEY_Y) == 2)
    {
        level1.PlaceObject(level1.models[1].directory, level1.models[1].name, level1.objects[players[0].objID].position + camFront * 8.0f,
                           glm::vec3(1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f), false, true, true);

        if (clientonline && clientvalidated <= 0.0f)
        {
            unsigned int lastObj = level1.objects.size() - 1;
            mClient->SendObject(level1.objects[lastObj], 1);
        }
    }
    if (ehandler.requestKeyState(GLFW_KEY_U) == 2)
    {
        level1.PlaceObject(level1.models[0].directory, level1.models[0].name, level1.objects[players[0].objID].position + camFront * 8.0f,
                           glm::vec3(1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f), false, true, true);

        if (clientonline && clientvalidated <= 0.0f)
        {
            unsigned int lastObj = level1.objects.size() - 1;
            mClient->SendObject(level1.objects[lastObj], 0);
        }
    }
    if (ehandler.requestKeyState(GLFW_KEY_I) == 2)
    {
        level1.PlaceObject(level1.models[2].directory, level1.models[2].name, level1.objects[players[0].objID].position + camFront * 8.0f,
                           glm::vec3(1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f), false, true, true);

        if (clientonline && clientvalidated <= 0.0f)
        {
            unsigned int lastObj = level1.objects.size() - 1;
            mClient->SendObject(level1.objects[lastObj], 2);
        }
    }
    if (ehandler.requestKeyState(GLFW_KEY_O) == 2)
    {
        level1.PlaceObject(level1.models[3].directory, level1.models[3].name, level1.objects[players[0].objID].position + camFront * 8.0f,
                           glm::vec3(1.0f),
                           glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f), false, true, true);
        if (clientonline && clientvalidated <= 0.0f)
        {
            unsigned int lastObj = level1.objects.size() - 1;
            mClient->SendObject(level1.objects[lastObj], 3);
        }
    }
    if (ehandler.requestKeyState(GLFW_KEY_P) == 2)
    {
        level1.PlaceObject(level1.models[4].directory, level1.models[4].name, level1.objects[players[0].objID].position + camFrontAlign * 8.0f,
                           glm::vec3(1.0f),
                           glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f), false, true, true);
        if (clientonline && clientvalidated <= 0.0f)
        {
            unsigned int lastObj = level1.objects.size() - 1;
            mClient->SendObject(level1.objects[lastObj], 4);
        }
    }
    if (ehandler.requestKeyState(GLFW_KEY_J) == 2)
    {
        level1.PlaceObject(level1.models[5].directory, level1.models[5].name, level1.objects[players[0].objID].position + camFrontAlign * 8.0f,
                           glm::vec3(1.0f),
                           glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f), false, true, true);
        if (clientonline && clientvalidated <= 0.0f)
        {
            unsigned int lastObj = level1.objects.size() - 1;
            mClient->SendObject(level1.objects[lastObj], 5);
        }
    }
    if (ehandler.requestKeyState(GLFW_KEY_K) == 2)
    {
        level1.PlaceObject(level1.models[6].directory, level1.models[6].name, level1.objects[players[0].objID].position + camFrontAlign * 8.0f,
                           glm::vec3(1.0f),
                           glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f), false, true, true);
        if (clientonline && clientvalidated <= 0.0f)
        {
            unsigned int lastObj = level1.objects.size() - 1;
            mClient->SendObject(level1.objects[lastObj], 6);
        }
    }
    if (ehandler.requestKeyState(GLFW_KEY_L) == 2)
    {
        level1.PlaceObject(level1.models[7].directory, level1.models[7].name, level1.objects[players[0].objID].position + camFrontAlign * 8.0f,
                           glm::vec3(1.0f),
                           glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f), false, true, true);
        if (clientonline && clientvalidated <= 0.0f)
        {
            unsigned int lastObj = level1.objects.size() - 1;
            mClient->SendObject(level1.objects[lastObj], 7);
        }
    }

    if (ehandler.requestKeyState(GLFW_KEY_ESCAPE) == 2)
    {
        glfwSetWindowShouldClose(mainWindow, true);

        if (serveronline)
            hostServer->Stop();

        serveronline = false;
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

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(4.3f, 0.0f, -28.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)};

    gfx::mesh playerModel("../gfx/models/player", "/player.obj"); // this is the origin of the problem with the player textures

    level1.PlaceObject("../gfx/models/terrain/simple", "/plane.obj", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(800.0f, 1.0f, 800.0f), glm::vec3(1.0f), glm::vec2(160.0f, 160.0f), false, true, true);
    // too bloated
    level1.PlaceObject("../gfx/models/items", "/carrot.obj", glm::vec3(0.8f, 5.0f, -3.0f), glm::vec3(1.0f),
                       glm::vec3(0.0f, 160.0f, 0.0f), glm::vec2(1.0f), true, true, false);
    level1.PlaceObject("../gfx/models/items", "/carrot.obj", glm::vec3(-0.4f, 5.0f, -3.8f), glm::vec3(1.5f),
                       glm::vec3(0.0f, 20.0f, 0.0f), glm::vec2(1.0f), true, true, false);
    level1.PlaceObject("../gfx/models/items", "/carrot.obj", glm::vec3(0.5f, 5.0f, -6.0f), glm::vec3(2.0f),
                       glm::vec3(0.0f, -60.0f, 0.0f), glm::vec2(1.0f), true, true, false);

    level1.PlaceObject("../gfx/models/enemies/smoosh", "/smooshman.obj", glm::vec3(4.0f, 1.5f, 0.0f), glm::vec3(1.0f), glm::vec3(0.0f), glm::vec2(1.0f), false, true, false);
    level1.PlaceObject("../gfx/models/enemies/smoosh", "/smooshman.obj", glm::vec3(4.0f, 1.5f, 0.0f), glm::vec3(0.5f), glm::vec3(0.0f), glm::vec2(1.0f), false, true, false);
    level1.PlaceObject("../gfx/models/enemies/smoosh", "/smooshman.obj", glm::vec3(4.0f, 1.5f, 0.0f), glm::vec3(1.5f), glm::vec3(0.0f), glm::vec2(1.0f), false, true, false);
    // unsigned int deathCubeID = level1.objects.size() - 1;

    for (int i = 0; i < 128; ++i)
    {
        float randX = static_cast<float>(rand() % 2400 - 1200) * 0.1f;
        float randZ = static_cast<float>(rand() % 2400 - 1200) * 0.1f;
        float randRot = static_cast<float>(rand() % 900) * 0.1f;
        level1.PlaceObject("../gfx/models/terrain/bush-1", "/bush.obj", glm::vec3(randX, 0.0f, randZ), glm::vec3(1.0f), glm::vec3(0.0f, randRot, 0.0f));
    }
    level1.PlaceObject("../gfx/models/structures/badstairs", "/badstairs.obj", glm::vec3(-5.0f, 0.0f, -25.0f), glm::vec3(3.0f), glm::vec3(0.0f), glm::vec2(1.0f), false, true, true);
    level1.PlaceObject("../gfx/models/structures/what", "/what.obj", glm::vec3(-15.0f, 0.0f, -25.0f), glm::vec3(1.0f), glm::vec3(0.0f), glm::vec2(1.0f), false, true, true);
    level1.PlaceObject("../gfx/models/structures/badarch", "/badarch.obj", glm::vec3(15.0f, 0.0f, -25.0f), glm::vec3(5.0f), glm::vec3(0.0f), glm::vec2(1.0f), false, true, true);

    // glm::vec3 randPosition = glm::vec3(float(rand() % 6 + 1) * 2.2f, -2.0f, float(rand() % 13 + 3) * 2.2f);
    glm::vec3 randPosition = glm::vec3(4.0f, -2.0f, -8.0f);
    for (int i = 0; i < 10; ++i)
    {
        // float yRot = (rand() % 8) * 45.0f;
        float yRot = 0.0f;
        glm::vec3 pos = randPosition + (static_cast<float>(i + 1) * 4.0f) + (sin(yRot) + cos(yRot));
        // glm::vec3 pos = randPosition * static_cast<float>(i + 1) * 4.0f;
        // glm::vec3 pos = glm::vec3(0.0f);

        level1.PlaceObject("../gfx/models/walls/W-o1", "/w-01.obj", glm::vec3(pos.x * (i % 4 - 1), -.0f, pos.z), glm::vec3(1.0f - i * 0.1f),
                           glm::vec3(0.0f, yRot, 0.0f), glm::vec3(1.0f), false, true, true);
        // level1.PlaceObject("../gfx/models/walls/cube/cube.obj", glm::vec3(pos.x * (i % 4 - 1), -1.0f, pos.z), glm::vec3(1.0f),
        //                    glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f), false, true);
    }

    for (int i = 0; i < PLAYER_COUNT; ++i)
    {
        level1.PlaceObject("../gfx/models/player", "/player.obj",
                           glm::vec3(-3.0f, -100.0f, -5.0f), glm::vec3(1.0f),
                           glm::vec3(1.0f), glm::vec2(1.0f),
                           false, false, false);

        players[i].objID = level1.objects.size() - 1;
        level1.objects[players[i].objID].invisible = true;
        level1.objects[players[i].objID].collisionMeshID = 0;
    }
    level1.objects[players[0].objID].position.y = 4.0f;
    level1.objects[players[0].objID].collidable = true;

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

        ehandler.poll();
        glfwPollEvents();

        playerInput();

        prevCamFront = camFront;

        // glm::vec3 distToPlayer = glm::vec3(level1.objects[deathCubeID].position.x - level1.objects[players[0].objID].position.x,
        //                                    0.0f, level1.objects[deathCubeID].position.z - level1.objects[players[0].objID].position.z);
        // glm::vec3 normDistToPlayer = glm::normalize(distToPlayer);
        // if (std::abs(distToPlayer.x * distToPlayer.z) > 1.0f && std::abs(distToPlayer.x * distToPlayer.z) < 45.0f)
        //     level1.objects[deathCubeID].position -= delta_time * 3.0f * normDistToPlayer;

        // level1.objects[deathCubeID].rotation.y = atan2()

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

                        object temp;
                        unsigned int objModelID;
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
                            if (!cl.queueEmpty())
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
                                case 3:
                                    msg >> eh;
                                    msg >> objModelID >> temp.position >> temp.scale >> temp.rotation >> temp.tScale >> temp.obtainable >> temp.collidable >> temp.solid;

                                    level1.PlaceObject(level1.models[objModelID].directory, level1.models[objModelID].name, temp.position, temp.scale, temp.rotation, temp.tScale, temp.obtainable, temp.collidable, temp.solid);
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

        glm::vec3 disco = glm::vec3(10.0f, 10.0f, 10.0f);

        glUseProgram(lightShader.ID);
        // lightShader.setInt("pointLightCount", 4);
        // lightShader.setInt("spotLightCount", 1);
        if (flashlight)
        {
            lightShader.setVec3("sLights[0].specular", 1.0, 1.0, 1.0);
            lightShader.setVec3("sLights[0].position", level1.objects[players[0].objID].position + camRight * 0.3f + camUp * 1.5f);
            lightShader.setVec3("sLights[0].direction", camFront + camRight * -0.05f);
            lightShader.setDouble("sLights[0].constant", 1.0f);
            lightShader.setDouble("sLights[0].linear", 0.24f);
            lightShader.setDouble("sLights[0].quadratic", 0.008f);
            lightShader.setDouble("sLights[0].cutOff", glm::cos(glm::radians(25.0f)));
            lightShader.setDouble("sLights[0].outerCutOff", glm::cos(glm::radians(29.0f)));
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
        glm::vec3 plPos = level1.objects[players[0].objID].position + glm::vec3(0.0f, 2.0f, 0.0f);
        view = glm::lookAt(plPos, plPos + camFront, camUp);

        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", proj);
        lightShader.setDouble("near", nearView);
        lightShader.setDouble("far", farView);

        for (unsigned int i = 0; i < PLAYER_COUNT; ++i)
        {
            plPos = level1.objects[players[i].objID].position;
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