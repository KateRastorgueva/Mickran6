#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

void writeLog(const std::string& filename, const std::string& message) {
    std::ofstream logFile(filename, std::ios::app);
    if (logFile.is_open()) {
        std::time_t now = std::time(nullptr);
        char timestamp[100];
        std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        std::cout << "[" << timestamp << "] " << message << std::endl;
        logFile << "[" << timestamp << "] " << message << std::endl;
        logFile.close();
    }
}

int main() {
    const char* socketPath = "/tmp/project6_socket";
    const char* logFile = "cpp_server.log";
    const char* myId = "CppServer";

    unlink(socketPath);

    int serverFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serverFd == -1) {
        writeLog(logFile, "ОШИБКА: Не удалось создать сокет");
        return 1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath, sizeof(addr.sun_path) - 1);

    if (bind(serverFd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        writeLog(logFile, "ОШИБКА: Не удалось привязать сокет");
        close(serverFd);
        return 1;
    }

    if (listen(serverFd, 5) == -1) {
        writeLog(logFile, "ОШИБКА: Не удалось начать прослушивание");
        close(serverFd);
        return 1;
    }

    writeLog(logFile, "Сервер запущен. Ожидание подключения...");

    int clientFd = accept(serverFd, NULL, NULL);
    if (clientFd == -1) {
        writeLog(logFile, "ОШИБКА: Не удалось принять подключение");
        close(serverFd);
        return 1;
    }

    writeLog(logFile, "Клиент подключился");

    char buffer[256] = {0};
    int bytesRead = read(clientFd, buffer, sizeof(buffer) - 1);
    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        writeLog(logFile, "Получено от клиента: " + std::string(buffer));
    }

    write(clientFd, myId, strlen(myId));

    close(clientFd);
    close(serverFd);
    unlink(socketPath);

    writeLog(logFile, "Сервер завершил работу");
    return 0;
}
