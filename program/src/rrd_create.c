/*****************************************************************************
 * RRDtool 1.4.0  Copyright by Tobi Oetiker, 1997-2009
 *****************************************************************************
 * rrd_create.c  creates new rrds
 *****************************************************************************/

#include <stdlib.h>
#include <time.h>
#include <locale.h>

#include "rrd_tool.h"
#include "rrd_rpncalc.h"
#include "rrd_hw.h"

#include "rrd_is_thread_safe.h"

#ifdef WIN32
# include <process.h>
#endif

unsigned long FnvHash(
    const char *str);
int       create_hw_contingent_rras(
    rrd_t *rrd,
    unsigned short period,
    unsigned long hashed_name);
void      parseGENERIC_DS(
    const char *def,
    rrd_t *rrd,
    int ds_idx);

static void rrd_free2(
    rrd_t *rrd);        /* our onwn copy, immmune to mmap */

int rrd_create(
    int argc,
    char **argv)
{
    struct option long_options[] = {
        {"start", required_argument, 0, 'b'},
        {"step", required_argument, 0, 's'},
        {0, 0, 0, 0}
    };
    int       option_index = 0;
    int       opt;
    time_t    last_up = time(NULL) - 10;
    unsigned long pdp_step = 300;
    rrd_time_value_t last_up_tv;
    char     *parsetime_error = NULL;
    long      long_tmp;
    int       rc;

    optind = 0;
    opterr = 0;         /* initialize getopt */

    while (1) {
        opt = getopt_long(argc, argv, "b:s:", long_options, &option_index);

        if (opt == EOF)
            break;

        switch (opt) {
        case 'b':
            if ((parsetime_error = rrd_parsetime(optarg, &last_up_tv))) {
                rrd_set_error("start time: %s", parsetime_error);
                return (-1);
            }
            if (last_up_tv.type == RELATIVE_TO_END_TIME ||
                last_up_tv.type == RELATIVE_TO_START_TIME) {
                rrd_set_error("specifying time relative to the 'start' "
                              "or 'end' makes no sense here");
                return (-1);
            }

            last_up = mktime(&last_up_tv.tm) +last_up_tv.offset;

            if (last_up < 3600 * 24 * 365 * 10) {
                rrd_set_error
                    ("the first entry to the RRD should be after 1980");
                return (-1);
            }
            break;

        case 's':
            long_tmp = atol(optarg);
            if (long_tmp < 1) {
                rrd_set_error("step size should be no less than one second");
                return (-1);
            }
            pdp_step = long_tmp;
            break;

        case '?':
            if (optopt != 0)
                rrd_set_error("unknown option '%c'", optopt);
            else
                rrd_set_error("unknown option '%s'", argv[optind - 1]);
            return (-1);
        }
    }
    if (optind == argc) {
        rrd_set_error("need name of an rrd file to create");
        return -1;
    }
    rc = rrd_create_r(argv[optind],
                      pdp_step, last_up,
                      argc - optind - 1, (const char **) (argv + optind + 1));

    return rc;
}

