/*
 * wifi.h
 *
 * Created: 19/10/2012 11:30:11 πμ
 *  Author: spilios
 */ 


#ifndef WIFI_H_
#define WIFI_H_

#ifdef __cplusplus
extern "C" {
	#endif
	
	
#include "compiler.h"
	

bool enable_wifi(void);
bool disable_wifi(void);
bool reset_wifi(void);

#endif /* WIFI_H_ */