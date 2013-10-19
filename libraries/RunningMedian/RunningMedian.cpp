//
//    FILE: RunningMedian.cpp
//  AUTHOR: Rob dot Tillaart at gmail dot com
// VERSION: 0.1.06
// PURPOSE: RunningMedian library for Arduino
//
// HISTORY:
// 0.1.00 - 2011-02-16 initial version
// 0.1.01 - 2011-02-22 added remarks from CodingBadly
// 0.1.02 - 2012-03-15 added
// 0.1.03 - 2013-09-30 added _sorted flag, minor refactor
// 0.1.04 - 2013-10-17 added getAverage(uint8_t) - kudo's to Sembazuru
// 0.1.05 - 2013-10-18 fixed bug in sort; removes default constructor; dynamic memory
// 0.1.06 - 2013-10-19 faster sort, dynamic arrays, replaced sorted float array with indirection array
//
// Released to the public domain
//

#include "RunningMedian.h"

RunningMedian::RunningMedian(uint8_t size)
{
    _size = constrain(size, MEDIAN_MIN_SIZE, MEDIAN_MAX_SIZE);

#ifdef RUNNING_MEDIAN_USE_MALLOC
    _ar = (float *) malloc(_size * sizeof(float));
    _p = (uint8_t *) malloc(_size * sizeof(uint8_t));
#endif

    clear();
}

RunningMedian::~RunningMedian()
{
#ifdef RUNNING_MEDIAN_USE_MALLOC
    free(_ar);
    free(_p);
#endif
}

// resets all counters
void RunningMedian::clear()
{
    _cnt = 0;
    _idx = 0;
    _sorted = false;

    for (uint8_t i=0; i< _size; i++) _p[i] = i;
}

// adds a new value to the data-set
// or overwrites the oldest if full.
void RunningMedian::add(float value)
{
    _idx++;
    if (_idx >= _size) _idx = 0; // wrap around
    _ar[_idx] = value;
    if (_cnt < _size) _cnt++;
    _sorted = false;
}

float RunningMedian::getMedian()
{
    if (_cnt > 0)
    {
        if (_sorted == false) sort();
        return _ar[_p[_cnt/2]];
    }
    return NAN;
}

#ifdef RUNNING_MEDIAN_ALL
float RunningMedian::getHighest()
{
    if (_cnt > 0)
    {
        if (_sorted == false) sort(); // assumes other fields are also retrieved otherwise inefficient
        return _ar[_p[_cnt-1]];
    }
    return NAN;
}

float RunningMedian::getLowest()
{
    if (_cnt > 0)
    {	
        if (_sorted == false) sort();
        return _ar[_p[0]];
    }
    return NAN;
}

float RunningMedian::getAverage()
{
    if (_cnt > 0)
    {	
        float sum = 0;
        for (uint8_t i=0; i< _cnt; i++) sum += _ar[i];
        return sum / _cnt;
    }
    return NAN;
}

float RunningMedian::getAverage(uint8_t nMedians)
{
    if ((_cnt > 0) && (nMedians > 0))
    {
        if (_cnt < nMedians) nMedians = _cnt;     // when filling the array for first time
        uint8_t start = ((_cnt - nMedians)/2);
        uint8_t stop = start + nMedians;

        if (_sorted == false) sort();

        float sum = 0;
        for (uint8_t i = start; i < stop; i++) sum += _ar[_p[i]];
        return sum / nMedians;
    }
    return NAN;
}

uint8_t RunningMedian::getSize() { return _size; };

uint8_t RunningMedian::getCount() { return _cnt; };
#endif

void RunningMedian::sort()
{
    // bubble sort with flag
    for (uint8_t i=0; i< _cnt-1; i++)
    {
        bool flag = true;
        for (uint8_t j=1; j< _cnt-i; j++)
        {
            if (_ar[_p[j-1]] > _ar[_p[j]])
            {
                uint8_t t = _p[j-1];
                _p[j-1] = _p[j];
                _p[j] = t;
                flag = false;
            }
        }
        if (flag) break;
    }

    _sorted = true;
}

// END OF FILE