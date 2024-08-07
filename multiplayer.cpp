#include "multiplayer.h"

size_t message::size() const
{
    return sizeof(message_header) + body.size();
}

template <typename T>
void queue<T>::push_back(const T &input)
{
    std::scoped_lock lock(mux_);
    deq_.emplace_back(std::move(input));

    std::unique_lock<std::mutex> ul(muxB_);
    cvB_.notify_one();
}
template <typename T>
void queue<T>::push_front(const T &input)
{
    std::scoped_lock lock(mux_);
    deq_.emplace_front(std::move(input));

    std::unique_lock<std::mutex> ul(muxB_);
    cvB_.notify_one();
}

// template <typename T>
// bool queue<T>::empty()
// {
//     std::scoped_lock lock(mux_);
//     return deq_.empty();
// }

template <typename T>
size_t queue<T>::length()
{
    std::scoped_lock lock(mux_);
    return deq_.size();
}

template <typename T>
void queue<T>::clear()
{
    std::scoped_lock lock(mux_);
    deq_.clear();
}

// template <typename T>
// T queue<T>::pop_front()
// {
//     std::scoped_lock lock(mux_);
//     auto t = std::move(deq_.front());
//     deq_.pop_front();
//     return t;
// }
template <typename T>
T queue<T>::pop_back()
{
    std::scoped_lock lock(mux_);
    auto t = std::move(deq_.back());
    deq_.pop_back();
    return t;
}

template <typename T>
void queue<T>::wait(bool doSomething)
{
    while (empty() && doSomething)
    {
        std::unique_lock<std::mutex> ul(muxB_);
        cvB_.wait(ul);
    }
}

connection::connection(owner parent, asio::io_context &context, tcp::socket socket, queue<owned_message> &qin)
    : io_(context), socket_(std::move(socket)), queueIn_(qin)
{
    ownerType_ = parent;

    if (ownerType_ == owner::server)
    {
        handshakeOut = uint64_t(std::chrono::system_clock::now().time_since_epoch().count());

        handshakeCheck = scramble(handshakeOut);
    }
}
connection::~connection() {}

void connection::ConnectToServer(const tcp::resolver::results_type &endP)
{
    if (ownerType_ != owner::client)
        return;

    // validationTimer = 10.0;
    asio::async_connect(socket_, endP,
                        [this](std::error_code ec, tcp::endpoint endpoint)
                        {
                            if (ec)
                            {
                                std::cout << "\n\tServer Connection Error: " << ec.message() << "\n";
                                return;
                            }
                            ReadValidation();
                        });
}
bool connection::ConnectToClient(server *server, uint32_t uid)
{
    // return true; sigh

    if (ownerType_ != owner::server)
        return false;

    if (socket_.is_open())
    {
        // validationTimer = 10.0;
        id = uid;
        WriteValidation();
        ReadValidation(server);
    }
    return true;
}
bool connection::Disconnect()
{
    if (!IsConnected())
        return true;

    asio::post(io_, [this]()
               { socket_.close(); });

    return true;
}
bool connection::IsConnected() const
{
    return socket_.is_open();
}

void connection::Send(const message &msg)
{
    asio::post(io_,
               [this, msg]()
               {
                   bool emptyQueue = !queueOut_.empty();
                   queueOut_.push_back(msg);
                   if (!emptyQueue)
                   {
                       WriteHeader();
                   }
               });
}

uint32_t connection::GetID() const
{
    return id;
}

