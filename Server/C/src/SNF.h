//////////////////////////////////////////////////////////////
/// \brief Main Header File
/// \file /SNF.h
/// This File Calls for all the header files exisitng in this 
/// library, if you plan on using this library as a whole <br>
/// eg not just using the \ref thpool.h "Thread Pool" or
/// \ref hashtable.h "Hash Table", then including <SNF.h>
/// should be better than including each required head one-by-one
///
//////////////////////////////////////////////////////////////
#ifndef __SNF
#define __SNF
#include <SNF/clt.h>
#include <SNF/cmds.h>
#include <SNF/epoll.h>
#include <SNF/hashtable.h>
#include <SNF/lookup3.h>
#include <SNF/network.h>
#include <SNF/opcode.h>
#include <SNF/request.h>
#include <SNF/SNF.h>
#include <SNF/thpool.h>
#include <SNF/utility.h>
#include <SNF/vars.h>
#endif
