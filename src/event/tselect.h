/*
 * tselect.h
 *
 *  Created on: Oct 11, 2016
 *      Author: angel
 */

#ifndef TSELECT_H_
#define TSELECT_H_

#include "../etcp.h"

int tselect( int, fd_set *, fd_set *, fd_set *);
unsigned int timeout( tofunc_t, void *, int );
void untimeout( unsigned int );

#endif /* TSELECT_H_ */