void connection::ReadHeader()
{
    asio::async_read(socket_, asio::buffer(&msgTmp_.header, sizeof(message_header)),
                     [this](std::error_code ec, std::size_t length)
                     {
                         if (ec)
                         {
                             std::cout << "Read Header Failed: " << ec.message() << "\n";
                             socket_.close();
                             return;
                         }

                         if (msgTmp_.header.size > 8)
                         {                                             // wouldn't that be because the 8 bytes are the header id???
                             msgTmp_.body.resize(msgTmp_.header.size); // for some reason msgtmp being 8 bytes less leads to a strange second message of random id to be read
                             ReadBody();
                             return;
                         }

                         AddToIncomingMessageQueue();
                     });
}
void connection::ReadBody()
{
    asio::async_read(socket_, asio::buffer(msgTmp_.body.data(), msgTmp_.body.size()),
                     [this](std::error_code ec, std::size_t length)
                     {
                         if (ec)
                         {
                             std::cout << "\n\tRead Body Failed: " << ec.message() << "\n";
                             socket_.close();
                             return;
                         }

                         AddToIncomingMessageQueue();
                     });
}
void connection::WriteHeader()
{
    asio::async_write(socket_, asio::buffer(&queueOut_.front().header, sizeof(message_header)),
                      [this](std::error_code ec, size_t length)
                      {
                          if (ec)
                          {
                              std::cout << "\n\tWrite Header Fail: " << ec.message() << "\n";
                              socket_.close();
                              return;
                          }

                          if (queueOut_.front().body.size() > 0)
                          {
                              WriteBody();
                          }
                          else
                          {
                              //   std::cout << queueOut_.front().header.id << " whyyyyy 2\n";
                              queueOut_.pop_front();

                              if (!queueOut_.empty())
                                  WriteHeader();
                          }
                      });
}
void connection::WriteBody()
{
    asio::async_write(socket_, asio::buffer(queueOut_.front().body.data(), queueOut_.front().size()),
                      [this](std::error_code ec, size_t length)
                      {
                          if (ec)
                          {
                              std::cout << "\n\tWrite Body Fail: " << ec.message() << "\n";
                              socket_.close();
                              return;
                          }

                          queueOut_.pop_front();

                          if (!queueOut_.empty())
                          {
                              WriteHeader();
                          }
                      });
}

void connection::AddToIncomingMessageQueue()
{
    if (ownerType_ == owner::server)
    {
        queueIn_.push_back({this->shared_from_this(), msgTmp_});
    }
    else
    {
        queueIn_.push_back({nullptr, msgTmp_});
    }

    ReadHeader();
}

// !! remember to change the scramble function before you export !!

// technically that makes the function close-sourced? But I don't think
// anyone would really care considering how it's just scrambling some
// bytes and stuff, and !! EVERYONE SHOULD MAKE THEIR OWN SCRAMBLE FUNCTION
// BEFORE BUILDING FOR SECURITY PURPOSES !!

// !! just change some of the numbers and letters below if you
// don't know how to !!

// network security is hard :(
// honestly just using passwords would probably be easiest and most secure
uint64_t connection::scramble(uint64_t input)
{
    // uint64_t out = input ^ (id * 0x9999999);
    // out = (out & 0xE4E4E4E46E6E5E5E) >> 3 | (out & 0x7B7B7B7B) << 5;
    // return out ^ 0x777777777;
    return input;
}

void connection::WriteValidation()
{
    asio::async_write(socket_, asio::buffer(&handshakeOut, sizeof(uint64_t)),
                      [this](std::error_code ec, std::size_t length)
                      {
                          if (ec)
                          {
                              std::cout << "\n\tFailed to send validation to client\n";
                              socket_.close();
                              return;
                          }

                          if (ownerType_ == owner::client)
                          {
                              //   if (validationTimer > 0.0)
                              //   {
                              //       validationTimer -= 0.5f;
                              //   ReadValidation();
                              //   return;
                              //   }

                              ReadHeader();
                          }
                      });
}
void connection::ReadValidation(server *server)
{
    asio::async_read(socket_, asio::buffer(&handshakeIn, sizeof(uint64_t)),
                     [this, server](std::error_code ec, std::size_t length)
                     {
                         if (ec)
                         {
                             std::cout << "\n\tClient failed to read validation\n";
                             socket_.close();
                             return;
                         }
                         if (ownerType_ == owner::client)
                         {
                             handshakeOut = scramble(handshakeIn);
                             WriteValidation();
                             return;
                         }

                         if (handshakeIn == handshakeCheck)
                         {
                             server->OnClientValidated(this->shared_from_this());

                             ReadHeader();
                         }
                         else
                         {
                             std::cout << "\n\tClient validation failed\n";
                             //  if (validationTimer > 0.0)
                             //  {
                             //      validationTimer -= 0.5f;
                             //  WriteValidation();
                             //  ReadValidation(server);
                             //  }
                             //  else
                             //  {
                             socket_.close();
                             //  }
                         }
                     });
}

client::client()
{
}
client::~client()
{
    Disconnect();
}

void client::Update(double delta_time)
{
}
// bool client::queueEmpty()
// {
//     return msgIn_.empty();
// }

