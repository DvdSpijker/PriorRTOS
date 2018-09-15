#ifndef KERNEL_MODULE_H_
#define KERNEL_MODULE_H_

#include "include/OsTypes.h"

/* KM = Kernel Module. */
typedef enum {
	KM_STATE_DISABLED, 
	KM_STATE_UNINITIALIZED, 
	KM_STATE_AVAILABLE, 
	KM_STATE_ERROR,
} KernelModuleState_t;

/* Kmf = Kernel Module Function. */
typedef OsResult_t	(*KmfGeneric_t)(void *context);
typedef U32_t		(*KmfObjectToString_t)(void *obj, char *obj_str);   /* Converts the passed object (obj) to a string (obj_str), the lenght of the string is returned. */
typedef OsResult_t	(*KmfHookError_t)(U32_t err_code);
typedef void 		(*KmfHookTick_t)(void);
typedef void		(*KmfOsFreqChange_t)(uint16_t old_freq, uint16_t new_freq);

struct KernelModule {
	const char name[];
	
	IdGroup_t id_type;
	KernelModuleState_t state;
	
	const struct KernelModule *dependencies[];
	
	const struct LinkedList_t *lists[];
	
	KmfGeneric_t        init;
	KmfGeneric_t		deinit;
	KmfHookError_t      on_error;
	KmfHookTick_t 		on_tick;
	KmfOsFreqChange_t	on_freq_change;
	KmfObjectToString_t object_to_string;
};



#endif