/* #define DEBUG */
int rrd_create_r(
    const char *filename,
    unsigned long pdp_step,
    time_t last_up,
    int argc,
    const char **argv)
{
    rrd_t     rrd;
    long      i;
    int       offset;
    char     *token;
    char      dummychar1[2], dummychar2[2];
    unsigned short token_idx, error_flag, period = 0;
    unsigned long hashed_name;

    /* init rrd clean */
    rrd_init(&rrd);
    /* static header */
    if ((rrd.stat_head = (stat_head_t*)calloc(1, sizeof(stat_head_t))) == NULL) {
        rrd_set_error("allocating rrd.stat_head");
        rrd_free2(&rrd);
        return (-1);
    }

    /* live header */
    if ((rrd.live_head = (live_head_t*)calloc(1, sizeof(live_head_t))) == NULL) {
        rrd_set_error("allocating rrd.live_head");
        rrd_free2(&rrd);
        return (-1);
    }

    /* set some defaults */
    strcpy(rrd.stat_head->cookie, RRD_COOKIE);
    strcpy(rrd.stat_head->version, RRD_VERSION3);   /* by default we are still version 3 */
    rrd.stat_head->float_cookie = FLOAT_COOKIE;
    rrd.stat_head->ds_cnt = 0;  /* this will be adjusted later */
    rrd.stat_head->rra_cnt = 0; /* ditto */
    rrd.stat_head->pdp_step = pdp_step; /* 5 minute default */

    /* a default value */
    rrd.ds_def = NULL;
    rrd.rra_def = NULL;

    rrd.live_head->last_up = last_up;

    /* optind points to the first non-option command line arg,
     * in this case, the file name. */
    /* Compute the FNV hash value (used by SEASONAL and DEVSEASONAL
     * arrays. */
    hashed_name = FnvHash(filename);
    for (i = 0; i < argc; i++) {
        unsigned int ii;

        if (strncmp(argv[i], "DS:", 3) == 0) {
            size_t    old_size = sizeof(ds_def_t) * (rrd.stat_head->ds_cnt);

            if ((rrd.ds_def = (ds_def_t*)rrd_realloc(rrd.ds_def,
                                          old_size + sizeof(ds_def_t))) ==
                NULL) {
                rrd_set_error("allocating rrd.ds_def");
                rrd_free2(&rrd);
                return (-1);
            }
            memset(&rrd.ds_def[rrd.stat_head->ds_cnt], 0, sizeof(ds_def_t));
            /* extract the name and type */
            switch (sscanf(&argv[i][3],
                           DS_NAM_FMT "%1[:]" DST_FMT "%1[:]%n",
                           rrd.ds_def[rrd.stat_head->ds_cnt].ds_nam,
                           dummychar1,
                           rrd.ds_def[rrd.stat_head->ds_cnt].dst,
                           dummychar2, &offset)) {
            case 0:
            case 1:
                rrd_set_error("Invalid DS name");
                break;
            case 2:
            case 3:
                rrd_set_error("Invalid DS type");
                break;
            case 4:    /* (%n may or may not be counted) */
            case 5:    /* check for duplicate datasource names */
                for (ii = 0; ii < rrd.stat_head->ds_cnt; ii++)
                    if (strcmp(rrd.ds_def[rrd.stat_head->ds_cnt].ds_nam,
                               rrd.ds_def[ii].ds_nam) == 0)
                        rrd_set_error("Duplicate DS name: %s",
                                      rrd.ds_def[ii].ds_nam);
                /* DS_type may be valid or not. Checked later */
                break;
            default:
                rrd_set_error("invalid DS format");
            }
            if (rrd_test_error()) {
                rrd_free2(&rrd);
                return -1;
            }

            /* parse the remainder of the arguments */
            switch (dst_conv(rrd.ds_def[rrd.stat_head->ds_cnt].dst)) {
            case DST_COUNTER:
            case DST_ABSOLUTE:
            case DST_GAUGE:
            case DST_DERIVE:
                parseGENERIC_DS(&argv[i][offset + 3], &rrd,
                                rrd.stat_head->ds_cnt);
                break;
            case DST_CDEF:
                parseCDEF_DS(&argv[i][offset + 3], &rrd,
                             rrd.stat_head->ds_cnt);
                break;
            default:
                rrd_set_error("invalid DS type specified");
                break;
            }

            if (rrd_test_error()) {
                rrd_free2(&rrd);
                return -1;
            }
            rrd.stat_head->ds_cnt++;
        } else if (strncmp(argv[i], "RRA:", 4) == 0) {
            char     *argvcopy;
            char     *tokptr = "";
            size_t    old_size = sizeof(rra_def_t) * (rrd.stat_head->rra_cnt);
            int       row_cnt;

            if ((rrd.rra_def = (rra_def_t*)rrd_realloc(rrd.rra_def,
                                           old_size + sizeof(rra_def_t))) ==
                NULL) {
                rrd_set_error("allocating rrd.rra_def");
                rrd_free2(&rrd);
                return (-1);
            }
            memset(&rrd.rra_def[rrd.stat_head->rra_cnt], 0,
                   sizeof(rra_def_t));

            argvcopy = strdup(argv[i]);
            token = strtok_r(&argvcopy[4], ":", &tokptr);
            token_idx = error_flag = 0;
            while (token != NULL) {
                switch (token_idx) {
                case 0:
                    if (sscanf(token, CF_NAM_FMT,
                               rrd.rra_def[rrd.stat_head->rra_cnt].cf_nam) !=
                        1)
                        rrd_set_error("Failed to parse CF name");
                    switch (cf_conv
                            (rrd.rra_def[rrd.stat_head->rra_cnt].cf_nam)) {
                    case CF_MHWPREDICT:
                        strcpy(rrd.stat_head->version, RRD_VERSION);    /* MHWPREDICT causes Version 4 */
                    case CF_HWPREDICT:
                        /* initialize some parameters */
                        rrd.rra_def[rrd.stat_head->rra_cnt].par[RRA_hw_alpha].
                            u_val = 0.1;
                        rrd.rra_def[rrd.stat_head->rra_cnt].par[RRA_hw_beta].
                            u_val = 1.0 / 288;
                        rrd.rra_def[rrd.stat_head->rra_cnt].
                            par[RRA_dependent_rra_idx].u_cnt =
                            rrd.stat_head->rra_cnt;
                        break;
                    case CF_DEVSEASONAL:
                    case CF_SEASONAL:
                        /* initialize some parameters */
                        rrd.rra_def[rrd.stat_head->rra_cnt].
                            par[RRA_seasonal_gamma].u_val = 0.1;
                        rrd.rra_def[rrd.stat_head->rra_cnt].
                            par[RRA_seasonal_smoothing_window].u_val = 0.05;
                        /* fall through */
                    case CF_DEVPREDICT:
                        rrd.rra_def[rrd.stat_head->rra_cnt].
                            par[RRA_dependent_rra_idx].u_cnt = -1;
                        break;
                    case CF_FAILURES:
                        rrd.rra_def[rrd.stat_head->rra_cnt].
                            par[RRA_delta_pos].u_val = 2.0;
                        rrd.rra_def[rrd.stat_head->rra_cnt].
                            par[RRA_delta_neg].u_val = 2.0;
                        rrd.rra_def[rrd.stat_head->rra_cnt].
                            par[RRA_window_len].u_cnt = 3;
                        rrd.rra_def[rrd.stat_head->rra_cnt].
                            par[RRA_failure_threshold].u_cnt = 2;
                        rrd.rra_def[rrd.stat_head->rra_cnt].
                            par[RRA_dependent_rra_idx].u_cnt = -1;
                        break;
                        /* invalid consolidation function */
                    case -1:
                        rrd_set_error
                            ("Unrecognized consolidation function %s",
                             rrd.rra_def[rrd.stat_head->rra_cnt].cf_nam);
                    default:
                        break;
                    }
                    /* default: 1 pdp per cdp */
                    rrd.rra_def[rrd.stat_head->rra_cnt].pdp_cnt = 1;
                    break;
                case 1:
                    switch (cf_conv
                            (rrd.rra_def[rrd.stat_head->rra_cnt].cf_nam)) {
                    case CF_HWPREDICT:
                    case CF_MHWPREDICT:
                    case CF_DEVSEASONAL:
                    case CF_SEASONAL:
                    case CF_DEVPREDICT:
                    case CF_FAILURES:
                        row_cnt = atoi(token);
                        if (row_cnt <= 0)
                            rrd_set_error("Invalid row count: %i", row_cnt);
                        rrd.rra_def[rrd.stat_head->rra_cnt].row_cnt = row_cnt;
                        break;
                    default:
                        rrd.rra_def[rrd.stat_head->rra_cnt].
                            par[RRA_cdp_xff_val].u_val = atof(token);
                        if (rrd.rra_def[rrd.stat_head->rra_cnt].
                            par[RRA_cdp_xff_val].u_val < 0.0
                            || rrd.rra_def[rrd.stat_head->rra_cnt].
                            par[RRA_cdp_xff_val].u_val >= 1.0)
                            rrd_set_error
                                ("Invalid xff: must be between 0 and 1");
                        break;
                    }
                    break;
                case 2:
                    switch (cf_conv
                            (rrd.rra_def[rrd.stat_head->rra_cnt].cf_nam)) {
                    case CF_HWPREDICT:
                    case CF_MHWPREDICT:
                        rrd.rra_def[rrd.stat_head->rra_cnt].par[RRA_hw_alpha].
                            u_val = atof(token);
                        if (atof(token) <= 0.0 || atof(token) >= 1.0)
                            rrd_set_error
                                ("Invalid alpha: must be between 0 and 1");
                        break;
                    case CF_DEVSEASONAL:
                    case CF_SEASONAL:
                        rrd.rra_def[rrd.stat_head->rra_cnt].
                            par[RRA_seasonal_gamma].u_val = atof(token);
                        if (atof(token) <= 0.0 || atof(token) >= 1.0)
                            rrd_set_error
                                ("Invalid gamma: must be between 0 and 1");
                        rrd.rra_def[rrd.stat_head->rra_cnt].
                            par[RRA_seasonal_smooth_idx].u_cnt =
                            hashed_name %
                            rrd.rra_def[rrd.stat_head->rra_cnt].row_cnt;
                        break;
                    case CF_FAILURES:
                        /* specifies the # of violations that constitutes the failure threshold */
                        rrd.rra_def[rrd.stat_head->rra_cnt].
                            par[RRA_failure_threshold].u_cnt = atoi(token);
                        if (atoi(token) < 1
                            || atoi(token) > MAX_FAILURES_WINDOW_LEN)
                            rrd_set_error
                                ("Failure threshold is out of range %d, %d",
                                 1, MAX_FAILURES_WINDOW_LEN);
                        break;
                    case CF_DEVPREDICT:
                        /* specifies the index (1-based) of CF_DEVSEASONAL array
                         * associated with this CF_DEVPREDICT array. */
                        rrd.rra_def[rrd.stat_head->rra_cnt].
                            par[RRA_dependent_rra_idx].u_cnt =
                            atoi(token) - 1;
                        break;
                    default:
                        rrd.rra_def[rrd.stat_head->rra_cnt].pdp_cnt =
                            atoi(token);
                        if (atoi(token) < 1)
                            rrd_set_error("Invalid step: must be >= 1");
                        break;
                    }
                    break;
                case 3:
                    switch (cf_conv
                            (rrd.rra_def[rrd.stat_head->rra_cnt].cf_nam)) {
                    case CF_HWPREDICT:
                    case CF_MHWPREDICT:
                        rrd.rra_def[rrd.stat_head->rra_cnt].par[RRA_hw_beta].
                            u_val = atof(token);
                        if (atof(token) < 0.0 || atof(token) > 1.0)
                            rrd_set_error
                                ("Invalid beta: must be between 0 and 1");
                        break;
                    case CF_DEVSEASONAL:
                    case CF_SEASONAL:
                        /* specifies the index (1-based) of CF_HWPREDICT array
                         * associated with this CF_DEVSEASONAL or CF_SEASONAL array. 
                         * */
                        rrd.rra_def[rrd.stat_head->rra_cnt].
                            par[RRA_dependent_rra_idx].u_cnt =
                            atoi(token) - 1;
                        break;
                    case CF_FAILURES:
                        /* specifies the window length */
                        rrd.rra_def[rrd.stat_head->rra_cnt].
                            par[RRA_window_len].u_cnt = atoi(token);
                        if (atoi(token) < 1
                            || atoi(token) > MAX_FAILURES_WINDOW_LEN)
                            rrd_set_error
                                ("Window length is out of range %d, %d", 1,
                                 MAX_FAILURES_WINDOW_LEN);
                        /* verify that window length exceeds the failure threshold */
                        if (rrd.rra_def[rrd.stat_head->rra_cnt].
                            par[RRA_window_len].u_cnt <
                            rrd.rra_def[rrd.stat_head->rra_cnt].
                            par[RRA_failure_threshold].u_cnt)
                            rrd_set_error
                                ("Window length is shorter than the failure threshold");
                        break;
                    case CF_DEVPREDICT:
                        /* shouldn't be any more arguments */
                        rrd_set_error
                            ("Unexpected extra argument for consolidation function DEVPREDICT");
                        break;
                    default:
                        row_cnt = atoi(token);
                        if (row_cnt <= 0)
                            rrd_set_error("Invalid row count: %i", row_cnt);
                        rrd.rra_def[rrd.stat_head->rra_cnt].row_cnt = row_cnt;
                        break;
                    }
                    break;
                case 4:
                    switch (cf_conv
                            (rrd.rra_def[rrd.stat_head->rra_cnt].cf_nam)) {
                    case CF_FAILURES:
                        /* specifies the index (1-based) of CF_DEVSEASONAL array
                         * associated with this CF_DEVFAILURES array. */
                        rrd.rra_def[rrd.stat_head->rra_cnt].
                            par[RRA_dependent_rra_idx].u_cnt =
                            atoi(token) - 1;
                        break;
                    case CF_DEVSEASONAL:
                    case CF_SEASONAL:
                        /* optional smoothing window */
                        if (sscanf(token, "smoothing-window=%lf",
                                   &(rrd.rra_def[rrd.stat_head->rra_cnt].
                                     par[RRA_seasonal_smoothing_window].
                                     u_val))) {
                            strcpy(rrd.stat_head->version, RRD_VERSION);    /* smoothing-window causes Version 4 */
                            if (rrd.rra_def[rrd.stat_head->rra_cnt].
                                par[RRA_seasonal_smoothing_window].u_val < 0.0
                                || rrd.rra_def[rrd.stat_head->rra_cnt].
                                par[RRA_seasonal_smoothing_window].u_val >
                                1.0) {
                                rrd_set_error
                                    ("Invalid smoothing-window %f: must be between 0 and 1",
                                     rrd.rra_def[rrd.stat_head->rra_cnt].
                                     par[RRA_seasonal_smoothing_window].
                                     u_val);
                            }
                        } else {
                            rrd_set_error("Invalid option %s", token);
                        }
                        break;
                    case CF_HWPREDICT:
                    case CF_MHWPREDICT:
                        /* length of the associated CF_SEASONAL and CF_DEVSEASONAL arrays. */
                        period = atoi(token);
                        if (period >
                            rrd.rra_def[rrd.stat_head->rra_cnt].row_cnt)
                            rrd_set_error
                                ("Length of seasonal cycle exceeds length of HW prediction array");
                        break;
                    default:
                        /* shouldn't be any more arguments */
                        rrd_set_error
                            ("Unexpected extra argument for consolidation function %s",
                             rrd.rra_def[rrd.stat_head->rra_cnt].cf_nam);
                        break;
                    }
                    break;
                case 5:
                    /* If we are here, this must be a CF_HWPREDICT RRA.
                     * Specifies the index (1-based) of CF_SEASONAL array
                     * associated with this CF_HWPREDICT array. If this argument 
                     * is missing, then the CF_SEASONAL, CF_DEVSEASONAL, CF_DEVPREDICT,
                     * CF_FAILURES.
                     * arrays are created automatically. */
                    rrd.rra_def[rrd.stat_head->rra_cnt].
                        par[RRA_dependent_rra_idx].u_cnt = atoi(token) - 1;
                    break;
                default:
                    /* should never get here */
                    rrd_set_error("Unknown error");
                    break;
                }       /* end switch */
                if (rrd_test_error()) {
                    /* all errors are unrecoverable */
                    free(argvcopy);
                    rrd_free2(&rrd);
                    return (-1);
                }
                token = strtok_r(NULL, ":", &tokptr);
                token_idx++;
            }           /* end while */
            free(argvcopy);
#ifdef DEBUG
            fprintf(stderr,
                    "Creating RRA CF: %s, dep idx %lu, current idx %lu\n",
                    rrd.rra_def[rrd.stat_head->rra_cnt].cf_nam,
                    rrd.rra_def[rrd.stat_head->rra_cnt].
                    par[RRA_dependent_rra_idx].u_cnt, rrd.stat_head->rra_cnt);
#endif
            /* should we create CF_SEASONAL, CF_DEVSEASONAL, and CF_DEVPREDICT? */
            if ((cf_conv(rrd.rra_def[rrd.stat_head->rra_cnt].cf_nam) ==
                 CF_HWPREDICT
                 || cf_conv(rrd.rra_def[rrd.stat_head->rra_cnt].cf_nam) ==
                 CF_MHWPREDICT)
                && rrd.rra_def[rrd.stat_head->rra_cnt].
                par[RRA_dependent_rra_idx].u_cnt == rrd.stat_head->rra_cnt) {
#ifdef DEBUG
                fprintf(stderr, "Creating HW contingent RRAs\n");
#endif
                if (create_hw_contingent_rras(&rrd, period, hashed_name) ==
                    -1) {
                    rrd_set_error("creating contingent RRA");
                    rrd_free2(&rrd);
                    return -1;
                }
            }
            rrd.stat_head->rra_cnt++;
        } else {
            rrd_set_error("can't parse argument '%s'", argv[i]);
            rrd_free2(&rrd);
            return -1;
        }
    }


    if (rrd.stat_head->rra_cnt < 1) {
        rrd_set_error("you must define at least one Round Robin Archive");
        rrd_free2(&rrd);
        return (-1);
    }

    if (rrd.stat_head->ds_cnt < 1) {
        rrd_set_error("you must define at least one Data Source");
        rrd_free2(&rrd);
        return (-1);
    }
    return rrd_create_fn(filename, &rrd);
}