bool client::Connect(const std::string &host, const std::string port)
{
    try
    {
        connection_ = std::make_unique<connection>(connection::owner::client, io_, tcp::socket(io_), msgIn_);

        tcp::resolver resolver(io_);
        tcp::resolver::results_type endpoints = resolver.resolve(host, port);

        connection_->ConnectToServer(endpoints);

        thr_ = std::thread([this]()
                           { io_.run(); });
    }
    catch (const std::exception &e)
    {
        std::cerr << "\n\tClient Connection Failed: " << e.what() << '\n';
    }

    return true;
}

void client::UpdatePlayer(glm::vec3 position, glm::vec3 rotation)
{
    message msg;
    msg.header.id = 1;

    msg << position << rotation;

    connection_->Send(msg);
}
void client::UpdateObject(unsigned int ID, glm::vec3 position, glm::vec3 rotation)
{
    message msg;
    msg.header.id = 2;

    msg << rotation << position << ID;

    connection_->Send(msg);
}
void client::SendObject(object obj, const unsigned int objModelID)
{
    message msg;
    msg.header.id = 3;

    msg << obj.solid << obj.collidable << obj.obtainable << obj.tScale << obj.rotation << obj.scale << obj.position << objModelID;

    connection_->Send(msg);
}
void client::RequestLevelDownload()
{
    message msg;
    msg.header.id = 4;

    connection_->Send(msg);
}
void client::CancelObjectUpdates(unsigned int ID)
{
    message msg;
    msg.header.id = 20;

    msg << ID;

    connection_->Send(msg);
}
void client::ConnectionGreeting()
{
    message msg;
    msg.header.id = 0;

    connection_->Send(msg);
}

void client::Disconnect()
{
    if (IsConnected())
        connection_->Disconnect();

    io_.stop();

    if (thr_.joinable())
        thr_.join();

    connection_.release();
}

bool client::IsConnected()
{
    if (connection_)
        return connection_->IsConnected();

    return false;
}

queue<owned_message> &client::Incoming()
{
    return msgIn_;
}

server::server(uint16_t port) : acc_(io_, tcp::endpoint(tcp::v4(), port))
{
}
server::~server()
{
    Stop();
}
bool server::Start()
{
    try
    {
        WaitForClientConnection();

        thr_ = std::thread([this]()
                           { io_.run(); }); // this might be the culprite for the server hanging on quit application
    }
    catch (const std::exception &e)
    {
        std::cerr << "\n\tServer Startup Fail: " << e.what() << '\n';
    }

    std::cout << "\n\tServer Online\n";
    return true;
}
void server::Stop()
{
    io_.stop();

    if (thr_.joinable())
        thr_.join();

    std::cout << "\n\tServer Offline\n";
}

void server::WaitForClientConnection()
{
    acc_.async_accept([this](std::error_code ec, tcp::socket socket)
                      {
                            if (ec)
                            {
                                std::cout << "\n\tServer Connection Failed: " << ec.message() << "\n";
                                socket.close();
                                WaitForClientConnection();
                            }
                            std::cout << "\n\tClient Connected: " << socket.remote_endpoint() << "\n";
                            std::shared_ptr<connection> newConnection = std::make_shared<connection>(connection::owner::server,
                                                                                                    io_, std::move(socket), msgIn_);

                            if (OnClientConnect(newConnection))
                            {
                                connections_.push_back(std::move(newConnection));
                                connections_.back()->ConnectToClient(this, ++idCounter_);

                                std::cout << "\n\tServer Connection Succeeded: " << connections_.back()->GetID() << "\n";
                            }
                            else
                            {
                                std::cout << "\n\tServer Connection Failed: Connection Denied\n";
                            }
                            WaitForClientConnection(); });
}

void server::MessageClient(std::shared_ptr<connection> client, const message &msg)
{
    if (!client || !client->IsConnected())
    {
        OnClientDisconnect(client);
        client.reset();
        connections_.erase(
            std::remove(connections_.begin(), connections_.end(), client), connections_.end());
        return;
    }

    client->Send(msg);
}
void server::MessageAllClients(const message &msg, std::shared_ptr<connection> cIgnore, std::shared_ptr<connection> cIgnore2)
{
    for (auto &client : connections_)
    {
        if (!client || !client->IsConnected())
        {
            OnClientDisconnect(client);
            client.reset();
            continue;
        }

        if (client == cIgnore || client == cIgnore2)
            continue;

        client->Send(msg);
    }

    connections_.erase(
        std::remove(connections_.begin(), connections_.end(), nullptr), connections_.end());
}

