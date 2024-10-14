#include <iostream>
#include <fstream>
#include <string>
#include <json/json.h> 
#include <winsock2.h> 
#include <Windows.h> 

#pragma comment(lib, "ws2_32.lib") 

using namespace std;

const string config_file = "config.json";

struct Config {
    string trigger_hotkey;
    bool always_enabled;
    double trigger_delay;
    double base_delay;
    int color_tolerance;
    double zone;
    int port;
};

Config loadConfig() {
    Config config;
    if (!ifstream(config_file).good()) {
        config.trigger_hotkey = "0x06";
        config.always_enabled = false;
        config.trigger_delay = 0.001;
        config.base_delay = 0.01;
        config.color_tolerance = 70;
        config.zone = 1.5;
        config.port = 65422;

        ofstream file(config_file);
        Json::Value jsonConfig;
        jsonConfig["trigger_hotkey"] = config.trigger_hotkey;
        jsonConfig["always_enabled"] = config.always_enabled;
        jsonConfig["trigger_delay"] = config.trigger_delay;
        jsonConfig["base_delay"] = config.base_delay;
        jsonConfig["color_tolerance"] = config.color_tolerance;
        jsonConfig["zone"] = config.zone;
        jsonConfig["port"] = config.port;
        file << jsonConfig;
        cout << "No config.json found. Default configuration created: " << config_file << endl;
    }
    else {
        ifstream file(config_file);
        Json::Value jsonConfig;
        file >> jsonConfig;
        config.trigger_hotkey = jsonConfig["trigger_hotkey"].asString();
        config.always_enabled = jsonConfig["always_enabled"].asBool();
        config.trigger_delay = jsonConfig["trigger_delay"].asDouble();
        config.base_delay = jsonConfig["base_delay"].asDouble();
        config.color_tolerance = jsonConfig["color_tolerance"].asInt();
        config.zone = jsonConfig["zone"].asDouble();
        config.port = jsonConfig["port"].asInt();
    }
    return config;
}

int main() {
    Config config = loadConfig();
    cout << "Loading..." << endl;
    cout << "Please open HW FREE.exe" << endl;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed" << endl;
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "socket failed" << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(config.port);
    if (bind(sock, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        cerr << "bind failed" << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    if (listen(sock, 1) == SOCKET_ERROR) {
        cerr << "listen failed" << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    cout << "Listening..." << endl;

    SOCKET conn = accept(sock, NULL, NULL);
    if (conn == INVALID_SOCKET) {
        cerr << "accept failed" << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    cout << "Connected. Loaded." << endl;

    while (true) {
        char data[1];
        int bytesRead = recv(conn, data, 1, 0);
        if (bytesRead > 0) {
            char key = data[0];
            if (isalnum(key)) {
                INPUT ip;
                ip.type = INPUT_KEYBOARD;
                ip.ki.wScan = 0;
                ip.ki.time = 0;
                ip.ki.dwExtraInfo = 0;
                ip.ki.dwFlags = 0;
                ip.ki.wVk = VkKeyScan(key);
                SendInput(1, &ip, sizeof(INPUT));

                ip.ki.dwFlags = KEYEVENTF_KEYUP;
                SendInput(1, &ip, sizeof(INPUT));
            }
        }
        else {
            break;
        }
    }

    closesocket(conn);
    closesocket(sock);
    WSACleanup();
    return 0;
}
