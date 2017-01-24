#ifndef GV_TI_SIMPLELINK_WRAPPER_HPP
#define GV_TI_SIMPLELINK_WRAPPER_HPP

/*
   CAUTION!!!
     A bug in TI's simplelink.h header (at least in version 1.0.1.6) causes an
     extern "C" declaration to be opened once and closed twice within that
     file. Here we work around this problem by using nesting, so we open an
     extern "C" before including "simplelink.h", and then test the specific
     SimpleLink version to check if we need to close it.
   TODO: keep track of future SimpleLink versions
 */
extern "C" {
#include "simplelink.h"
#if (! (SL_MAJOR_VERSION_NUM==1L && SL_MINOR_VERSION_NUM==0L && SL_VERSION_NUM==1L && SL_SUB_VERSION_NUM==6L ))
    } // close extern "C"
#endif

#endif // !GV_TI_SIMPLELINK_WRAPPER_HPP

