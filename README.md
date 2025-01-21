# TP2_SYNTHESE_INFO


Ce code implémente un client TFTP (Trivial File Transfer Protocol) simplifié pour demander un fichier à un serveur. Voici une explication des différentes parties :

1. Inclusion des bibliothèques nécessaires
#include <netdb.h>, #include <sys/socket.h>, etc. : Utilisées pour gérer les sockets réseau, les connexions, et la résolution d'adresses.
#include <stdio.h> et #include <stdlib.h> : Pour les entrées/sorties et la gestion de mémoire.
#include <string.h> : Pour manipuler des chaînes de caractères.
2. Définition d'une taille de buffer
c
Copier
Modifier
#define BUF_SIZE 512
Définit la taille maximale des messages échangés.
3. Fonction createSocket
Crée un socket UDP basé sur les paramètres d'adressage donnés (famille, type, protocole).
Si la création échoue, la fonction retourne -1 avec un message d'erreur.
4. Fonction sendRRQ
Envoie une requête RRQ (Read Request) au serveur TFTP pour demander un fichier :
Opcode : Les deux premiers octets identifient le type de message (0x01 pour RRQ).
Filename : Le nom du fichier demandé.
Mode : Spécifie le mode de transfert (par exemple, "octet" pour un transfert binaire).
Le message est construit dans un buffer
