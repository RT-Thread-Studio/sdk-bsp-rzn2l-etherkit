/*
 * samplefeature.h
 *
 *  Created on:
 *      Author:
 */

#ifndef ETHERCAT_RENESAS_SAMPLEFEATURE_H_
#define ETHERCAT_RENESAS_SAMPLEFEATURE_H_

#define CIA402_DRIVE 1 // IO+CiA402+FoE
#define SEMI_DEVICE  2 // IO+       FoE+CDP
#define CONFORMANCE  3 // IO+CiA402+FoE+CDP

/*** Select a feature **********************/
#define ETHERCAT_FEATURE CIA402_DRIVE

#endif /* ETHERCAT_RENESAS_SAMPLEFEATURE_H_ */