void parseGENERIC_DS(
    const char *def,
    rrd_t *rrd,
    int ds_idx)
{
    char      minstr[DS_NAM_SIZE], maxstr[DS_NAM_SIZE];
    char     *old_locale;

    /*
       int temp;

       temp = sscanf(def,"%lu:%18[^:]:%18[^:]", 
       &(rrd -> ds_def[ds_idx].par[DS_mrhb_cnt].u_cnt),
       minstr,maxstr);
     */
    old_locale = setlocale(LC_NUMERIC, "C");
    if (sscanf(def, "%lu:%18[^:]:%18[^:]",
               &(rrd->ds_def[ds_idx].par[DS_mrhb_cnt].u_cnt),
               minstr, maxstr) == 3) {
        if (minstr[0] == 'U' && minstr[1] == 0)
            rrd->ds_def[ds_idx].par[DS_min_val].u_val = DNAN;
        else
            rrd->ds_def[ds_idx].par[DS_min_val].u_val = atof(minstr);

        if (maxstr[0] == 'U' && maxstr[1] == 0)
            rrd->ds_def[ds_idx].par[DS_max_val].u_val = DNAN;
        else
            rrd->ds_def[ds_idx].par[DS_max_val].u_val = atof(maxstr);

        if (!isnan(rrd->ds_def[ds_idx].par[DS_min_val].u_val) &&
            !isnan(rrd->ds_def[ds_idx].par[DS_max_val].u_val) &&
            rrd->ds_def[ds_idx].par[DS_min_val].u_val
            >= rrd->ds_def[ds_idx].par[DS_max_val].u_val) {
            rrd_set_error("min must be less than max in DS definition");
            setlocale(LC_NUMERIC, old_locale);
            return;
        }
    } else {
        rrd_set_error("failed to parse data source %s", def);
    }
    setlocale(LC_NUMERIC, old_locale);
}

