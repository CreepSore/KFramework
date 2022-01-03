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

    if (send(this->dbSocket, toSend.c_str(), toSend.length(), 0) != 0) {
        if (this->reconnectAutomatically) {
            this->establishConnection(this->reconnectAutomatically);
        }
        return false;
    };
    return true;
}

bool kfw::core::DatabridgeClient::receivePacket(DatabridgePacket& packet) {
    std::string received;
    char _recvBuffer[1024];
    int recvLen = 0;
    while (recvLen = recv(this->dbSocket, _recvBuffer, 1024, 0)) {
        received.append(_recvBuffer, recvLen);
    }

    int pos = received.find("\n");
    if (pos != std::string::npos) {
        std::string split = this->recvBuffer.substr(0, pos);
        this->recvBuffer = this->recvBuffer.substr(pos);
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
