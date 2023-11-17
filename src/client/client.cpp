#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#define PORT "58001"

int fd, errcode, newfd;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints, *res;
struct sockaddr_in addr;
char buffer[128];

int main() {
    fd = socket(AF_INET, SOCK_STREAM, 0); // TCP socket, AF_INET = IPv4 abd SOCK_STREAM = TCP
    if (fd == -1) exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    errcode = getaddrinfo("tejo.tecnico.ulisboa.pt", PORT, &hints, &res);
    if (errcode != 0) exit(1);

    errcode = connect(fd, res->ai_addr, res->ai_addrlen);
    if (errcode == -1) exit(1);

    n = write(fd, "Hdasdello!\n", 7);
    if (n == -1) exit(1);

    n = read(fd, buffer, 128);
    if (n == -1) exit(1);

    write(1, "echo: ", 6);
    write(1, buffer, n);

    freeaddrinfo(res);
    close(fd);

    return 0;
}