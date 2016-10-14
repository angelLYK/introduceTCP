/*
 * readvrec.c
 *
 *  Created on: Sep 22, 2016
 *      Author: angel
 */

#include "../etcp.h"
#include <stdint.h>
int readvrec( SOCKET fd, char *bp, size_t len ){
	u_int32_t reclen;
	int rc;

	rc = readn(fd, (char *) &reclen, sizeof(u_int32_t));
	if(rc != sizeof(u_int32_t)){
		return rc < 0 ?-1 : 0;
	}
	reclen = ntohl(reclen);
	if(reclen > len){//oom, discard message
		set_errno(EMSGSIZE);
		return -1;
	}

	rc = readn(fd, bp, reclen);
	if(rc != reclen){
		return rc < 0 ? -1: 0;
	}

	return rc;

}
