#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h> //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h>
#include <string.h>
#define LENGTH 512
long NodePC = 0;
bool MultiServer = false;
using namespace std;
void *connection_handler(void *socket_desc);
void *Recieve(void *sock);

string Convert(char *Buffer)
{
    string str1(Buffer);return str1;
}
void HandleTheIncomingTraffic(char *buffer, int socket_desc)
{
    string x;
    long SocketDesc = (long)socket_desc;
    string str1(buffer);
    if (str1 == ":node")
    {
        cout << "Sending File to the Client" << endl;
        send(SocketDesc, "Confirm Node", 30, 0);
        char TrafficBuffer[100];
        recv(SocketDesc, TrafficBuffer, sizeof(TrafficBuffer), 0);
        cout << "Reply:  " << TrafficBuffer << endl;
        string str2(TrafficBuffer);
        if (str2 == "y")
        {
            NodePC = SocketDesc;
            MultiServer = true;
            // send(NodePC, "This is the relaying Process", 50,0);
        }
        else
        {
            cout << "No Nodes Online" << endl;
        }
    }
    else if(str1==":sendFile")
    {
     if (MultiServer==true)
     {
         send(NodePC,"Relay",10,0);
          ofstream filex; string s="Relay";
            filex.open("rip.txt");
            filex<<s;
            filex.close();
            cout<<"File sent"<<endl;
     }


    }
    else if (str1 == ":disp")
    {
        cout << "Displaying Directories to the client" << endl;
        if (NodePC == 0)
        {
            cout << "No Nodes online.." << endl;
        }
        else
        {
            send(NodePC, "This is the relay to the node.", 100, 0);
           
            
        }
    }
    else if (str1 == ":close")
    {
        cout << "Exiting Server" << endl;
    }
}

int main(int argc, char *argv[])
{
    int socket_desc, client_sock, c;
    struct sockaddr_in server, client[5];
    long cli[5];
    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        cout << "Cannot create the socket" << endl;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    //Binding the socket to the port
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    cout << "Bind done" << endl;

    cout << "Waiting for the incoming connections" << endl;
    c = sizeof(struct sockaddr_in);
    listen(socket_desc, 3);
    //Creating a thread variable
    pthread_t threadid[5];
    int i = 0;
    while (i < 5)
    {
        cli[i] = accept(socket_desc, (struct sockaddr *)&client[i], (socklen_t *)&c);
        cout << "Conn Accepted" << endl;
        pthread_create(&threadid[i], NULL, connection_handler, (void *)&cli[i]);
        i++;
    }
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    return 0;
}

void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int *)socket_desc;
    int read_size;
    char *message, client_message[2000];

    send(sock, "Hello From The Node Slave", 22, 0);

    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int res = getpeername(sock, (struct sockaddr *)&addr, &addr_size);
    void *PortNum = &addr.sin_port;
    int j = (int)addr.sin_port;
    cout << "Port Number: " << j << endl;

    string s = to_string(j);
    char arr[s.length() + 1];
    strcpy(arr, s.c_str());
    send(sock, arr, 22, 0);
    while (1)
    {
        char buffer[100];
        int j = recv(sock, buffer, 100, 0);
        cout << buffer << endl;
        HandleTheIncomingTraffic(buffer, sock);
    }
    return 0;
}
void *Recieve(void *sock)
{
    long fd = (long)sock;
    while (1)
    {
        char buffer[100];

        int x = recv(fd, buffer, 100, 0);
        if (x <= 0)
        {
            cout << "Server Failed Exiting" << endl;
            exit(1);
        }
        cout << "Contents are: " << buffer << endl;
    }
}
