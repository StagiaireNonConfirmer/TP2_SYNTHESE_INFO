#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 512

// Crée un socket et retourne son descripteur
int createSocket(struct addrinfo *rp) {
    int sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (sfd == -1) {
        perror("Could not create socket");
        return -1;
    }
    return sfd;
}

// Fonction pour envoyer une requête RRQ
void sendRRQ(int sfd, const char *filename, const char *mode) {
    char buffer[BUF_SIZE];
    size_t len;

    // Construction de la requête RRQ
    buffer[0] = 0x00; // Opcode MSB
    buffer[1] = 0x01; // Opcode LSB (1 = RRQ)
    strcpy(buffer + 2, filename);
    strcpy(buffer + 2 + strlen(filename) + 1, mode); // Mode suit après le NULL de filename

    len = 2 + strlen(filename) + 1 + strlen(mode) + 1;

    // Envoi de la requête RRQ
    if (send(sfd, buffer, len, 0) == -1) {
        perror("Error sending RRQ");
        close(sfd);
        exit(EXIT_FAILURE);
    }

    printf("RRQ sent: filename='%s', mode='%s'\n", filename, mode);
}

// Fonction pour gérer la réception de données (simplifié pour un seul paquet)
void receiveData(int sfd) {
    char buffer[BUF_SIZE];
    ssize_t len;

    len = recv(sfd, buffer, BUF_SIZE, 0);
    if (len == -1) {
        perror("Error receiving data");
        close(sfd);
        exit(EXIT_FAILURE);
    }

    // Vérifiez si c'est un paquet de données (opcode 0x03)
    if (buffer[0] == 0x00 && buffer[1] == 0x03) {
        printf("Received data packet: %ld bytes\n", len - 4); // Exclure l'entête TFTP
        printf("Data: %s\n", buffer + 4); // Afficher les données
    }

    // Envoi d'un ACK
    char ack[4] = {0x00, 0x04, buffer[2], buffer[3]}; // Opcode 0x04, block ID
    if (send(sfd, ack, sizeof(ack), 0) == -1) {
        perror("Error sending ACK");
        close(sfd);
        exit(EXIT_FAILURE);
    }

    printf("ACK sent for block %d\n", buffer[3]);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server> <port> <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *server = argv[1];
    const char *port = argv[2];
    const char *filename = argv[3];

    struct addrinfo hints, *result, *rp;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;      // IPv4 ou IPv6
    hints.ai_socktype = SOCK_DGRAM;  // Socket UDP
    hints.ai_protocol = IPPROTO_UDP; // Protocole UDP

    int s = getaddrinfo(server, port, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    int sfd = -1;
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = createSocket(rp);
        if (sfd == -1)
            continue;

        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;

        close(sfd);
    }

    if (rp == NULL) {
        fprintf(stderr, "Could not connect to server\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    sendRRQ(sfd, filename, "octet"); // Envoyer une requête en mode "octet"
    receiveData(sfd);                // Recevoir les données

    close(sfd);
    return 0;
}
