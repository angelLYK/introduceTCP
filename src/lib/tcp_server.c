/*
 * tcp_server.c
 *
 *  Created on: Sep 23, 2016
 *      Author: angel
 */

#include "../etcp.h"

SOCKET tcp_server( char *hname, char *sname ){
	struct sockaddr_in local;
	SOCKET s;
	const int on = 1;
	const int enable = 1;
	set_address(hname, sname, &local, "tcp");
	s = socket(AF_INET, SOCK_STREAM, 0);
	if(!isvalidsock(s)){
		error(1, errno, "socket call failed!");
	}
	if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))){
		error(1, errno, "setsockopt failed!");
	}
	/*if(setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (void *)&enable, sizeof(enable))){
		error(1, errno, "setsockopt failed!");
	}*/
	if(bind(s, (struct sockaddr *)&local, sizeof(local))){
		error(1, errno, "bind error");
	}
	if(listen(s, NLISTEN)){
		error(1, errno, "listen failed!");
	}

	return s;

}
