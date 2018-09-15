/*
 * IdTypeDef.h
 *
 *  Created on: 16 mei 2018
 *      Author: Dorus
 */

#ifndef ID_TYPE_DEF_H_
#define ID_TYPE_DEF_H_

#include "include/IdType.h"

/* Initialize ID pools. */
void KIdInit(void);

/* Request an ID of a specific type. */
Id_t KIdRequest(IdGroup_t type);

#endif /*ID_TYPE_DEF_H_ */
