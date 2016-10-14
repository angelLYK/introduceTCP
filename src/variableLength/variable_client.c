/*
 * vrcv.c
 *
 *  Created on: Sep 29, 2016
 *      Author: angel
 */
#include "../etcp.h"
#include <sys/uio.h>

int main(int argc, char **argv) {
	SOCKET s;
	u_int32_t n;
	char buf[128];
	struct iovec iov[2];

	INIT();
	s = tcp_client(argv[1], argv[2]);
	iov[0].iov_base = (char *)&n;
	iov[0].iov_len = sizeof(n);

	iov[1].iov_base = buf;
	while(fgets(buf, sizeof(buf), stdin) != NULL){
		iov[1].iov_len = strlen(buf);
		error(0, 0, "buf size: %d\n", iov[1].iov_len);
		n = htonl(iov[1].iov_len);
		if(writev(s, iov, 2) < 0){
			error(1, errno, "writev failure!");
		}
	}
	EXIT(0);
}


