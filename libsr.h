/* libsr -- shift register abstraction */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

/** Prefix for function names to avoid namespace collision
 * i.e. test() becomes sr_test()
 * Defaults to sr_ if not defined
 */
#define LIBSR_PREFIX sr_

/** Type which is ideally the fastest type on the target machine,
 * having size equal to its word size
 * Defaults to uint32_t
 */
#define LIBSR_BASE_TYPE uint32_t

/* Defaults */
#if !defined(LIBSR_PREFIX)
#define LIBSR_PREFIX sr_
#endif

#if !defined(LIBSR_BASE_TYPE)
#define LIBSR_BASE_TYPE uint32_t
#endif

/* Utilitary macros */
#define _SRPASTER(x, y) x##y
#define _SREVALUATOR(x, y) _SRPASTER(x, y)
#define _SR_(funcname) _SREVALUATOR(LIBSR_PREFIX, funcname)

/** Have type defined as macro parameter properly redefined */
typedef LIBSR_BASE_TYPE _SR_(base_t);

/** Errors */
typedef enum _SR_(err_e) {
    _SR_(EOK) = 0,    // no error
    _SR_(ENULL)
} _SR_(err);


/* Forward declare this to allow for referencing
 */
typedef struct _SR_(obj_s) _SR_(obj);

/**
 * typedef * _SR_(shiftfunc)
 * Pointer to function shifting out data to shift register
 */
// clang-format off
typedef _SR_(err) (* _SR_(shiftfunc))(struct _SR_(obj_s) *, uint8_t *arr, _SR_(base_t) arr_len);
// clang-format on

/**
 * typedef * `_SR_(latchupfunc)`
 * Pointer to function latching up the shift register after all data has been transfered
 */
// clang-format off
typedef _SR_(err) (* _SR_(latchupfunc))(struct _SR_(obj_s) *);
// clang-format on

/**
 * This is inherited by every concrete driver implementation.
 * Contains pointers to `shiftfunc` and `latchupfunc`
 */
typedef struct _SR_(DriverBase_s) {
        _SR_(shiftfunc) shift;
        _SR_(latchupfunc) latchup;
        _SR_(err) (*driver_init)();
}_SR_(DriverBase);

struct _SR_(obj_s) {
        uint8_t *arr;
        bool isinv;
        _SR_(base_t) srnum;
        _SR_(DriverBase) *srdriver;
};

_SR_(err) _SR_(init)(_SR_(DriverBase) *srdriver, _SR_(obj) *obj, _SR_(base_t) srnum, bool isinv, uint8_t *bitarr);

bool _SR_(get)(_SR_(obj) *obj, _SR_(base_t) num);
void _SR_(set)(_SR_(obj) *obj, _SR_(base_t) num, bool state, bool autocommit);
void _SR_(toggle)(_SR_(obj) *obj, _SR_(base_t) num, bool autocommit);

_SR_(err) _SR_(commit)(_SR_(obj) *obj);
