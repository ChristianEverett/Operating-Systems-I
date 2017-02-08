/*
 * dounts.h
 *
 *  Created on: Sep 16, 2016
 *      Author: Christian Everett
 */

#ifndef DONUTS_H_
#define DONUTS_H_

#define		SHAREDMEMORY	"ringBufferMem"
#define		PRODSEM			"prod"
#define		CONSEM			"con"
#define		OUTPTRSEM		"outptr"
#define		NUMFLAVORS	 	4
#define		NUMSLOTS       	50
#define		NUMSEMIDS	 	3
#define		PROD		 	0
#define		CONSUMER	 	1
#define		OUTPTR		 	2

struct donut_ring
{
	//four ring buffers
	int flavor[NUMFLAVORS][NUMSLOTS];
	//Pointer to next item in each buffer for consumers
	int outptr[NUMFLAVORS];
};

#endif /* DONUTS_H_ */
