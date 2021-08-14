#include "libsr.h"
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

_SR_(err) _SR_(init)(_SR_(DriverBase) *srdriver, _SR_(obj) *obj, _SR_(base_t) srnum, bool isinv, uint8_t *bitarr)
{
		if (NULL == obj || NULL == bitarr || NULL == srdriver)
						return _SR_(ENULL);

	/* In case then user is prefer to use inverted type of SR
		 is necessary to set 'isinv' flag to high-level (=1) state.
		 Therefore, input bit array must be inverted.
		 For write section below and similar read-write section all bits will
		 inverted automatically */
	if(isinv){
		for(_SR_(base_t) i=0;i<srnum;i++)
			bitarr[i] ^= 0xff;
	}

	*obj = (_SR_(obj)) {
		.arr = bitarr,
		.isinv = isinv,
		.srnum = srnum,
		.srdriver = srdriver
	};

	obj->srdriver->driver_init();

	return _SR_(EOK);
}

/**
 * Toggle the particular bit in a shift register data array
 */
void _SR_(toggle)(_SR_(obj) *obj, _SR_(base_t) num, bool autocommit)
{
	if(NULL == obj){
		return;
	}

	_SR_(base_t) nreg = num >> 3;

 if(nreg < obj->srnum)
	obj->arr[nreg] ^= (1 << num);

/* Incorrect arguments haven't to influence on autocommit */
if(autocommit)
	_SR_(commit)(obj);

}

/**
 * Set the particular bit in 'state' condition in shift register data array
 */
void _SR_(set)(_SR_(obj) *obj, _SR_(base_t) num, bool state, bool autocommit)
{
	if(NULL == obj){
		return;
	}

	_SR_(base_t) nreg = num >> 3;

	/* Exist only one condition for high-level bit setting:
		 then (bool) state is 1 and is set non-inverting bit register type.
		 The rest cases will produce only clear bit operation
	*/
	if(nreg < obj->srnum){
		if(true == state && false == obj->isinv){
			obj->arr[nreg] |= (1 << num);
		}else{
			obj->arr[nreg] &= ~(1 << num);
		}
	}

	if(autocommit)
		_SR_(commit)(obj);
}

/**
 * Toggle the particular bit at shift register data array
 */
bool _SR_(get)(_SR_(obj) *obj, _SR_(base_t) num)
{
	if(NULL == obj){
		return 0;
	}

	bool resultbit = 0;

	_SR_(base_t) nreg = num >> 3;

	return (nreg < obj->srnum)
				? ( resultbit = (obj->isinv)
				? ~(obj->arr[nreg] & (1 << (num & 0x0000000f)))
				:  (obj->arr[nreg] & (1 << (num & 0x0000000f))) )
				: 0;
}

/**
 * Transfer a current bit sequence to the Shift register hardware set
 */
 _SR_(err) _SR_(commit)(_SR_(obj) *obj)
{
	_SR_(err) err = _SR_(EOK);

	if(NULL == obj){
		return _SR_(ENULL);
	}

	/* Data transferring operation via already specified driver */
	if(_SR_(EOK) != ( err = obj->srdriver->shift(obj, obj->arr, obj->srnum))){
					return _SR_(ENULL);
	}

	if(_SR_(EOK) != ( err = obj->srdriver->latchup(obj))){
		return _SR_(ENULL);
	}

	return err;
}
