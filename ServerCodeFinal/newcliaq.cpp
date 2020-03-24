#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "iostream"
#include "pthread.h"
#include "unistd.h"

using namespace std;

void *recvMsg(void *socket);
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

    image = fopen("home/usman/Desktop/ss.jpg", "wb");

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
int main()
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
    inet_aton("127.0.0.1", &s_addr.sin_addr);

    if (connect(fd, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1)
    {
        perror("Connect failed \n");
        exit(1);
    }

    //         pthread_t thread;
    //         pthread_create(&thread, NULL, recvMsg, (void*) fd);
    //  pthread_t thread2;
    //         pthread_create(&thread2, NULL, recvMsg, (void*) fd);
    char buf2[100];
    recv(fd,buf2,sizeof(buf2),0);
    while(1)
    { char x[100];
        char buffer[100];
        cout<<"Enter Something"<<endl;
        cin>>buffer;
        send(fd,buffer,sizeof(buffer),0);
        cin>>x;
          send(fd,x,sizeof(x),0);
        // char buffer2[100];
        // recv(fd, buffer2,sizeof(buffer2),0);
        // cout<<buffer2<<endl;
        receive_image(fd,x);

        cout<<"Image recieved"<<endl;
    }
    // receive_image(fd);

    return 0;
}

void *recvMsg(void *socket)
{
    long fd = (long)socket;
    char buffer[100];
    while (1)
    {
        recv(fd, buffer, 100, 0);
        cout << "Message from Server : " << buffer << endl;
    }
}