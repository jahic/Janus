///this file defines the threading control in Janus
#ifndef _JANUS_THREAD_CONTROL_
#define _JANUS_THREAD_CONTROL_

#include "janus_api.h"
/* JANUS Definitions for runtime states */
#include "state.h"
/* JANUS Runtime handlers for loops */
#include "loop.h"

/* Whole structure shared by all the threads
 * This structure must be aligned to cache line width */
typedef struct _shared_state {
    /* The following variables are set frequently,
     * dummy space is added to remove false sharing effects.
     */
    /* The bank of registers for sharing between the threads */
    shared_state_t          registers;
    /* The shared stamp for thread's epoch */
    uint64_t                global_stamp;
    uint64_t                dummy[7];
    /* The shared stamp for thread's epoch */
    uint64_t                global_lock;
    uint64_t                dummy2[7];
    /* Record of all loops that are in need of parallelisation */
    loop_t                  *loops;
    loop_t                  *current_loop;

    /* The following variables are set once and remain constant
     * there is no need to avoid false sharing */
    volatile uint64_t       current_pc;
    volatile uint32_t       ready;
    ///A flag to tell the warden thread to start JIT compilation
    volatile uint32_t       code_gen_start;
    ///All code has finished JITing.
    volatile uint32_t       code_ready;
    ///A flag to tell all threads to leave thread pool and jump to a PC
    volatile uint32_t       start_run;
    ///A flag to tell all threads to jump back to thread pool
    volatile uint32_t       need_yield;
    ///A flag to tell all threads to terminate
    volatile uint32_t       need_exit;
    ///The code is working on an allocated stack insted of original stack
    volatile uint32_t       stack_allocated;
    ///The code is in a parallelising loop if set
    volatile uint32_t       loop_on;
#ifdef JANUS_JITSTM
    ///The code is in speculative mode if set
    volatile uint32_t       speculate_on;
    ///Currently not yet used
    volatile uint32_t       speculate_dummy;
#endif
    /** \brief runtime flag to indicate the runtime check fails */
    volatile uint32_t       runtime_check_fail;
    /* Record of all functions that are involved */
    uint32_t                number_of_functions;
    //dynamic_code_t          **functions;
    uint64_t                warden_thread;
    uint64_t                *lockArray;
    /* Shared stacks */
    uint64_t                stack_ptr;
    uint64_t                stack_base;
    uint32_t                stack_size;
    /* Depending register mask */
    uint32_t                depending_reg_mask;
    uint32_t                reduction_reg_mask;
    volatile int            finished_threads;
    volatile uint64_t       loop_invocation;

} janus_shared_t;

///Pointers to shared region
extern janus_shared_t *shared;
///Static region of shared threads space
extern janus_shared_t shared_region;
///Status
extern int janus_status;

///Initialise janus threads
int janus_thread_system_init(void);

#endif
