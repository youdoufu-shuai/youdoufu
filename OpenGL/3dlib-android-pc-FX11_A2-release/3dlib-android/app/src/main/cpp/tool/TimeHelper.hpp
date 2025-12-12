/**************************************************************************************************
 * @file TimeHelper.hpp
 * @brief log util.
 * 
 * @details
 * 
 *  HISTORY
 *  -----------------------------------------------------------------------------------------------
 *  Version   Date        Author    Description
 *  -----------------------------------------------------------------------------------------------
 *  0.1       2023-05-15  TDZ       Initial.
 * 
 * 
**************************************************************************************************/
#ifndef TIMEHELPER_HPP
#define TIMEHELPER_HPP

#include <sys/time.h>
#include <cstdlib>
#include <ctime>


namespace BaseLib{


class TimeHelper
{
public:
    static long GetTimestampMillis() 
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }

    static long GetTimestampMicros() 
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec * 1000000 + tv.tv_usec;
    }
};

};


#endif /* TIMEHELPER_HPP */