/* Create the CF_DEVPREDICT, CF_DEVSEASONAL, CF_SEASONAL, and CF_FAILURES RRAs
 * associated with a CF_HWPREDICT RRA. */
int create_hw_contingent_rras(
    rrd_t *rrd,
    unsigned short period,
    unsigned long hashed_name)
{
    size_t    old_size;
    rra_def_t *current_rra;

    /* save index to CF_HWPREDICT */
    unsigned long hw_index = rrd->stat_head->rra_cnt;

    /* advance the pointer */
    (rrd->stat_head->rra_cnt)++;
    /* allocate the memory for the 4 contingent RRAs */
    old_size = sizeof(rra_def_t) * (rrd->stat_head->rra_cnt);
    if ((rrd->rra_def = (rra_def_t*)rrd_realloc(rrd->rra_def,
                                    old_size + 4 * sizeof(rra_def_t))) ==
        NULL) {
        rrd_free2(rrd);
        rrd_set_error("allocating rrd.rra_def");
        return (-1);
    }
    /* clear memory */
    memset(&(rrd->rra_def[rrd->stat_head->rra_cnt]), 0,
           4 * sizeof(rra_def_t));

    /* create the CF_SEASONAL RRA */
    current_rra = &(rrd->rra_def[rrd->stat_head->rra_cnt]);
    strcpy(current_rra->cf_nam, "SEASONAL");
    current_rra->row_cnt = period;
    current_rra->par[RRA_seasonal_smooth_idx].u_cnt = hashed_name % period;
    current_rra->pdp_cnt = 1;
    current_rra->par[RRA_seasonal_gamma].u_val =
        rrd->rra_def[hw_index].par[RRA_hw_alpha].u_val;
    current_rra->par[RRA_dependent_rra_idx].u_cnt = hw_index;
    rrd->rra_def[hw_index].par[RRA_dependent_rra_idx].u_cnt =
        rrd->stat_head->rra_cnt;

    /* create the CF_DEVSEASONAL RRA */
    (rrd->stat_head->rra_cnt)++;
    current_rra = &(rrd->rra_def[rrd->stat_head->rra_cnt]);
    strcpy(current_rra->cf_nam, "DEVSEASONAL");
    current_rra->row_cnt = period;
    current_rra->par[RRA_seasonal_smooth_idx].u_cnt = hashed_name % period;
    current_rra->pdp_cnt = 1;
    current_rra->par[RRA_seasonal_gamma].u_val =
        rrd->rra_def[hw_index].par[RRA_hw_alpha].u_val;
    current_rra->par[RRA_dependent_rra_idx].u_cnt = hw_index;

    /* create the CF_DEVPREDICT RRA */
    (rrd->stat_head->rra_cnt)++;
    current_rra = &(rrd->rra_def[rrd->stat_head->rra_cnt]);
    strcpy(current_rra->cf_nam, "DEVPREDICT");
    current_rra->row_cnt = (rrd->rra_def[hw_index]).row_cnt;
    current_rra->pdp_cnt = 1;
    current_rra->par[RRA_dependent_rra_idx].u_cnt = hw_index + 2;   /* DEVSEASONAL */

    /* create the CF_FAILURES RRA */
    (rrd->stat_head->rra_cnt)++;
    current_rra = &(rrd->rra_def[rrd->stat_head->rra_cnt]);
    strcpy(current_rra->cf_nam, "FAILURES");
    current_rra->row_cnt = period;
    current_rra->pdp_cnt = 1;
    current_rra->par[RRA_delta_pos].u_val = 2.0;
    current_rra->par[RRA_delta_neg].u_val = 2.0;
    current_rra->par[RRA_failure_threshold].u_cnt = 7;
    current_rra->par[RRA_window_len].u_cnt = 9;
    current_rra->par[RRA_dependent_rra_idx].u_cnt = hw_index + 2;   /* DEVSEASONAL */
    return 0;
}

