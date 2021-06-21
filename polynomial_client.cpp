/*-------------------------------------------------------------------------*
 *---									---*
 *---		polynomial_client.c					---*
 *---									---*
 *---	    This file defines a C program that asks the user for a	---*
 *---	list of polynomials, it sends this list to a server, and then 	---*
 *---	receives the values of a polynomial function with those numbers	---*
 *---	in reverse order.						---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---	Version 1.0					Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/

/*---		Header file inclusion					---*/

#include	"PolynomialServerHeaders.h"
#include	<unistd.h>	// open, close, read, write
#include	<sys/select.h> // select()



//  PURPOSE:  To ask the user for the name and the port of the server.  The
//	server name is returned in 'url' up to length 'urlLen'.  The port
//	number is returned in '*portPtr'.  No return value.
void	obtainUrlAndPort	(int		urlLen,
				 char*		url,
				 int*		portPtr
				)
{
  //  I.  Application validity check:
  if  ( (url == NULL)  ||  (portPtr == NULL) )
  {
    fprintf(stderr,"BUG: NULL ptr to obtainUrlAndPort()\n");
    exit(EXIT_FAILURE);
  }

  if   (urlLen <= 1)
  {
    fprintf(stderr,"BUG: Bad string length to obtainUrlAndPort()\n");
    exit(EXIT_FAILURE);
  }

  //  II.  Get server name and port number:
  //  II.A.  Get server name:
  printf("Machine name [%s]? ",DEFAULT_HOSTNAME);
  fgets(url,urlLen,stdin);

  char*	cPtr	= strchr(url,'\n');

  if  (cPtr != NULL)
    *cPtr = '\0';

  if  (url[0] == '\0')
    strncpy(url,DEFAULT_HOSTNAME,urlLen);

  //  II.B.  Get port numbe:
  char	buffer[BUFFER_LEN];

  printf("Port number? ");
  fgets(buffer,BUFFER_LEN,stdin);

  *portPtr = strtol(buffer,NULL,10);

  //  III.  Finished:
}


//  PURPOSE:  To attempt to connect to the server named 'url' at port 'port'.
//	Returns file-descriptor of socket if successful, or '-1' otherwise.
int	attemptToConnectToServer	(const char*	url,
					 int		port
					)
{
  //  I.  Application validity check:
  if  (url == NULL)
  {
    fprintf(stderr,"BUG: NULL ptr to attemptToConnectToServer()\n");
    exit(EXIT_FAILURE);
  }


  //  II.  Attempt to connect to server:
  //  II.A.  Create a socket:
  int socketDescriptor = socket(AF_INET, // AF_INET domain
				SOCK_STREAM, // Reliable TCP
				0);

  //  II.B.  Ask DNS about 'url':
  struct addrinfo* hostPtr;
  int status = getaddrinfo(url,NULL,NULL,&hostPtr);

  if (status != 0)
  {
    fprintf(stderr,"%s\n",gai_strerror(status));
    return(-1);
  }

  //  II.C.  Attempt to connect to server:
  struct sockaddr_in server;
  // Clear server datastruct
  memset(&server, 0, sizeof(server));

  // Use TCP/IP
  server.sin_family = AF_INET;

  // Tell port # in proper network byte order
  server.sin_port = htons(port);

  // Copy connectivity info from info on server ("hostPtr")
  server.sin_addr.s_addr =
	((struct sockaddr_in*)hostPtr->ai_addr)->sin_addr.s_addr;

  status = connect(socketDescriptor,(struct sockaddr*)&server,sizeof(server));

  if  (status < 0)
  {
    fprintf(stderr,"Could not connect %s:%d\n",url,port);
    return(-1);
  }

  //  III.  Finished:
  return(socketDescriptor);
}


//  PURPOSE:  To do the work of the application.  Gets letter from user, sends
//	it to server over file-descriptor 'fd', and either prints text of
//	returned error code, or prints returned file.  No return value.
void	communicateWithServer	(int	fd
				)
{
  //  I.  Application validity check:

  //  II.  Do work of application:
  //  II.A.  Get integers from user and send them to server:
  float		x;

  do
  {
    char	buffer[BUFFER_LEN+1];

    printf("Please the next integer, or 0.0 to quit: ");
    fgets(buffer,BUFFER_LEN,stdin);

    x			= strtod(buffer,NULL);
    int		toSend	= htonl(*(int*)&x);
    write(fd,&toSend,sizeof(toSend));
  }
  while  (x != 0.0);

  //  II.B.  Get response from server:
  do
  {
    uint32_t	toReceive;

    read(fd,&toReceive,sizeof(toReceive));
    toReceive		= ntohl(toReceive);
    x			= *(float*)&toReceive;

    read(fd,&toReceive,sizeof(toReceive));
    toReceive		= ntohl(toReceive);
    float	fOfX	= *(float*)&toReceive;

    printf("f(%g) = %g\n",x,fOfX);
  }
  while  (x != 0.0);

  //  III.  Finished:
}


//  PURPOSE:  To do the work of the client.  Ignores command line parameters.
//	Returns 'EXIT_SUCCESS' to OS on success or 'EXIT_FAILURE' otherwise.
int	main	()
{
  char		url[BUFFER_LEN];
  int		port;
  int		fd;

  obtainUrlAndPort(BUFFER_LEN,url,&port);
  fd	= attemptToConnectToServer(url,port);

  if  (fd < 0)
    exit(EXIT_FAILURE);

  communicateWithServer(fd);
  close(fd);
  return(EXIT_SUCCESS);
}
