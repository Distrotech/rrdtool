/*****************************************************************************
 * RRDtool 1.2.8  Copyright by Tobi Oetiker, 1997-2005
 *****************************************************************************
 * rrd_version Return
 *****************************************************************************
 * Initial version by Burton Strauss, ntopSupport.com - 5/2005
 *****************************************************************************/

#include "rrd_tool.h"

double
rrd_version(void)
{
  return NUMVERS;
}


