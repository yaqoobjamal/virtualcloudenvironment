#include <iostream>
#include <fstream>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <sys/types.h>
#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h> //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h>
#include <string.h>
#include "newserver.h"
#define LENGTH 512
long NodePC = 0;
bool MultiServer = false;
pthread_mutex_t lock;
using namespace std;
void *connection_handler(void *socket_desc);
void *Recieve(void *sock);
int getdir(string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir(dir.c_str())) == NULL)
    {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

void getfilex()
{
    string dir = string("/home/me/Downloads");
    vector<string> files = vector<string>();

    getdir(dir, files);

    for (unsigned int i = 0; i < files.size(); i++)
    {

        cout << files[i] << endl;
    }
    //  return 0;
}
const string currentDateTime()
{
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}
void SendFileNode()
{
    long fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("Socket Creation Failed \n");
        exit(1);
    }
    struct sockaddr_in s_addr;
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(8888);
    inet_aton("192.168.0.102", &s_addr.sin_addr);

    if (connect(fd, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1)
    {
        perror("Connect failed \n");
        exit(1);
    }
    send(fd, "Relaying to the fucking node", 100, 0);
    send(fd, ":sendFile", 10, 0);
}
void HandleTheIncomingTraffic(char *buffer, int socket_desc)
{
    bool Authenticated = false;
    string x;
    long SocketDesc = (long)socket_desc;
    string str1(buffer);

    if (str1 == ":node")
    {
        cout << "Nody Node" << endl;
    }
    else if (str1 == ":sendFile")
    {
    }
    else if (str1 == ":close")
    {
        cout << "Client Disconnected" << endl;
    }
    else if (str1 == ":retr")
    {
        string dir = string("/home/usman/Desktop/Directory");
        vector<string> files = vector<string>();

        getdir(dir, files);

        for (unsigned int i = 0; i < files.size(); i++)
        {
            char temp[100];
            strcpy(temp, files[i].c_str());
            send(socket_desc, temp, sizeof(temp), 0);
            // sleep(1);
            cout << files[i] << endl;
        }
        char filebuf[100];
        recv(socket_desc, filebuf, sizeof(filebuf), 0);
        // sleep(1);
        cout << "FileBuffer: " << filebuf << endl;
    }
    else if (str1 == ":file")
    {
        printf("Reading Picture Size\n");
        int size;
        read(socket_desc, &size, sizeof(int));

        //Read Picture Byte Array
        printf("Reading Picture Byte Array\n");
        char p_array[size];
        read(socket_desc, p_array, size);

        //Convert it Back into Picture
        printf("Converting Byte Array to Picture\n");
        FILE *image;
        image = fopen("c1.png", "w");
        fwrite(p_array, 1, sizeof(p_array), image);
        fclose(image);
    }
    else if (str1 == ":auth.umer")
    {
    }
}
bool Login(char *buffer, int socket_desc)
{
    //Converting into C++ String
    string PassCode(buffer);

    if (PassCode == "Login")
        return true;
    else
        return false;
}
int send_image(int socket, char buffer[])
{
    cout << buffer << endl;
    FILE *picture;
    int size, read_size, stat, packet_index;
    char send_buffer[10240], read_buffer[256];
    packet_index = 1;
string filefs = "/home/usman/Desktop/";
    filefs = filefs + buffer;
    picture = fopen(filefs.c_str(), "rb");
    printf("Getting Picture Size\n");

    if (picture == NULL)
    {
        printf("Error Opening Image File");
    }

    fseek(picture, 0, SEEK_END);
    size = ftell(picture);
    fseek(picture, 0, SEEK_SET);
    printf("Total Picture size: %i\n", size);

    //Send Picture Size
    printf("Sending Picture Size\n");
    write(socket, (void *)&size, sizeof(int));

    //Send Picture as Byte Array
    printf("Sending Picture as Byte Array\n");

    do
    { //Read while we get errors that are due to signals.
        stat = read(socket, &read_buffer, 255);
        printf("Bytes read: %i\n", stat);
    } while (stat < 0);

    printf("Received data in socket\n");
    //    printf("Socket data: %c\n", read_buffer);

    while (!feof(picture))
    {
        //while(packet_index = 1){
        //Read from the file into our send buffer
        read_size = fread(send_buffer, 1, sizeof(send_buffer) - 1, picture);
        //Send data through our socket
        do
        {
            stat = write(socket, send_buffer, read_size);
        } while (stat < 0);
        printf("Packet Number: %i\n", packet_index);
        printf("Packet Size Sent: %i\n", read_size);
        printf(" \n");
        printf(" \n");
        packet_index++;
        //Zero out our send buffer
        bzero(send_buffer, sizeof(send_buffer));
    }
}

int receive_image(long socket, char buffy[])
{ // Start function

    int buffersize = 0, recv_size = 0, size = 0, read_size, write_size, packet_index = 1, stat;

    char imagearray[10241], verify = '1';
    FILE *image;

    //Find the size of the image
    do
    {
        stat = read(socket, &size, sizeof(int));
    } while (stat < 0);

    printf("Packet received.\n");
    printf("Packet size: %i\n", stat);
    printf("Image size: %i\n", size);
    printf(" \n");

    char buffer[] = "Got it";

    //Send our verification signal
    do
    {
        stat = write(socket, &buffer, sizeof(int));
    } while (stat < 0);

    printf("Reply sent\n");
    printf(" \n");
    string filefs = "/home/usman/Desktop/";
    filefs = filefs + buffy;
    image = fopen((filefs.c_str()), "wb");

    if (image == NULL)
    {
        printf("Error has occurred. Image file could not be opened\n");
        return -1;
    }

    //Loop while we have not received the entire file yet

    int need_exit = 0;
    struct timeval timeout = {10000, 0};

    fd_set fds;
    int buffer_fd, buffer_out;
    cout << size << "-----" << endl;
    while (recv_size < size)
    {
        //while(packet_index < 2){

        FD_ZERO(&fds);
        FD_SET(socket, &fds);

        buffer_fd = select(FD_SETSIZE, &fds, NULL, NULL, &timeout);

        if (buffer_fd < 0)
            printf("error: bad file descriptor set.\n");

        if (buffer_fd == 0)
            printf("error: buffer read timeout expired.\n");

        if (buffer_fd > 0)
        {
            do
            {
                read_size = read(socket, imagearray, 10241);
            } while (read_size < 0);

            printf("Packet number received: %i\n", packet_index);
            printf("Packet size: %i\n", read_size);

            //Write the currently read data into our image file
            write_size = fwrite(imagearray, 1, read_size, image);
            printf("Written image size: %i\n", write_size);

            if (read_size != write_size)
            {
                printf("error in read write\n");
            }

            //Increment the total number of bytes read
            recv_size += read_size;
            packet_index++;
            printf("Total received image size: %i\n", recv_size);
            printf(" \n");
            printf(" \n");
        }
    }

    fclose(image);
    printf("Image successfully Received!\n");
    return 1;
}
long EstbConn()
{
 long fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1)
        {
            perror("Socket Creation Failed \n");
            exit(1);
        }
        struct sockaddr_in s_addr;
        s_addr.sin_family = AF_INET;
        s_addr.sin_port = htons(8888);
        inet_aton("192.168.0.102", &s_addr.sin_addr);

        if (connect(fd, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1)
        {
            perror("Connect failed \n");
            //exit(1);
            return -1;
        }
        send(fd, ":node", 10, 0);
    return fd;
}
void *connection_handler(void *socket_desc)
{
    // pthread_mutex_lock(&lock);
    int sock = *(int *)socket_desc;
    int read_size;
    char *message, client_message[2000];
    cout << "Hello" << endl;
    send(sock, "Hello From The server", 22, 0);
    // struct sockaddr_in addr;
    // socklen_t addr_size = sizeof(struct sockaddr_in);
    // int res = getpeername(sock, (struct sockaddr *)&addr, &addr_size);
    // void *PortNum = &addr.sin_port;
    // int jj = (int)addr.sin_port;
    // cout << "Port Number: " << j << endl;
    // string s = to_string(j);
    // char arr[s.length() + 1];
    // strcpy(arr, s.c_str());
    // send(sock, arr, 22, 0);
    // sleep(.5);
    // send(sock, "Enter your cloud password", 50, 0);
    // recv(sock, client_message, sizeof(client_message), 0);
    // cout << "The password: " << client_message << endl;
    // bool Authenticated = false;
    int LoginTries = 0;
    if (client_message[0] == ':' && client_message[1] == 'a')
    {
        string str1(client_message);
        // cout<<"Android Connected"<<endl;
        size_t pos;
        string delim = ",";
        pos = str1.find(delim);
        string androidmessage = str1.substr(pos + 1, 500);
        cout << "Android Client established" << endl;
        cout << androidmessage << endl;

        string empspace = " ";
        string newline = " \n";
        string timenow = currentDateTime();
        timenow = timenow + empspace;
        androidmessage = androidmessage + newline;
        androidmessage = timenow + androidmessage;
        ofstream out("/home/usman/Desktop/Directory/Log.txt", ios::app);
        out << androidmessage;

        cout << "Sending file to the Node" << endl;

        long fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1)
        {
            perror("Socket Creation Failed \n");
            exit(1);
        }
        struct sockaddr_in s_addr;
        s_addr.sin_family = AF_INET;
        s_addr.sin_port = htons(8888);
        inet_aton("192.168.0.102", &s_addr.sin_addr);

        if (connect(fd, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1)
        {
            perror("Connect failed \n");
            //exit(1);
        }
        send(fd, ":relaythingy", 10, 0);
    }
    // while (Authenticated != true)
    // {
    //     char buf[100];
    //     int x = recv(sock, buf, sizeof(buf), 0);

    //     Authenticated = Login(buf, 10);
    //     if (LoginTries == 3)
    //         break;
    //     LoginTries++;
    // }
    // if (LoginTries == 3 && !Authenticated)
    // {
    //     cout << "In If" << endl;
    //     close(sock);
    // }
    //  cout << "C1" << endl;
    while (1)
    {
        char buffer[1000];
        int jj = recv(sock, buffer, 1000, 0);
        cout << "Buffer Contents: " << buffer << endl;
        if (jj <= 0)
        {
            close(sock);
            cout << "Socket is closed" << endl;
            // break;
            while(1);
             //exit(1);
        }
            cout << buffer << endl;

            string str1(buffer);
        if (buffer[0] == ':' && buffer[1] == 'a')
        {
            string str1(buffer);
            // cout<<"Android Connected"<<endl;
            size_t pos;
            string delim = ",";
            pos = str1.find(delim);
            string androidmessage = str1.substr(pos + 1, 500);
            cout << "Android Client established" << endl;
            cout << androidmessage << endl;

            string empspace = " ";
            string newline = " \n";
            string timenow = currentDateTime();
            timenow = timenow + empspace;
            androidmessage = androidmessage + newline;
            androidmessage = timenow + androidmessage;
            ofstream out("/home/usman/Desktop/Directory/Log.txt", ios::app);
            out << androidmessage;
        }
        else if (str1 == ":node")
        {
            cout << "Nody Node" << endl;
            // long j=EstbConn();
            // cout<<"Socket Descriptor of Node is: "<< j<<endl;
        }
        else if (str1 == ":download")
        {
            cout << "IN SEND" << endl;
            char xyz[100];
            recv(sock, xyz, sizeof(xyz), 0);
            cout << "xyz:::" << xyz << endl;
            send_image(sock, xyz);
        }
        else if (str1 == ":upload")
        {
            cout << "In upload" << endl;
            char UploadBuffer[100];
            recv(sock, UploadBuffer, sizeof(UploadBuffer), 0);
            cout << UploadBuffer << "::  " << endl;
            receive_image(sock, UploadBuffer);
            long Node=EstbConn();

            send(Node,":upload",8,0);


            // cout<<"Relaying File to the Node Server."<<endl;
            
         //   sleep(5);
        //  cout<<"File recived on the main server uploading it to the node server "<<UploadBuffer<<endl;
         //sleep(2);
        //  for(int i=0;i<100000;i++)
        //  {

        //  }
            cout<<"Upload buffer contents."<<endl;
            sleep(2);
            send (Node, UploadBuffer,sizeof(UploadBuffer),0);
            send_image(Node,UploadBuffer);
        }
        else if (str1 == ":retr")
        {
            string dir = string("/home/usman/Desktop/NetworksUpdated");
            vector<string> files = vector<string>();

            getdir(dir, files);
            string Filexy = to_string(files.size());
            char FileSize[Filexy.length() + 1];
            strcpy(FileSize, Filexy.c_str());
            send(sock, FileSize, sizeof(FileSize), 0);
            cout << FileSize << ": FileLength" << endl;
            for (unsigned int i = 0; i < files.size(); i++)
            {
                char temp[100];
                strcpy(temp, files[i].c_str());
                send(sock, temp, sizeof(temp), 0);
                cout << files[i] << endl;
            }
            char filebuf[100];
            // recv(sock, filebuf, sizeof(filebuf), 0);
        }
        else if (str1 == ":exit")
        {
            cout << "Client requested logout." << endl;
            // close(sock);
        }
        else
        {
            cout << str1 << "/Type some valid commands/" << endl;
        }
    }

    // pthread_mutex_unlock(&lock);

    return 0;
}
