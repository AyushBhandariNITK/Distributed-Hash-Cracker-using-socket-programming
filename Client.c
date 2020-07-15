#include <stdio.h> 
#include <stdbool.h>
#include <string.h>     //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>    //close 
#include <arpa/inet.h> //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <crypt.h>    // hashing function
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
  
void strev(char *str)                   // Utility function to reverse a string 
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
 
 
 //packet request function and checking hash between range
 
 void requestPacket(char* sendBuffer, char* requestMsg, char * receiveBuffer, int serverfd, char* hash)
 {
    sprintf(sendBuffer, requestMsg);                           
    write(serverfd, sendBuffer, sizeof(sendBuffer));                //sending request msg
    read(serverfd, receiveBuffer, 80);                              //reading range
    int range = atoi(receiveBuffer);                                //integer range  
    clientRange(range);                                             //printing range in base 36
    long int lowerLimit = 1000000 * range;
    long int upperLimit = lowerLimit + 999999;
    for(long int i = lowerLimit; i <= upperLimit; ++i)
    {
        char temp[100];
        memset(temp, 0, 100);
        //printf("%s %d\n", fromDeci(temp, 36, i), strcmp(hash, crypt(fromDeci(temp, 36, i),"aa")));
        
        if(compare1(hash, crypt(fromDeci(temp, 36, i),"aa")))
        {
            char pass[100] = "found :";                           //password with found
            printf("Hash found: %s \n",fromDeci(temp, 36, i));  
            strcat(pass, fromDeci(temp, 36, i));
            write(serverfd, pass, sizeof(pass));            //send over flag
            
           // sleep(0.1);
            //write(serverfd, fromDeci(temp, 36, i), sizeof(fromDeci(temp, 36, i)));
            exit(0);
        }
    }
    
 }
 




int main(int argc, char **argv)
{
    int serverfd;
    char sendBuffer[80], receiveBuffer[80], hash[80];
    struct sockaddr_in serveraddr;  
    bzero(&serveraddr, sizeof(serveraddr));
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    inet_pton(AF_INET, argv[1], &serveraddr.sin_addr);
    connect(serverfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    int a = read(serverfd, hash, 80);    
    printf("%s\n",hash);
    hash[a] ='\0';
  
    
    
 
    for(int i =0; i<6; i++) {
        requestPacket(sendBuffer, "firstPacket", receiveBuffer, serverfd, hash);
        
    }
    
    
         
    
}







// while(1)
    // {
    //     read(serverfd, receiveBuffer, 80);
        
    //     printf("%s\n",receiveBuffer);
    //  }     
    
    
    
    
       // sprintf(sendBuffer, "firstPacket");
    // write(serverfd, sendBuffer, sizeof(sendBuffer));
    // read(serverfd, receiveBuffer, 80);
    // printf("%s\n",receiveBuffer);
    
    
    // sprintf(sendBuffer, "secondPacket");
    // write(serverfd, sendBuffer, sizeof(sendBuffer));
    // read(serverfd, receiveBuffer, 80);
    // printf("%s\n",receiveBuffer);
    
    
    // sprintf(sendBuffer, "thirdPacket");
    // write(serverfd, sendBuffer, sizeof(sendBuffer));
    // read(serverfd, receiveBuffer, 80);
    // printf("%s\n",receiveBuffer);
