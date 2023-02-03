#include <string>
#include <ws2tcpip.h>
#include <WinSock2.h>
#include "k-framework.h"
#include "json.hpp"

kfw::core::DatabridgeClient::DatabridgeClient(const std::string& ip, int port) {
    memset(&this->addr, 0x00, sizeof(this->addr));
    this->addr.sin_family = AF_INET;
    this->addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr.S_un.S_addr);
}

kfw::core::DatabridgeClient::~DatabridgeClient() {
    closesocket(this->dbSocket);
}

bool kfw::core::DatabridgeClient::establishConnection(bool reconnectAutomatically) {
    this->reconnectAutomatically = reconnectAutomatically;
    WSADATA wdata;
    WSAStartup(MAKEWORD(2, 0), &wdata);
    closesocket(this->dbSocket);
    this->dbSocket = socket(AF_INET, SOCK_STREAM, 0);
    return connect(this->dbSocket, (SOCKADDR*)&this->addr, sizeof(SOCKADDR)) == 0;
}

bool kfw::core::DatabridgeClient::sendData(DatabridgePacket packet) {
    std::string toSend = packet.toJsonObject().dump();
    toSend.append("\n");

    if (send(this->dbSocket, toSend.c_str(), toSend.length(), 0) == SOCKET_ERROR) {
        if (WSAGetLastError() == WSAENOTCONN) {
            if (this->reconnectAutomatically) {
                this->establishConnection(this->reconnectAutomatically);
            }
        }
        return false;
    }
    return true;
}

bool kfw::core::DatabridgeClient::receivePacket(DatabridgePacket& packet) {
    std::string received;
    char _recvBuffer[1024];
    int recvLen = 0;

    pollfd* toPoll = new pollfd[] { 0 };
    toPoll->fd = this->dbSocket;
    toPoll->events = POLLIN;
    toPoll->revents = POLLIN;
    int pollres = WSAPoll(toPoll, 1, 0);

    if (pollres == 0 || pollres == SOCKET_ERROR) {
        return false;
    }

    if (toPoll->revents & POLLIN == 0) {
        return false;
    }

    while (recvLen = recv(this->dbSocket, _recvBuffer, 1024, 0)) {
        if (recvLen == SOCKET_ERROR) {
            int err = WSAGetLastError();
            std::stringstream ss;
            ss << "Socket Error: " << err;
            Factory::getDefaultLogger()->log(ss.str(), "DatabridgeClient");
            return false;
        }
        received.append(_recvBuffer, recvLen);

        pollfd* toPoll = new pollfd[]{ 0 };
        toPoll->fd = this->dbSocket;
        toPoll->revents = POLLIN;
        pollres = WSAPoll(toPoll, 1, 0);

        if (pollres == 0 || pollres == SOCKET_ERROR) {
            break;
        }

        if (toPoll->revents & POLLIN == 0) {
            break;
        }
    }

    int pos = received.find("\n");
    if (pos != std::string::npos) {
        std::string split = received.substr(0, pos);
        received = received.substr(pos);
        nlohmann::json parsedPacket = nlohmann::json::parse(split);

        packet.id = parsedPacket.value("id", "0");
        packet.type = parsedPacket.value("type", "NONE");
        packet.data = parsedPacket.at("data");
        return true;
    }
    
    return false;
}

void kfw::core::DatabridgeClient::closeConnection() {
    closesocket(this->dbSocket);
}

kfw::core::DatabridgePacket::DatabridgePacket() {
    this->id = "0";
    this->type = "NONE";
    this->data = nlohmann::json::object();
}

kfw::core::DatabridgePacket::DatabridgePacket(std::string type, nlohmann::json data) {
    this->id = "0";
    this->type = type;
    this->data = data;
}

nlohmann::json kfw::core::DatabridgePacket::toJsonObject() {
    auto packet = nlohmann::json::object({ { "id", this->id }, { "type", this->type }, { "data", this->data } });
    return packet;
}
