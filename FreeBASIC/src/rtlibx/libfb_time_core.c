/*
 *  libfb - FreeBASIC's runtime library
 *	Copyright (C) 2004-2005 Andre V. T. Vicentini (av1ctor@yahoo.com.br) and others.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * time_core.c -- core time/date functions
 *
 * chng: aug/2005 written [mjs]
 *
 */

#include <assert.h>
#include "fbext.h"

/*:::::*/
int fb_hTimeLeap( int year )
{
    if( ( year % 400 ) == 0 )
        return 1;
    if( ( year % 100 ) == 0 )
        return 0;
    return ( ( ( year & 3 ) == 0 ) ? 1 : 0 );
}

/*:::::*/
int fb_hTimeDaysInMonth( int month, int year )
{
    static const int days[] =
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    assert(month >= 1 && month <= 12 );
    if( month==2 )
        return days[month-1] + fb_hTimeLeap( year );
    return days[month-1];
}

/*:::::*/
void fb_hNormalizeDate( int *pDay, int *pMonth, int *pYear )
{
    assert( pDay!=NULL );
    assert( pMonth!=NULL );
    assert( pYear!=NULL );
    {
        int day = *pDay;
        int month = *pMonth;
        int year = *pYear;
        if( month < 1 ) {
            int sub_months = -month + 1;
            int sub_years = (sub_months + 11) / 12;
            year -= sub_years;
            month = sub_years * 12 - sub_months + 1;
        } else {
            --month;
            year += month / 12;
            month %= 12;
            ++month;
        }

        if( day < 1 ) {
            int sub_days = -day + 1;
            while (sub_days > 0) {
                int dom;
                if( --month==0 ) {
                    month=12;
                    --year;
                }
                dom = fb_hTimeDaysInMonth( month, year );
                if( sub_days > dom ) {
                    sub_days -= dom;
                } else {
                    day = dom - sub_days + 1;
                    sub_days = 0;
                }
            }
        } else {
            int dom = fb_hTimeDaysInMonth( month, year );
            while( day > dom ) {
                day -= dom;
                if( ++month==13 ) {
                    month = 1;
                    ++year;
                }
                dom = fb_hTimeDaysInMonth( month, year );
            }
        }
        *pDay = day;
        *pMonth = month;
        *pYear = year;
    }
}

/*:::::*/
int fb_hTimeGetIntervalType( FBSTRING *interval )
{
    int result = FB_TIME_INTERVAL_INVALID;
    FB_STRLOCK();
    if( interval!=NULL && interval->data!=NULL ) {
        if( strcmp( interval->data, "yyyy" )==0 ) {
            result = FB_TIME_INTERVAL_YEAR;
        } else if( strcmp( interval->data, "q" )==0 ) {
            result = FB_TIME_INTERVAL_QUARTER;
        } else if( strcmp( interval->data, "m" )==0 ) {
            result = FB_TIME_INTERVAL_MONTH;
        } else if( strcmp( interval->data, "y" )==0 ) {
            result = FB_TIME_INTERVAL_DAY_OF_YEAR;
        } else if( strcmp( interval->data, "d" )==0 ) {
            result = FB_TIME_INTERVAL_DAY;
        } else if( strcmp( interval->data, "w" )==0 ) {
            result = FB_TIME_INTERVAL_WEEKDAY;
        } else if( strcmp( interval->data, "ww" )==0 ) {
            result = FB_TIME_INTERVAL_WEEK_OF_YEAR;
        } else if( strcmp( interval->data, "h" )==0 ) {
            result = FB_TIME_INTERVAL_HOUR;
        } else if( strcmp( interval->data, "n" )==0 ) {
            result = FB_TIME_INTERVAL_MINUTE;
        } else if( strcmp( interval->data, "s" )==0 ) {
            result = FB_TIME_INTERVAL_SECOND;
        }
    }
    FB_STRUNLOCK();
    return result;
}
