/*
 * Try.h
 *
 * Created: 1-3-2017 11:29:04
 *  Author: Dorus
 */


#ifndef TRY_H_
#define TRY_H_

#include <setjmp.h>

#define TRY do { jmp_buf ex_buf__; switch( setjmp(ex_buf__) ) { case 0: while(1) {
#define CATCH(x) break; case x:
#define FINALLY break; } default: {
#define ETRY break; } } }while(0)
#define THROW(x) longjmp(ex_buf__, x)

#endif /* TRY_H_ */