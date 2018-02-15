#ifndef KERNEL_MODULE_H_
#define KERNEL_MODULE_H_

#include <Types.h>

/* KM = Kernel Module. */
typdef enum {
	KM_STATE_DISABLED, 
	KM_STATE_UNINITIALIZED, 
	KM_STATE_AVAILABLE, 
	KM_STATE_ERROR,
} KernelModuleState_t

/* Kmf = Kernel Module Function. */
typdef OsResult_t	(*KmfInit)(const void *args);
typdef uint32_t		(*KmfObjectToString)(void *obj, char *obj_str);
typdef OsResult_t	(*KmfHookException)(U32_t exception);
typdef void 		(*KmfHookTick)(void);

struct KernelModule {
	const char name[];
	
	IdType_t id_type;
	KernelModuleState_t state;
	
	const struct KernelModule *dependencies[];
	
	const struct LinkedList_t *lists[];
	
	KmfInit 			init;
	KmfInit 			deinit;
	KmfHookException 	hook_exception;
	KmfHookTick 		hook_tick;
	KmfObjectToString 	object_to_string;
};



#endif