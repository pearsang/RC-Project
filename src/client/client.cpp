#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <iostream>
#define PORT "58011"

int fd, errcode, newfd;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
char buffer[128];

int main() {
    fd = socket(AF_INET, SOCK_DGRAM, 0); // TCP socket, AF_INET = IPv4 abd SOCK_STREAM = TCP
    if (fd == -1) exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    errcode = getaddrinfo("tejo.tecnico.ulisboa.pt", PORT, &hints, &res);
    if (errcode != 0) exit(1);

    /* n = sendto(fd, "LST\n", 4, 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) exit(1);

    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, 128, 0, (struct sockaddr *) &addr, &addrlen);
    if (n == -1) exit(1);

    write(1, "received: ", 10);
    write(1, buffer, n); */

    while (!std::cin.eof()) {
        std::cin.getline(buffer, 128);
        buffer[strlen(buffer)] = '\n';
        n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
        if (n == -1) exit(1);

        addrlen = sizeof(addr);
        n = recvfrom(fd, buffer, 128, 0, (struct sockaddr *) &addr, &addrlen);
        if (n == -1) exit(1);

        write(1, "received: ", 10);
        write(1, buffer, n);
    }

    freeaddrinfo(res);
    close(fd);

    return 0;
}