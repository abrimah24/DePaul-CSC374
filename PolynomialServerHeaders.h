/*-------------------------------------------------------------------------*
 *---									---*
 *---		PolynomialServerHeaders.h				---*
 *---									---*
 *---	    This file declares constants common to both the client and	---*
 *---	server of an application where the client asks the user for a	---*
 *---	list of floating point numbers, and the server computes a	---*
 *---	polynomial function for each number in reverse order.		---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---	Version 1.0					Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/


/*---		Header file inclusion					---*/

#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<unistd.h>	// read(), write()
#include	<sys/socket.h>	// For socket()
#include	<netinet/in.h>	// For sockaddr_in and htons()
#include	<netdb.h>	// For getaddrinfo()
#include	<errno.h>	// For errno var
#include	<sys/stat.h>	// For open(), read(),write(), stat()
#include	<fcntl.h>	// and close()


/*---		Definition of constants:				---*/

#define		BUFFER_LEN		256
#define		DEFAULT_HOSTNAME	"localhost"
#define		PROGRAM_NAME		"polynomialComputer"
const int	ERROR_SUM		= -1;
#define		END_OF_INPUT_MARKER	"end\n"


/*---		Declaration of classes:				---*/

struct	FloatNode
{
  float		number_;
  FloatNode* nextPtr_;

  FloatNode	()
  		{
		  number_	= 0;
		  nextPtr_	= NULL;
		}

  FloatNode	(float	newNumber,FloatNode*	newNextPtr	= NULL)
  		{
		  number_	= newNumber;
		  nextPtr_	= newNextPtr;
		}

};

class	FloatStack
{
  FloatNode*	topPtr_;

public :

  FloatStack			() :
				topPtr_(NULL)
				{ }

  ~FloatStack			();

  bool		isEmpty		()
				const
				{ return(topPtr_ == NULL); }

  void		push		(float	number
				);

  float		pop		();

};
