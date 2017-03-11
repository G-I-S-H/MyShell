/*
Author: Dustin Grady
Assignment: HW Assignment 5
Purpose: To create a script which will allow for bash commands to be executed using fork(), file i/o and redirection
Status: Finished/ Tested
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
  char input[50];
  char buff[1000];
  char *token;
  char *tokenContainer[50];
  char *currentValue;
  char *redirectionFile;
  int i = 0;
  int lessThanSymPos = 0; //Position of our < redirection symbol
  int lessRedirDetected = 0;//Redirection flag
  int greaterThanSymPos = 0; //Position of our > redirection symbol
  int greaterRedirDetected = 0; //Redirection flag
  FILE *file;
  //-----------------Date and Time-------------------
  time_t timer;
  struct tm *tm_info;
  char day[3];
  char month[9];
  char year[5];

  time(&timer);
  tm_info = localtime(&timer);
  strftime(month, 9, "%B", tm_info);
  strftime(day, 3, "%d", tm_info);
  strftime(year, 5, "%Y", tm_info);
  //-----------------Date and Time-------------------

  //-------------------File I/O----------------------
  if(argc > 1)//Error handling for file I/O
  {
    file = fopen(argv[1], "r"); 
    if(file == NULL)
    {
      printf("Error opening file.\n");
      exit(0);
    }
  }
  
  while(1)
  {
    if(argc > 1)//If additional argument was passed in, do file I/O
    {
      //Read through file/ tokenize
      currentValue = fgets(buff, sizeof(buff), file);

      if(currentValue == NULL)
      {
	exit(0);
      }

      token = strtok(buff, " \t \n");
      while(token != NULL)
      {

	if(strcmp(token, ">") == 0)//Triggered
	{
	  greaterThanSymPos = i;//Keep track of index where the ">" was found
	  greaterRedirDetected = 1;//Redirection flag triggered
	}

        else if(strcmp(token, "<") == 0)
	{
	  lessThanSymPos = i;
	  lessRedirDetected = 1;
	}
	else
	{
	  tokenContainer[i] = strdup(token);
	  i++;
	}
	token = strtok(NULL, " \t \n");
      }
      tokenContainer[i] = NULL;
    } 
    //-------------------File I/O----------------------

    //----------------Standard Input------------------- 
    else
    {
      printf("msh> ");
      //Run program in loop until EoF or "exit" is entered
      if(fgets(input, sizeof(input), stdin) == NULL || strcmp(input,"exit\n") == 0)
      {
	exit(0);
      }

      input[strlen(input)-1] = '\0';//Set last character in array to null to offset enterkey
      token = strtok(input, " \t");//Tokenize input based on space or tab
    
      while(token != NULL)
      {       
	if(strcmp(token, ">") == 0)//Triggered
	{
	  greaterThanSymPos = i;//Keep track of index where the ">" was found
	  greaterRedirDetected = 1;//Redirection flag triggered
	}
	
	else if(strcmp(token, "<") == 0)
	{
	  lessThanSymPos = i;
	  lessRedirDetected = 1;
	}
	
	else
	{
	  tokenContainer[i] = strdup(token);//Populate tokenContainer with tokens
	  i++;
	}
	token = strtok(NULL, " \t");	  
      }
      tokenContainer[i] = NULL;//Reset container values to null for next sequence
    
      if(i == 0)
      {
	continue;
      }
    }
    //----------------Standard Input-------------------

    //Per assignment specifications, modified help and added today functions
    if(strcmp(tokenContainer[0], "help") == 0)
    {
      printf("Hint: Try typing 'man ' + your command for more information.\n");
    }
    if(strcmp(tokenContainer[0], "today") == 0)
    {
      printf("%s %s, %s\n", month, day, year);
    }
    
    //Added cd functionality
    if(strcmp(tokenContainer[0], "cd") == 0)
    {
      if(tokenContainer[1] == NULL)
      {
	chdir(getenv("HOME"));//Change to home directory if only "cd" is entered
      }
      else
      {
	chdir(tokenContainer[1]);
      }
    }

    int rc = fork();
    if(rc < 0)//Failed fork
    {
      printf("%s Fork failed :( \n");
    }

    else if(rc == 0)//New child process
    {
      //-----------------------Redirection-----------------------
      if(greaterRedirDetected == 1)//If redirection was detected
      {
	file = fopen(tokenContainer[greaterThanSymPos], "w");//Open file for redirection output (create if needed)
	dup2(fileno(file), STDOUT_FILENO);//Call dup2 function to create a copy of descriptor
	fclose(file);
	tokenContainer[greaterThanSymPos] = '\0';//Set filename to null so it doesn't get processed by execvp
      }
      //-----------------------Redirection-----------------------
      execvp(tokenContainer[0], tokenContainer);//Execute commands
      exit(0);
    }    
    else
    {
      int wc = wait(NULL);//Wait for child to finish
    } 
    lessRedirDetected = 0;
    greaterRedirDetected = 0;//Reset flag
    i = 0;//Reset our counter
  }  
  return 0;
}



