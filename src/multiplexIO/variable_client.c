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

	fd_set readmask;
	fd_set allreads;

	INIT();
	s = tcp_client(argv[1], argv[2]);
	iov[0].iov_base = (char *) &n;
	iov[0].iov_len = sizeof(n);

	iov[1].iov_base = buf;

	FD_ZERO(&allreads);
	FD_SET(s, &allreads);
	FD_SET(0, &allreads);

	for (;;) {

		readmask = allreads;

		int sl = select(s + 1, &readmask, NULL, NULL, NULL);
		if (sl <= 0) {
			error(1, errno, "bad select return {%d}", sl);
		} else if (FD_ISSET(0, &readmask)) {
			if (fgets(buf, sizeof(buf), stdin) != NULL) {
				iov[1].iov_len = strlen(buf);
				error(0, 0, "buf size: %d\n", iov[1].iov_len);
				n = htonl(iov[1].iov_len);
				if (writev(s, iov, 2) < 0) {
					error(1, errno, "writev failure!");
				}
			} else {
				close(s);
				break;
			}
		} else if (FD_ISSET(s, &readmask)) {
			n = recv(s, buf, sizeof(buf), 0);
			if (n < 0) {
				error(1, errno, "readvrec returned error");
			} else if (n == 0) {
				error(1, 0, "server disconnected\n");
			} else {
				write(1, buf, n);
			}
		}

	}

	EXIT(0);
}

