
#pragma once
#ifndef __DEBUG_H
#define __DEBUG_H

#include "../Cryptography/BNMath/BNMath.h"

#include <stdio.h>

//Trace level definitions
#define TRACE_LEVEL_OFF      0
#define TRACE_LEVEL_FATAL    1
#define TRACE_LEVEL_ERROR    2
#define TRACE_LEVEL_WARNING  3
#define TRACE_LEVEL_INFO     4
#define TRACE_LEVEL_DEBUG    5
#define TRACE_LEVEL_VERBOSE  6

//Default trace level
#ifndef TRACE_LEVEL
#define TRACE_LEVEL TRACE_LEVEL_DEBUG
#endif

//Trace output redirection
#ifndef TRACE_PRINTF
#define TRACE_PRINTF(...)		fprintf(stderr, __VA_ARGS__)
#endif

#ifndef TRACE_ARRAY
#define TRACE_ARRAY(p, a, n)	fprintf(stderr, "Trace Array")
#endif

#ifndef TRACE_MPI
#define TRACE_MPI(p, a)			BN_Dump(stderr, p, a)
#endif

//Debugging macros
#if (TRACE_LEVEL >= TRACE_LEVEL_FATAL)
#define TRACE_FATAL(...) TRACE_PRINTF(__VA_ARGS__)
#define TRACE_FATAL_ARRAY(p, a, n) TRACE_ARRAY(p, a, n)
#define TRACE_FATAL_MPI(p, a) TRACE_MPI(p, a)
#else
#define TRACE_FATAL(...)
#define TRACE_FATAL_ARRAY(p, a, n)
#define TRACE_FATAL_MPI(p, a)
#endif

#if (TRACE_LEVEL >= TRACE_LEVEL_ERROR)
#define TRACE_ERROR(...) TRACE_PRINTF(__VA_ARGS__)
#define TRACE_ERROR_ARRAY(p, a, n) TRACE_ARRAY(p, a, n)
#define TRACE_ERROR_MPI(p, a) TRACE_MPI(p, a)
#else
#define TRACE_ERROR(...)
#define TRACE_ERROR_ARRAY(p, a, n)
#define TRACE_ERROR_MPI(p, a)
#endif

#if (TRACE_LEVEL >= TRACE_LEVEL_WARNING)
#define TRACE_WARNING(...) TRACE_PRINTF(__VA_ARGS__)
#define TRACE_WARNING_ARRAY(p, a, n) TRACE_ARRAY(p, a, n)
#define TRACE_WARNING_MPI(p, a) TRACE_MPI(p, a)
#else
#define TRACE_WARNING(...)
#define TRACE_WARNING_ARRAY(p, a, n)
#define TRACE_WARNING_MPI(p, a)
#endif

#if (TRACE_LEVEL >= TRACE_LEVEL_INFO)
#define TRACE_INFO(...) TRACE_PRINTF(__VA_ARGS__)
#define TRACE_INFO_ARRAY(p, a, n) TRACE_ARRAY(p, a, n)
#define TRACE_INFO_NET_BUFFER(p, b, o, n)
#define TRACE_INFO_MPI(p, a) TRACE_MPI(p, a)
#else
#define TRACE_INFO(...)
#define TRACE_INFO_ARRAY(p, a, n)
#define TRACE_INFO_NET_BUFFER(p, b, o, n)
#define TRACE_INFO_MPI(p, a)
#endif

#if (TRACE_LEVEL >= TRACE_LEVEL_DEBUG)
#define TRACE_DEBUG(...) TRACE_PRINTF(__VA_ARGS__)
#define TRACE_DEBUG_ARRAY(p, a, n) TRACE_ARRAY(p, a, n)
#define TRACE_DEBUG_NET_BUFFER(p, b, o, n)
#define TRACE_DEBUG_MPI(p, a) TRACE_MPI(p, a)
#else
#define TRACE_DEBUG(...)
#define TRACE_DEBUG_ARRAY(p, a, n)
#define TRACE_DEBUG_NET_BUFFER(p, b, o, n)
#define TRACE_DEBUG_MPI(p, a)
#endif

#if (TRACE_LEVEL >= TRACE_LEVEL_VERBOSE)
#define TRACE_VERBOSE(...) TRACE_PRINTF(__VA_ARGS__)
#define TRACE_VERBOSE_ARRAY(p, a, n) TRACE_ARRAY(p, a, n)
#define TRACE_VERBOSE_NET_BUFFER(p, b, o, n)
#define TRACE_VERBOSE_MPI(p, a) TRACE_MPI(p, a)
#else
#define TRACE_VERBOSE(...)
#define TRACE_VERBOSE_ARRAY(p, a, n)
#define TRACE_VERBOSE_NET_BUFFER(p, b, o, n)
#define TRACE_VERBOSE_MPI(p, a)
#endif


#endif // !__DEBUG_H