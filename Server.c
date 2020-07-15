
#include <stdio.h> 
#include <stdbool.h>
#include <string.h>         //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>         //close 
#include <arpa/inet.h>      //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h>       //FD_SET, FD_ISSET, FD_ZERO macros 
#include <crypt.h>          // hashing function
 
#define TRUE 1 
#define FALSE 0 
#define PORT 8888 
 
// compare function
 
bool compare1(char msg[], char buffer[])
{
    return memcmp( buffer, msg, strlen(msg) ) == 0;
}   


// Functions to convert int number to any base string                   

char reVal(int num) 
{ 
    if (num >= 0 && num <= 9) 
        return (char)(num + '0'); 
    else
        return (char)(num - 10 + 'A'); 
} 

// function to reverse a string 
void strev(char *str)                  
{ 
    int len = strlen(str); 
    int i; 
    for (i = 0; i < len/2; i++) 
    { 
        char temp = str[i]; 
        str[i] = str[len-i-1]; 
        str[len-i-1] = temp; 
    } 
} 
  
char* fromDeci(char res[], int base, long inputNum) 
{ 

    int index = 0;  // Initialize index of result 
  
    // Convert input number is given base by repeatedly 
    // dividing it by base and taking remainder 
    while (inputNum > 0) 
    { 
        res[index++] = reVal(inputNum % base); 
        inputNum /= base; 
    } 
    if(index < 5)
    {
        for(int i=index;i<=4;i++)
        {
            res[index++] = '0';
        }
    }
    res[index] = '\0'; 
  
    // Reverse the result 
    strev(res); 
    
  
    return res; 
}   
  
 //print range function
 
 void clientRange(int range)
 {  
    char res[100], res1[100];
    long int lowerLimit = 1000000 * range;
    long int upperLimit = lowerLimit + 999999;
    printf("Range from: %ld to: %ld \n", lowerLimit, upperLimit);
    printf("Range from: %s to: %s \n", fromDeci(res, 36, lowerLimit), fromDeci(res1, 36, upperLimit));
     
 } 
  
 // generate random 5 letter string
char *rand_string(char *str, int size)
{
    char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for(int i=0; i<size; i++)
    {
        
        char c = charset[rand() % 36];
        str[i] = c;   
    }
    str[size] = '\0';
    
    
    
    return str;
}   
 
 
 //generate crypt from rand_string
 
 char* generatePassword(char *pass)
 {
    return crypt(pass,"aa");
 }
 
  
// main 
   
