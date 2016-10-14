/*
 * connect_server.c
 *
 *  Created on: Oct 13, 2016
 *      Author: angel
 */
#include "../etcp.h"

int main(int argc, char **argv) {
	SOCKET s;
	SOCKET s1;
	char buf[128];

	INIT();

	s = tcp_server(NULL, argv[1]);
	s1 = accept(s, NULL, NULL);
	if(!isvalidsock(s1)){
		error(1, errno, "accept failed");
	}

	for(;;){
		int rc = recv(s1, buf, sizeof(buf), 0);
		if(rc == 0){//EOF
			error(0, 0, "client close\n");
			break;
		}
	}

//	sleep(5);
	close(s1);
	close(s);
	exit(0);
}

