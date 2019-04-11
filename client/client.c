// Cwk2: client.c - message length headers with variable sized payloads
//  also use of readn() and writen() implemented in separate code module

/* 
  Author: Jules Maurice Mulisa
  Student ID: S1719024
  Version: 0.1 
*/

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "rdwrn.h"

#define INPUTSIZ 10

//designing the menu
void displaymenu()
{
    printf("0. Display menu\n");
    printf("1. Display Student Info\n");
    printf("2. Display System Info\n");
    printf("3. Display employee info\n");
    printf("4. Display Files in Directory\n");
    printf("5. Display Random Numbers \n");
    printf("6. Exit\n");
}


typedef struct {
    int id_number;
    int age;
    float salary;
} employee;

// how to send and receive structs
void send_and_get_employee(int socket, employee *e)  {
    size_t payload_length = sizeof(employee);

    // send the original struct
    writen(socket, (unsigned char *) &payload_length, sizeof(size_t));  
    writen(socket, (unsigned char *) e, payload_length);      

    // get back the altered struct
    readn(socket, (unsigned char *) &payload_length, sizeof(size_t));    
    readn(socket, (unsigned char *) e, payload_length);

    // print out details of received & altered struct
    printf("Age is %d\n", e->age);
    printf("id is %d\n", e->id_number);
    printf("Salary is %6.2f\n", e->salary);    
} // end send_and_get_employee()

// how to receive a string
void get_student_name_Id(int socket){
    char hello_string[32];
    size_t k;

    readn(socket, (unsigned char *) &k, sizeof(size_t));  
    readn(socket, (unsigned char *) hello_string, k);

    printf("Student Information\nMy Name: %s\n", hello_string);
    printf("Received: %zu bytes\n\n", k);
} // end get_student_name_Id()


// receiving random numbers
void get_random_numbers(int socket){
    char five_randoms[256];
    size_t k;

    readn(socket, (unsigned char *) &k, sizeof(size_t));  
    readn(socket, (unsigned char *) five_randoms, k);

    printf("%s", five_randoms);
    printf("Received: %zu bytes of random_numbers\n\n", k);
} // end get_random_numbers()


//getting system info
void get_system_info(int socket){
	printf("%s\n","OS: Linux\n Version: 2.3.5654.56\nRelease: 8945/34 2019");
}


// getting file names
void get_files_names(int socket){
	printf("%s\n","Displaying filenames of output dir");
}


int main(void){
    // *** this code down to the next "// ***" does not need to be changed except the port number
    
    char input;
    char name[INPUTSIZ];
    
    int sockfd = 0;
    struct sockaddr_in serv_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      perror("Error - could not create socket");
      exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;

    // IP address and port of server we want to connect to
    serv_addr.sin_port = htons(50001);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // try to connect...
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)  {
      perror("Error - connect failed");
      exit(1);  
    } else
       printf("Connected to server...\n");

    // ***
    // your own application code will go here and replace what is below... 
    // i.e. your menu etc.

	displaymenu();
	
	// example employee data
	employee *employee1;    
	employee1 = (employee *) malloc(sizeof(employee));

	// arbitrary values
	employee1->age = 23;
	employee1->id_number = 3;
	employee1->salary = 13000.21;
	int i;	//	for the loop

		do {
		printf("option> ");
		fgets(name, INPUTSIZ, stdin);	// get the value from input
		name[strcspn(name, "\n")] = 0;
		input = name[0];
		if (strlen(name) > 1)
			input = 'x';	// set invalid if input more 1 char

		switch (input) {
		case '0':
			displaymenu();
			break;
		case '1':
			// get a string from the server
			get_student_name_Id(sockfd);
			break;
		case '2':
			//getting system info
			get_system_info(sockfd);
			break;
		case '3':
			// send and receive a changed struct to/from the server
		
			for (i = 0; i < 5; i++) {
				 printf("(Counter: %d)\n", i);
		   send_and_get_employee(sockfd, employee1);
				 printf("\n");
			}

			// free up the memory taken by employee1
			free(employee1);
			break;
		case '4':
			// getting files in directory
			get_files_names(sockfd);
			break;
		case '5':
			// get random numbers from server
			//get_random_numbers(sockfd);
			break;
			
		case '6':
			printf("Goodbye!\n");
			break;
		default:
			printf("Invalid choice - 0 displays options...!\n");
			break;
		}
		} while (input != '4');


    //socket cleaning 
    close(sockfd);

    // successfully executed
    exit(EXIT_SUCCESS);
} // end main()