/* create and empty rrd file according to the specs given */

int rrd_create_fn(
    const char *file_name,
    rrd_t *rrd)
{
    unsigned long i, ii;
    rrd_value_t *unknown;
    int       unkn_cnt;
    rrd_file_t *rrd_file_dn;
    rrd_t     rrd_dn;
    unsigned  rrd_flags = RRD_READWRITE | RRD_CREAT;

    unkn_cnt = 0;
    for (i = 0; i < rrd->stat_head->rra_cnt; i++)
        unkn_cnt += rrd->stat_head->ds_cnt * rrd->rra_def[i].row_cnt;

    if ((rrd_file_dn = rrd_open(file_name, rrd, rrd_flags)) == NULL) {
        rrd_set_error("creating '%s': %s", file_name, rrd_strerror(errno));
        rrd_free2(rrd);
        return (-1);
    }

    rrd_write(rrd_file_dn, rrd->stat_head, sizeof(stat_head_t));

    rrd_write(rrd_file_dn, rrd->ds_def, sizeof(ds_def_t) * rrd->stat_head->ds_cnt);

    rrd_write(rrd_file_dn, rrd->rra_def,
          sizeof(rra_def_t) * rrd->stat_head->rra_cnt);

    rrd_write(rrd_file_dn, rrd->live_head, sizeof(live_head_t));

    if ((rrd->pdp_prep = (pdp_prep_t*)calloc(1, sizeof(pdp_prep_t))) == NULL) {
        rrd_set_error("allocating pdp_prep");
        rrd_free2(rrd);
        rrd_close(rrd_file_dn);
        return (-1);
    }

    strcpy(rrd->pdp_prep->last_ds, "U");

    rrd->pdp_prep->scratch[PDP_val].u_val = 0.0;
    rrd->pdp_prep->scratch[PDP_unkn_sec_cnt].u_cnt =
        rrd->live_head->last_up % rrd->stat_head->pdp_step;

    for (i = 0; i < rrd->stat_head->ds_cnt; i++)
        rrd_write(rrd_file_dn, rrd->pdp_prep, sizeof(pdp_prep_t));

    if ((rrd->cdp_prep = (cdp_prep_t*)calloc(1, sizeof(cdp_prep_t))) == NULL) {
        rrd_set_error("allocating cdp_prep");
        rrd_free2(rrd);
        rrd_close(rrd_file_dn);
        return (-1);
    }


    for (i = 0; i < rrd->stat_head->rra_cnt; i++) {
        switch (cf_conv(rrd->rra_def[i].cf_nam)) {
        case CF_HWPREDICT:
        case CF_MHWPREDICT:
            init_hwpredict_cdp(rrd->cdp_prep);
            break;
        case CF_SEASONAL:
        case CF_DEVSEASONAL:
            init_seasonal_cdp(rrd->cdp_prep);
            break;
        case CF_FAILURES:
            /* initialize violation history to 0 */
            for (ii = 0; ii < MAX_CDP_PAR_EN; ii++) {
                /* We can zero everything out, by setting u_val to the
                 * NULL address. Each array entry in scratch is 8 bytes
                 * (a double), but u_cnt only accessed 4 bytes (long) */
                rrd->cdp_prep->scratch[ii].u_val = 0.0;
            }
            break;
        default:
            /* can not be zero because we don't know anything ... */
            rrd->cdp_prep->scratch[CDP_val].u_val = DNAN;
            /* startup missing pdp count */
            rrd->cdp_prep->scratch[CDP_unkn_pdp_cnt].u_cnt =
                ((rrd->live_head->last_up -
                  rrd->pdp_prep->scratch[PDP_unkn_sec_cnt].u_cnt)
                 % (rrd->stat_head->pdp_step
                    * rrd->rra_def[i].pdp_cnt)) / rrd->stat_head->pdp_step;
            break;
        }

        for (ii = 0; ii < rrd->stat_head->ds_cnt; ii++) {
            rrd_write(rrd_file_dn, rrd->cdp_prep, sizeof(cdp_prep_t));
        }
    }

    /* now, we must make sure that the rest of the rrd
       struct is properly initialized */

    if ((rrd->rra_ptr = (rra_ptr_t*)calloc(1, sizeof(rra_ptr_t))) == NULL) {
        rrd_set_error("allocating rra_ptr");
        rrd_free2(rrd);
        rrd_close(rrd_file_dn);
        return (-1);
    }

    /* changed this initialization to be consistent with
     * rrd_restore. With the old value (0), the first update
     * would occur for cur_row = 1 because rrd_update increments
     * the pointer a priori. */
    for (i = 0; i < rrd->stat_head->rra_cnt; i++) {
        rrd->rra_ptr->cur_row = rrd_select_initial_row(rrd_file_dn, i, &rrd->rra_def[i]);
        rrd_write(rrd_file_dn, rrd->rra_ptr, sizeof(rra_ptr_t));
    }

    /* write the empty data area */
    if ((unknown = (rrd_value_t *) malloc(512 * sizeof(rrd_value_t))) == NULL) {
        rrd_set_error("allocating unknown");
        rrd_free2(rrd);
        rrd_close(rrd_file_dn);
        return (-1);
    }
    for (i = 0; i < 512; ++i)
        unknown[i] = DNAN;

    while (unkn_cnt > 0) {
        if(rrd_write(rrd_file_dn, unknown, sizeof(rrd_value_t) * min(unkn_cnt, 512)) < 0)
        {
            rrd_set_error("creating rrd: %s", rrd_strerror(errno));
            return -1;
        }

        unkn_cnt -= 512;
    }
    free(unknown);
    rrd_free2(rrd);
    if (rrd_close(rrd_file_dn) == -1) {
        rrd_set_error("creating rrd: %s", rrd_strerror(errno));
        return -1;
    }
    /* flush all we don't need out of the cache */
    rrd_init(&rrd_dn);
    if((rrd_file_dn = rrd_open(file_name, &rrd_dn, RRD_READONLY)) != NULL)
    {
        rrd_dontneed(rrd_file_dn, &rrd_dn);
        /* rrd_free(&rrd_dn); */
        rrd_close(rrd_file_dn);
    }
    return (0);
}


static void rrd_free2(
    rrd_t *rrd)
{
    free(rrd->live_head);
    free(rrd->stat_head);
    free(rrd->ds_def);
    free(rrd->rra_def);
    free(rrd->rra_ptr);
    free(rrd->pdp_prep);
    free(rrd->cdp_prep);
    free(rrd->rrd_value);
}