bool server::OnClientConnect(std::shared_ptr<connection> client)
{
    std::cout << "\n\tClient Connection Succeeded: " << client->GetID();
    return true;
}
void server::OnClientDisconnect(std::shared_ptr<connection> client)
{
}
void server::OnMessage(std::shared_ptr<connection> client, message &msg, world &level)
{
    if (msg.header.id == 0)
    {
        std::cout << "client " << client->GetID() << " sent connection greetings\n";
        // MessageClient(client, msg);

        // message newMsg;
        // newMsg.header.id = 0;
        // unsigned int stupid = 32929292;
        // newMsg << stupid << stupid << stupid;
        // MessageAllClients(newMsg);
    }
    if (msg.header.id == 1)
    {
        message newMsg;
        newMsg.header.id = 1;
        glm::vec3 pos, rot;

        message_header msgtempheader;
        msg >> msgtempheader;
        msg >> rot >> pos;
        newMsg << pos << rot << client->GetID();

        MessageAllClients(newMsg, client);
    }
    if (msg.header.id == 2)
    {
        message_header temp;
        msg >> temp;
        unsigned int ID;
        msg >> ID;

        msg >> level.objects[ID].position >> level.objects[ID].rotation;

        int tID = 0;
        for (unsigned int i = 0; i < connections_.size(); ++i)
        {
            if (connections_.at(i) == client)
                tID = i;
        }
        level.objects[ID].beingHeld = tID;

        // message newMsg;
        // newMsg.header.id = 2;
        // newMsg << level.objects[ID].beingHeld << level.objects[ID].rotation << level.objects[ID].position << ID;
        // MessageClient(connections_.front(), newMsg);
    }
    if (msg.header.id == 3)
    {
        message_header temp;
        msg >> temp;
        object tempObj; // add object msg output
        unsigned int objModelID;
        msg >> objModelID >> tempObj.position >> tempObj.scale >> tempObj.rotation >> tempObj.tScale >> tempObj.obtainable >> tempObj.collidable >> tempObj.solid;

        message newMsg;
        newMsg.header.id = 3;
        newMsg << tempObj.solid << tempObj.collidable << tempObj.obtainable << tempObj.tScale << tempObj.rotation << tempObj.scale << tempObj.position << objModelID;

        MessageAllClients(newMsg, client);
    }
    if (msg.header.id == 20)
    {
        message_header temp;
        msg >> temp;
        unsigned int ID;
        msg >> ID;

        level.objects[ID].beingHeld = -1;

        // message newMsg;
        // newMsg.header.id = 2;
        // newMsg << level.objects[ID].beingHeld << level.objects[ID].rotation << level.objects[ID].position << ID;
        // MessageClient(connections_.front(), newMsg);
    }
}

void server::Update(size_t messageLimit, bool &doSomething, double delta_time, world &level, float &msgUpdate)
{
    msgIn_.empty();
    // msgIn_.wait(doSomething);
    if (msgUpdate < 0.0f)
    {
        for (unsigned int i = 0; i < level.objects.size(); ++i)
        {
            if (!level.objects[i].obtainable || level.objects[i].position == level.objects[i].prevPosition)
                continue;

            message msg;
            msg.header.id = 2;
            msg << level.objects[i].beingHeld << level.objects[i].rotation << level.objects[i].position << i;

            if (level.objects[i].beingHeld < 1)
                MessageAllClients(msg, connections_.front());
            else
                MessageAllClients(msg, connections_.at(level.objects[i].beingHeld), connections_.front());
        }

        // if (level.objects.size() == prevLevelObjCount)
        //     return; // add server and client updates for placing objects

        // MessageAllClients(msg, connections_.front());
    }

    size_t messageCount = 0;

    while (messageCount < messageLimit && !msgIn_.empty())
    {
        auto msg = msgIn_.pop_front();

        OnMessage(msg.remote, msg.msg, level);

        ++messageCount;
    }
}

void server::OnClientValidated(std::shared_ptr<connection> client)
{
    message yayMsg;
    yayMsg.header.id = 0;

    MessageClient(client, yayMsg);
}

unsigned int server::ConnectionCount()
{
    return connections_.size();
}