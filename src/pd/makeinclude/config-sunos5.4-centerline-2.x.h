/** 
 *  Copyright (c) 1999~2017, Altibase Corp. and/or its affiliates. All rights reserved.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License, version 3,
 *  as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
 
/* -*- C++ -*- */
// config-sunos5.4-centerline-2.x.h,v 4.24 1998/12/29 15:47:28 levine Exp

// The following configuration file is designed to work for SunOS 5.4
// platforms using the Centerline 2.x C++ compiler.

#ifndef PDL_CONFIG_H
#define PDL_CONFIG_H

#define PDL_HAS_TEMPLATE_SPECIALIZATION

// Platform supports pread() and pwrite()
#define PDL_HAS_P_READ_WRITE

// Maximum compensation (10 ms) for early return from timed ::select ().
#if !defined (PDL_TIMER_SKEW)
# define PDL_TIMER_SKEW 10 * 1000
#endif /* PDL_TIMER_SKEW */

#define PDL_HAS_HANDLE_SET_OPTIMIZED_FOR_SELECT

#define PDL_HAS_XPG4_MULTIBYTE_CHAR

// Platform supports System V IPC (most versions of UNIX, but not Win32)
#define PDL_HAS_SYSV_IPC

// Sun has the wrong prototype for sendmsg.
#define PDL_HAS_BROKEN_SENDMSG

// The SunOS 5.x version of rand_r is inconsistent with the header files...
#define PDL_HAS_BROKEN_RANDR

// Platform supports system configuration information.
#define PDL_HAS_SYSINFO

// Platform supports the POSIX regular expression library
#define PDL_HAS_REGEX

// Platform supports recvmsg and sendmsg.
#define PDL_HAS_MSG

// Compiler/platform contains the <sys/syscall.h> file.
#define PDL_HAS_SYSCALL_H

// Platform has terminal ioctl flags like TCGETS and TCSETS.
#define PDL_HAS_TERM_IOCTLS

// Compiler/platform correctly calls init()/fini() for shared libraries.
#define PDL_HAS_AUTOMATIC_INIT_FINI

// Platform supports POSIX O_NONBLOCK semantics.
#define PDL_HAS_POSIX_NONBLOCK

// Compiler/platform has correctly prototyped header files.
#define PDL_HAS_CPLUSPLUS_HEADERS

// Compiler/platform supports SunOS high resolution timers.
#define PDL_HAS_HI_RES_TIMER

// Platform supports IP multicast
#define PDL_HAS_IP_MULTICAST

// Platform contains <poll.h>.
#define PDL_HAS_POLL

// Platform supports POSIX timers via timestruc_t.
#define PDL_HAS_POSIX_TIME

// Platform supports the /proc file system.
#define PDL_HAS_PROC_FS

// Platform supports the prusage_t struct.
#define PDL_HAS_PRUSAGE_T

// Compiler/platform defines the sig_atomic_t typedef.
#define PDL_HAS_SIG_ATOMIC_T

// Platform supports SVR4 extended signals.
#define PDL_HAS_SIGINFO_T
#define PDL_HAS_UCONTEXT_T

// Compiler/platform provides the sockio.h file.
#define PDL_HAS_SOCKIO_H

// Compiler supports the ssize_t typedef.
#define PDL_HAS_SSIZE_T

// Platform supports STREAMS.
#define PDL_HAS_STREAMS

// Platform supports STREAM pipes.
#define PDL_HAS_STREAM_PIPES

// Compiler/platform supports strerror ().
#define PDL_HAS_STRERROR

// Compiler/platform supports struct strbuf.
#define PDL_HAS_STRBUF_T

// Compiler/platform supports SVR4 dynamic linking semantics.
#define PDL_HAS_SVR4_DYNAMIC_LINKING

// Andreas Ueltschi tells me this is a good thing...
#define PDL_HAS_SVR5_GETTIMEOFDAY

// Compiler/platform supports SVR4 signal typedef.
#define PDL_HAS_SVR4_SIGNAL_T

// Platform lacks pthread_sigaction
#define PDL_LACKS_PTHREAD_THR_SIGSETMASK

// Compiler/platform supports SVR4 PDL_TLI (in particular, T_GETNAME stuff)...
#define PDL_HAS_SVR4_TLI

// Platform provides <sys/filio.h> header.
#define PDL_HAS_SYS_FILIO_H

// Compiler/platform supports sys_siglist array.
#define PDL_HAS_SYS_SIGLIST

/* Turn off the following defines if you want to disable threading. */
// Compile using multi-thread libraries.
#if !defined (PDL_MT_SAFE)
# define PDL_MT_SAFE 1
#endif

// Platform supports threads.
#define PDL_HAS_THREADS

// Platform supports Solaris threads.
#define PDL_HAS_STHREADS

// Compiler/platform has thread-specific storage
#define PDL_HAS_THREAD_SPECIFIC_STORAGE

// Platform supports reentrant functions (i.e., all the POSIX *_r functions).
#define PDL_HAS_REENTRANT_FUNCTIONS

/* end threading defines */

#define PDL_HAS_PRIOCNTL
#define PDL_NEEDS_LWP_PRIO_SET

// Platform supports PDL_TLI timod STREAMS module.
#define PDL_HAS_TIMOD_H

// Platform supports PDL_TLI tiuser header.
#define PDL_HAS_TIUSER_H

// Platform provides PDL_TLI function prototypes.
//#define PDL_HAS_TLI_PROTOTYPES

// Platform supports PDL_TLI.
//#define PDL_HAS_TLI

#define PDL_NEEDS_DEV_IO_CONVERSION

// 10 millisecond fudge factor to account for Solaris timers...
#if !defined (PDL_TIMER_SKEW)
# define PDL_TIMER_SKEW 1000 * 10
#endif /* PDL_TIMER_SKEW */

// Turns off the tracing feature.
#if !defined (PDL_NTRACE)
# define PDL_NTRACE 1
#endif /* PDL_NTRACE */

// Defines the page size of the system.
#define PDL_PAGE_SIZE 4096
#define PDL_HAS_IDTYPE_T
#define PDL_HAS_DIRENT

#endif /* PDL_CONFIG_H */
