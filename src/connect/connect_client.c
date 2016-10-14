/*
 * connect_client.c
 *
 *  Created on: Oct 13, 2016
 *      Author: angel
 */
#include "../etcp.h"

int main(int argc, char **argv) {
	SOCKET s;
	s = tcp_client(argv[1], argv[2]);
	sleep(15);
	close(s);
	exit(0);
}

