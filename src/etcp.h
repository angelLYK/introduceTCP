/*
 * etcp.h
 *
 *  Created on: Sep 22, 2016
 *      Author: angel
 */

#ifndef ETCP_H_
#define ETCP_H_


#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include "skel.h"

#define TRUE			1
#define FALSE			0
#define NLISTEN			5		/* max waiting connections */
#define NSMB			5		/* number shared memory bufs */
#define SMBUFSZ			256		/* size of shared memory buf */

extern char *program_name;		/* for error */

#ifdef __SVR4
#define bzero(b,n)	memset( ( b ), 0, ( n ) )
#endif

typedef void ( *tofunc_t )( void * );

void error( int, int, char*, ... );
int readn( SOCKET, char *, size_t );
int readvrec( SOCKET, char *, size_t );
int readline( SOCKET, char *, size_t );
int tcp_server( char *, char * );
int tcp_client( char *, char * );
void set_address( char *, char *, struct sockaddr_in *, char * );

#endif /* ETCP_H_ */
