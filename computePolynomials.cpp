/*-------------------------------------------------------------------------*
 *---									---*
 *---		computePolynomial.cpp					---*
 *---									---*
 *---	    This file defines a function that creates a child process,	---*
 *---	sends the contents of '*inputStackPtr' to the child process  	---*
 *---	via a pipe, reads the results that the child gives back using	---*
 *---	another pipe, and put those results into '*outputStackPtr'.	---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---	Version 1.0		2021 May 27		Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/

#include	"PolynomialServerHeaders.h"
#include	<sys/wait.h>

void		computePolynomials
				(FloatStack*	outputStackPtr,
				 FloatStack*	inputStackPtr
				)
{
  int		toChild[2];
  int		fromChild[2];
  pid_t		polynomialPid;
  char		buffer[BUFFER_LEN];
  float		fOfX;
  float		x;  
  
  //  YOUR CODE HERE
  pipe(toChild);
  pipe(fromChild);
  polynomialPid=fork();
  if  (polynomialPid == 0)
  {
    //  YOUR CODE HERE
    close(toChild[1]);
    close(fromChild[0]);
    dup2(toChild[0],STDIN_FILENO);
    dup2(fromChild[1],STDOUT_FILENO);
    int r=execl("./polynomialCcomputer","./polynomialCcomputer",NULL);
    if(r==-1){
      //printf("failed\n");
      exit(EXIT_FAILURE);
    }
  }

  close(toChild[0]);
  close(fromChild[1]);
  //  YOUR CODE HERE
  while  ( !inputStackPtr->isEmpty() )
  {
    //  YOUR CODE HERE
    char str[200];
    float f=inputStackPtr->pop();
    
    sprintf(str,"%g\n",f);
   // printf("str %s\n",str);
    write(toChild[1],str,strlen(str));
   // write(toChild[1],str,sizeof(str));

  }
  write(toChild[1],END_OF_INPUT_MARKER,sizeof(END_OF_INPUT_MARKER));
  //  YOUR CODE HERE
  FILE*	inputPtr	= fdopen(fromChild[0],"r");

  while  ( fgets(buffer,BUFFER_LEN,inputPtr) != NULL )
  {
    //  YOUR CODE HERE
    float x, fofx;
    sscanf(buffer,"%g %f",&x,&fofx);
    //printf("buf %s\n",buffer);
    outputStackPtr->push(fofx);
    outputStackPtr->push(x);

  }
  close(toChild[1]);
  close(fromChild[0]);
  wait(NULL);
  //  YOUR CODE HERE
}
