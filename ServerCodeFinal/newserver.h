#ifndef NEWSERVER_H
#define NEWSERVER_H
void *connection_handler(void *socket_desc);
void HandleTheIncomingTraffic(char *buffer, int socket_desc);
#endif