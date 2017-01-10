/*
_CLASS_(tcb_c) {

_PUB_(void*, func, (int a, int b, int c));

}
*/
//#define _CLASS_(name) 

/* Get self by member and member address */
#define __SELF__(t,m,n) t* thiz = (t*)((int)(&n) - (int)(&(((t*)0)->m)));
