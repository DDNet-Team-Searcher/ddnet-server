#include "base/system.h"
#include <arpa/inet.h>
#include <netinet/in.h>

#include <protos/request.pb.h>

const int PORT = 42069;
const std::string HOST = "127.0.0.1";

namespace ddts {
    void Shutdown(unsigned int happeningId) {
        sockaddr_in serv_addr;
        int sock = socket(AF_INET, SOCK_STREAM, 0);

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port= htons(PORT);

        inet_pton(AF_INET, HOST.data(), &serv_addr.sin_addr);

        connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

        Request request;
        request.set_action(::Action::SHUTDOWN);
        request.set_id(happeningId);

        const size_t size = request.ByteSizeLong();
        char *bytes = new char[size];

        request.SerializeToArray(bytes, size);

        send(sock, bytes, size, 0);

        std::vector<char> gottem(1024);

        //wait for the response owo. works for now
        recv(sock, gottem.data(), gottem.size(), 0);
    }
}
