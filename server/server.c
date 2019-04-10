// Cwk2: server.c - multi-threaded server using readn() and writen()"

/* 
  Author: Jules Maurice Mulisa
  Student ID: S1719024
  Version: 0.1 
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include "rdwrn.h"

//StudentID  and Name
#define my_ID "S1719024"
#define my_Name "JULES MAURICE MULISA"

// thread function
void *client_handler(void *);

typedef struct {
    int id_number;
    int age;
    float salary;
} employee;

void get_and_send_employee(int, employee *);
void send_NameID(int);
void send_Random_Numbers(int);

// you shouldn't need to change main() in the server except the port number
int main(void){
    int listenfd = 0, connfd = 0;

    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;
    socklen_t socksize = sizeof(struct sockaddr_in);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(50001);

    bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    if (listen(listenfd, 10) == -1) {
    perror("Failed to listen");
    exit(EXIT_FAILURE);
    }
    // end socket setup

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    while (1) {
    printf("Waiting for a client to connect...\n");
    connfd = accept(listenfd, (struct sockaddr *) &client_addr, &socksize);
    printf("Connection accepted...\n");

    pthread_t sniffer_thread;
        // third parameter is a pointer to the thread function, fourth is its actual parameter
    if (pthread_create(&sniffer_thread, NULL, client_handler,
       (void *) &connfd) < 0) {
      perror("could not create thread");
      exit(EXIT_FAILURE);
    }
    //Now join the thread , so that we dont terminate before the thread
    //pthread_join( sniffer_thread , NULL);
    printf("Handler assigned\n");
    }

    // never reached...
    // ** should include a signal handler to clean up
    exit(EXIT_SUCCESS);
} // end main()

// thread function - one instance of each for each connected client
// this is where the do-while loop will go
void *client_handler(void *socket_desc){
    //Get the socket descriptor
    int connfd = *(int *) socket_desc;

    //sending the name and ID
    send_NameID(connfd);

    //sending random numbers to the client
    void send_Random_Numbers(int socket);

    //sending the employee information
    employee *employee1;
    employee1 = (employee *) malloc(sizeof(employee));

    int i;
    for (i = 0; i < 5; i++) {
      printf("(Counter: %d)\n", i);
      get_and_send_employee(connfd, employee1);
      printf("\n");
    }

    free(employee1);

    shutdown(connfd, SHUT_RDWR);
    close(connfd);

    printf("Thread %lu exiting\n", (unsigned long) pthread_self());

    // always clean up sockets gracefully
    shutdown(connfd, SHUT_RDWR);
    close(connfd);

    return 0;
}  // end client_handler()

// Sending name concantinated with Student ID
void send_NameID(int socket){
    // concatenating name and ID
    char my_Info_String[]="";

    //set the memory of the string
    //memset(my_Info_String, '\0', sizeof(my_Info_String));

    // copy the starting words to the info string
    strcpy(my_Info_String, "Student Name is: ");
    //concatenate student name and ID
    strcat(my_Info_String, my_Name);
    strcat(my_Info_String, "\nStudentID is: ");
    strcat(my_Info_String, my_ID);

    size_t n = strlen(my_Info_String) + 1;
    writen(socket, (unsigned char *) &n, sizeof(size_t)); 
    writen(socket, (unsigned char *) my_Info_String, n);    
} // end send_NameID()


// Sending name concantinated with Student ID
void send_Random_Numbers(int socket){
    
    // the string to be sent to the client
    char five_radoms[256];
    
    //declaring variables
    int counter, random_Number;

    strcpy(five_radoms, "\nBelow are five Random Numbers [between 0 and 1000]\n");
    
    // time_t to help in srand  initialization
    time_t t;
    random_Number = 5;
     //random number generator intializing
    srand((unsigned) time(&t));
   
    for (counter = 1; counter <= 5; counter++) {
      random_Number = rand() % 1000 + 1;
      
      char string_build[] = "\nRandom Number ";
      char random_to_str[5];  // to store a stringfied int of a randmon nmber
      char count_to_str[2];   // to store a stringfied int of a counter nmber
      
      sprintf(random_to_str,"%d", random_Number);  // convert the random nmber to a string
      sprintf(count_to_str,"%d", counter);         //convert the counter nmber to a string
      
      strcat(string_build,  count_to_str);
      strcat(string_build, " --> ");

      strcat(string_build, random_to_str);  // add the random number to str line
      strcat(five_radoms, string_build);    // add the new line to the general string to sent to client
      
    }

    // the string to be sent to the client is five_randoms
    printf("%s\n",five_radoms);
    

    size_t n = strlen(five_radoms) + 1;
    writen(socket, (unsigned char *) &n, sizeof(size_t)); 
    writen(socket, (unsigned char *) five_radoms, n);
   
} // end send_Random_Numbers()


// as before...
void get_and_send_employee(int socket, employee * e){
    size_t payload_length;

    size_t n =
    readn(socket, (unsigned char *) &payload_length, sizeof(size_t));
    printf("payload_length is: %zu (%zu bytes)\n", payload_length, n);
    n = readn(socket, (unsigned char *) e, payload_length);

    printf("Age is %d\n", e->age);
    printf("id is %d\n", e->id_number);
    printf("Salary is %6.2f\n", e->salary);
    printf("(%zu bytes)\n", n);

    // make arbitrary changes to the struct & then send it back
    e->age++;
    e->salary += 1.0;

    writen(socket, (unsigned char *) &payload_length, sizeof(size_t));
    writen(socket, (unsigned char *) e, payload_length);
}  // end get_and_send_employee()
