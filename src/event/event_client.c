/*
 * xout3.c
 *
 *  Created on: Oct 11, 2016
 *      Author: angel
 */

#include "../etcp.h"
#include "tselect.h"

#define ACK		0x6
#define MRSZ	128
#define T1		3000
#define T2		5000
#define ACKSZ	(sizeof(u_int32_t) + 1)

typedef struct {
	u_int32_t len;
	u_int32_t cookie;
	char buf[128];
} packet_t;

typedef struct {
	packet_t pkt;
	int id;
} msgrec_t;

static msgrec_t mr[MRSZ];
static SOCKET s;

msgrec_t *getfreerec(void){
	msgrec_t *mp;
	for(mp = mr; mp < mr + MRSZ; mp++){
		if(mp->pkt.len == -1){
			return mp;
		}
	}
	error(1, 0, "getfreerec: out of message records\n");
	return NULL;		/* quiet compiler warnings */
}

msgrec_t *findmsgrec(u_int32_t mid){
	msgrec_t *mp;
	for(mp = mr; mp < mr + MRSZ; mp++){
		if(mp->pkt.len != -1 && mp->pkt.cookie == mid){
			return mp;
		}
	}
	error( 0, 0, "findmsgrec: no message for ACK %d\n", mid );
	return NULL;
}

void freemsgrec(msgrec_t *mp){
	if(mp->pkt.len == -1){
		error( 1, 0, "freemsgrec: message record already free\n" );
	}
	mp->pkt.len = -1;
}

static void drop( msgrec_t *mp ){
	error( 0, 0, "Dropping msg:   %s", mp->pkt.buf );
	freemsgrec( mp );
}

static void lost_ACK(msgrec_t *mp){
	error(0, 0, "Retrying msg:	%s", mp->pkt.buf);
	if(send(s, &mp->pkt, sizeof(u_int32_t) + ntohl(mp->pkt.len), 0) < 0){
		error(1, errno, "lost_ACK: send failure");
	}
	mp->id = timeout((tofunc_t) drop, mp, T2);
}

int main(int argc, char **argv) {
	fd_set allreads;
	fd_set readmask;
	msgrec_t *mp;
	int rc;
	int mid;
	int cnt = 0;
	u_int32_t msgid = 0;
	char ack[ACKSZ];

	INIT();
	s = tcp_client(argv[1], argv[2]);
	FD_ZERO(&allreads);
	FD_SET(s, &allreads);
	FD_SET(0, &allreads);

	for(mp = mr; mp < mr + MRSZ; mp++){
		mp->pkt.len = -1;
	}

	for(;;){
		readmask = allreads;
		rc = tselect(s + 1, &readmask, NULL, NULL);
		if ( rc < 0 )
			error( 1, errno, "tselect failure" );
		if ( rc == 0 )
			error( 1, 0, "tselect returned with no events\n" );

		if(FD_ISSET(s, &readmask)){
			rc = recv(s, ack + cnt, ACKSZ - cnt, 0);
			if(rc == 0){
				error( 1, 0, "server disconnected\n" );
			}else if(rc < 0){
				error( 1, errno, "recv failure" );
			}

			if((cnt += rc) < ACKSZ){
				error(0, 0, "rc->%d\n", rc);
				continue;
			}

			cnt = 0;
			if(ack[0] != ACK){
				error( 0, 0, "warning: illegal ACK msg\n" );
				continue;
			}
			memcpy(&mid, ack + 1, sizeof(u_int32_t));
			error(0, 0, "mid->%d\n", mid);
			mp = findmsgrec(mid);
			if(mp != NULL){
				untimeout(mp->id);
				freemsgrec(mp);
			}
		}

		if(FD_ISSET(0, &readmask)){
			mp = getfreerec();
			rc = read(0, mp->pkt.buf, sizeof(mp->pkt.buf));
			if(rc < 0){
				error( 1, errno, "read failure" );
			}
			mp->pkt.buf[rc] = '\0';
			mp->pkt.cookie = msgid++;
			mp->pkt.len = htonl(sizeof(u_int32_t) + rc);
			if(send(s, &mp->pkt, 2 * sizeof(u_int32_t) + rc, 0)< 0){
				error(1, errno, "send failure");
			}
			mp->id = timeout((tofunc_t) lost_ACK, mp, T1);
		}
	}
}








