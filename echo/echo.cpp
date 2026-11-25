#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

void print_help(const char* prog_name) {
    std::cout << "Использование: " << prog_name <<"\n"
              << "  -h, --help         Показать эту справку\n"
              << "  -i, --ip <адрес>   (по умолчанию: 172.16.40.13)\n"
              << "  -p, --port <порт>  (по умолчанию: 7)\n";
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        print_help(argv[0]);
        return 0;
    }

    std::string ip = "172.16.40.13";
    int port = 7;

    for (int i = 1; i < argc; i = i + 1) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            print_help(argv[0]);
            return 0;
        } else if (arg == "-i" || arg == "--ip") {
            if (i + 1 >= argc) {
                std::cerr << "Ошибка: после " << arg << " нужно указать IP-адрес\n";
                return 1;
            }
            i = i + 1;
            ip = argv[i];
        } else if (arg == "-p" || arg == "--port") {
            if (i + 1 >= argc) {
                std::cerr << "Ошибка: после " << arg << " нужно указать номер порта\n";
                return 1;
            }
            i = i + 1;
            port = std::stoi(argv[i]);
        } else {
            std::cerr << "Неизвестный параметр: " << arg << "\n";
            print_help(argv[0]);
            return 1;
        }
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) {
        std::cerr << "Неверный IP-адрес: " << ip << "\n";
        close(sock);
        return 1;
    }

    if (connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }

    std::cout << "Введите строку: ";
    std::string msg;
    if (!std::getline(std::cin, msg)) {
        close(sock);
        return 0;
    }
    msg = msg + "\n";

    ssize_t sent = send(sock, msg.c_str(), msg.size(), 0);
    if (sent < 0) {
        perror("send");
        close(sock);
        return 1;
    }

    char buf[1024];
    ssize_t n = recv(sock, buf, sizeof(buf) - 1, 0);
    if (n < 0) {
        perror("recv");
        close(sock);
        return 1;
    }

    buf[n] = '\0';
    std::cout << "Эхо-ответ: " << buf;

    close(sock);
    return 0;
}