int main(int argc , char **argv) 
{ 
    
   
    int range = 0;      // This is count for range
    
    
    
    int opt = TRUE; 
    int master_socket , addrlen , new_socket , client_socket[30] , 
        max_clients = 30 , activity, i , valread , sd; 
    int max_sd,copy; 
    struct sockaddr_in address; 
     
     
    //receiving buffer    
    char buffer[1025]; //data buffer of 1K 
    
    //sending buffer
    char bufferSend[1025];
        
        
    
     // random password generated
   
    char message[100] = "BCDES";
    char* hash ;
    hash = generatePassword(message);
    
    printf("Password is created: %s \n",message);
    printf("It's hash is: %s\n",hash);
     
    
    
    
    
    // set of socket descriptors 
    fd_set readfds; 
    
    
    
    
    //initialise all client_socket[] to 0 so not checked 
    for (i = 0; i < max_clients; i++) 
    { 
        client_socket[i] = 0; 
    } 
        
    //create a master socket 
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    
    //set master socket to allow multiple connections , 
    //this is just a good habit, it will work without this 
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
        sizeof(opt)) < 0 ) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    
    //type of socket created 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
        
    //bind the socket to localhost port 8888 
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    printf("Listener on port %d \n", PORT); 
        
    //try to specify maximum of 3 pending connections for the master socket 
    if (listen(master_socket, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
        
    //accept the incoming connection
     
    addrlen = sizeof(address); 
    puts("Waiting for connections ..."); 
        
    while(TRUE) 
    { 
        //clear the socket set 
        FD_ZERO(&readfds); 
    
        //add master socket to set 
        FD_SET(master_socket, &readfds); 
        copy = max_sd = master_socket; 
           
        //add child sockets to set 

        for ( i = 0 ; i < max_clients ; i++) 
        { 
            //socket descriptor 
            sd = client_socket[i]; 
                
            //if valid socket descriptor then add to read list 
            if(sd > 0) 
                FD_SET( sd , &readfds); 
                
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd) 
                max_sd = sd; 
        } 
    
        //wait for an activity on one of the sockets , timeout is NULL , 
        //so wait indefinitely 
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL); 
    
        if ((activity < 0) && (errno!=EINTR)) 
        { 
            printf("select error"); 
        } 
            
        //If something happened on the master socket , 
        //then its an incoming connection 
        if (FD_ISSET(master_socket, &readfds)) 
        { 
            if ((new_socket = accept(master_socket, 
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
            { 
                perror("accept"); 
                exit(EXIT_FAILURE); 
            } 
            
            //inform user of socket number - used in send and receive commands 
            printf("New connection , socket fd is %d , ip is : %s , port : %d  \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs (address.sin_port)); 
        
            //send new connection greeting message 
            if( send(new_socket, hash, strlen(hash), 0) != strlen(hash) ) 
            { 
                perror("send"); 
            } 
                
            puts("Hash sent successfully");
             
                
            //add new socket to array of sockets 
            for (i = 0; i < max_clients; i++) 
            { 
                //if position is empty 
                if( client_socket[i] == 0 ) 
                { 
                    client_socket[i] = new_socket; 
                    printf("Adding to list of sockets as %d\n" , i); 
                        
                    break; 
                } 
            } 
        } 
            
        //else its some IO operation on some other socket 
        for (i = 0; i < max_clients; i++) 
        { 
            sd = client_socket[i]; 
                
            if (FD_ISSET( sd , &readfds)) 
            { 
                //Check if it was for closing , and also read the 
               
                  
                valread = read( sd , buffer, 1024);  // read incoming msg 
                
                if (valread == 0)  
                { 
                    //Somebody disconnected , get his details and print 
                    
                    
                    getpeername(sd , (struct sockaddr*)&address , \ 
                        (socklen_t*)&addrlen); 
                    printf("Host disconnected , ip %s , port %d \n" , 
                        inet_ntoa(address.sin_addr) , ntohs(address.sin_port)); 
                        
                    
                    //Close the socket and mark as 0 in list for reuse 
                    
                    
                    close( sd ); 
                    client_socket[i] = 0; 
                } 
                    
                // Sending message to particular client
                else
                { 
                    //set the string terminating NULL byte on the end 
                    //of the data read 
                    buffer[valread] = '\0';
                    
                    
                    
                    if(compare1("over" ,buffer)) 
                    {   
                       //Somebody disconnected , get his details and print 
                    
                    
                        getpeername(sd , (struct sockaddr*)&address , \ 
                            (socklen_t*)&addrlen); 
                        printf("Host disconnected , ip %s , port %d \n" , 
                            inet_ntoa(address.sin_addr) , ntohs(address.sin_port)); 
                        
                    
                        //Close the socket and mark as 0 in list for reuse 
                        
                        
                        close( sd ); 
                        client_socket[i] = 0; 
                    }
                    else if(memcmp( buffer, "found", strlen("found")) == 0)
                    {
                        printf("Password %s ", buffer);
                        // bzero(&buffer, sizeof(buffer));
                        // read(sd, buffer, sizeof(buffer));
                        
                        // printf("%s", buffer);
                        exit(0);
                    }
                    else
                    {
                        sprintf(bufferSend, "%d", range);  // terminated with null character
                        send(sd ,bufferSend , strlen(bufferSend), 0); // sending range to client
                        clientRange(range++);
                        fflush(stdout);
                    }
                    
                   
                   
                   
                } 
            } 
        } 
    } 

                    
        
    return 0; 
} 













                    
                    
                    
                    
                    
                    


              
                    


                    
                                    
 
 
