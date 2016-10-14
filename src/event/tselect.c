/*
 * tselect.c
 *
 *  Created on: Oct 11, 2016
 *      Author: angel
 */

#include "../etcp.h"
#include "tselect.h"

#define NTIMERS 25

typedef struct tevent_t tevent_t;
struct tevent_t {
	tevent_t *next;
	struct timeval tv;
	void (* func)(void *);
	void *arg;
	unsigned int id;
};

static tevent_t *active = NULL;
static tevent_t *free_list = NULL;

static tevent_t *allocate_timer(void){
	tevent_t *tp;
	if(free_list == NULL){
		free_list = malloc(NTIMERS * sizeof(tevent_t));
		if(free_list == NULL){//something is wrong
			error(1,0,"could not allocate timers\n");
		}
		for(tp = free_list;
				tp < free_list + NTIMERS -1; tp++){
			tp->next = tp + 1;
		}
		tp->next = NULL;
	}
	tp = free_list;
	free_list = tp->next;
	return tp;
}

unsigned int timeout(void (* func)(void *), void * arg, int ms){
	tevent_t *tp;
	tevent_t *tcur;
	tevent_t **tprev;
	static unsigned int id = 1;
	tp = allocate_timer();
	tp->func = func;
	tp->arg = arg;
	if(gettimeofday(&tp->tv, NULL) < 0){
		error( 1, errno, "timeout: gettimeofday failure" );
	}
	tp->tv.tv_usec += ms * 1000;
	if(tp->tv.tv_usec > 1000000){
		tp->tv.tv_sec += tp->tv.tv_usec / 1000000;
		tp->tv.tv_usec %= 1000000;
	}

	for(tprev = &active, tcur = active;
			tcur && !timercmp(&tp->tv, &tcur->tv, <);
			tprev = &tcur->next,tcur = tcur->next)
	{;}
	*tprev = tp;
	tp->next = tcur;
	tp->id = id++;
	return tp->id;
}

void untimeout(unsigned int id){
	tevent_t **tprev;
	tevent_t *tcur;

	for(tprev = &active, tcur = active;
			tcur && id != tcur->id;
			tprev = &tcur->next, tcur = tcur->next)
	{;}
	if(tcur == NULL){
		error( 0, 0, "untimeout called for non-existent timer (%d)\n", id );
		return;
	}
	*tprev = tcur->next;
	tcur->next = free_list;
	free_list = tcur;
}

int tselect(int maxpl, fd_set *re, fd_set *we, fd_set *ee){
	fd_set rmask;
	fd_set wmask;
	fd_set emask;

	struct timeval now;
	struct timeval tv;
	struct timeval *tvp;

	tevent_t *tp;

	int n;
	if(re){
		rmask = *re;
	}
	if(we){
		wmask = *re;
	}
	if(ee){
		emask = *re;
	}

	for(;;){
		if ( gettimeofday( &now, NULL ) < 0 ){
			error( 1, errno, "tselect: gettimeofday failure" );
		}
		while(active && !timercmp(&now, &active->tv, <)){
			active->func(active->arg);
			tp = active;
			active = active->next;
			tp->next = free_list;
			free_list = tp;
		}
		if(active){
			tv.tv_sec = active->tv.tv_sec - now.tv_sec;
			tv.tv_usec = active->tv.tv_usec - now.tv_usec;
			if ( tv.tv_usec < 0 ){
				tv.tv_usec += 1000000;
				tv.tv_sec--;
			}
			tvp = &tv;
		}else if(re == NULL && we == NULL && ee == NULL){
			return 0;
		}else{
			tvp = NULL;
		}

		n = select(maxpl, re, we, ee, tvp);
		if ( n < 0 )
			return -1;
		if ( n > 0 )
			return n;
		if ( re )
			*re = rmask;
		if ( we )
			*we = wmask;
		if ( ee )
			*ee = emask;

	}
	return 0;
}



