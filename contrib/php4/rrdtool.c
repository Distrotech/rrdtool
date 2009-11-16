/*
 *
 * php_rrdtool.c
 *
 *	PHP interface to RRD Tool. (for php4/zend)
 *
 *
 *       Joe Miller, <joeym@ibizcorp.com>, <joeym@inficad.com> 
 *          iBIZ Technology Corp,  SkyLynx / Inficad Communications
 *          2/12/2000 & 7/18/2000
 *
 *       Jeffrey Wheat <jeff@cetlink.net> - 10/01/2002
 *          - Fixed to build with php-4.2.3
 *
 * See README, INSTALL, and USAGE files for more details.
 *
 * $Id$
 *
 */

/* PHP Includes */
#include "php.h"
#include "php_logos.h"
#include "ext/standard/info.h"
#include "SAPI.h"

/* rrdtool includes */
#include "php_rrdtool.h"
#include "rrdtool_logo.h"
#include <rrd.h>

#ifdef HAVE_CONFIG_H
#include "php_config.h"
#endif

#if HAVE_RRDTOOL

/* If you declare any globals in php_rrdtool.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(rrdtool)
 */

function_entry rrdtool_functions[] = {
	PHP_FE(rrd_graph, NULL)
	PHP_FE(rrd_fetch, NULL)
	PHP_FE(rrd_error, NULL)
	PHP_FE(rrd_clear_error, NULL)
	PHP_FE(rrd_update, NULL)
	PHP_FE(rrd_last, NULL)
        PHP_FE(rrd_lastupdate,                          NULL)
	PHP_FE(rrd_create, NULL)
	PHP_FE(rrdtool_info, NULL)
	PHP_FE(rrdtool_logo_guid, NULL)
	{NULL, NULL, NULL}
};

zend_module_entry rrdtool_module_entry = {
#if ZEND_EXTENSION_API_NO >= 220050617
	STANDARD_MODULE_HEADER_EX, NULL,
	NULL, /* dependencies */
#else
	STANDARD_MODULE_HEADER,
#endif
	"rrdtool", /* name */
	rrdtool_functions, /* functions */
	PHP_MINIT(rrdtool), /* module_startup_func */
	PHP_MSHUTDOWN(rrdtool), /* module_shutdown_func */
	NULL, /* request_startup_func */
	NULL, /* request_shutdown_func */
	PHP_MINFO(rrdtool), /* info_func */
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_RRDTOOL
ZEND_GET_MODULE(rrdtool)
#endif

#ifdef COMPILE_DL_RRDTOOL
#define PHP_RRD_VERSION_STRING "1.2.x extension"
#else
#define PHP_RRD_VERSION_STRING "1.2.x bundled"
#endif

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(rrdtool)
{
	php_register_info_logo(RRDTOOL_LOGO_GUID   , "image/gif", rrdtool_logo   , sizeof(rrdtool_logo));
	
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(rrdtool)
{
	php_unregister_info_logo(RRDTOOL_LOGO_GUID);
	
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(rrdtool)
{
	php_info_print_box_start(1);
	PUTS("<a href=\"http://people.ee.ethz.ch/~oetiker/webtools/rrdtool/\" target=\"rrdtool\"><img border=\"0\" src=\"");
	if (SG(request_info).request_uri) {
		PUTS(SG(request_info).request_uri);
	}
	PUTS("?="RRDTOOL_LOGO_GUID"\" alt=\"ClamAV logo\" /></a>\n");
	php_printf("<h1 class=\"p\">rrdtool Version %s</h1>\n", PHP_RRD_VERSION_STRING);
	php_info_print_box_end();
	php_info_print_table_start();
	php_info_print_table_row(2, "rrdtool support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto mixed rrd_graph(string file, array args_arr, int argc)
	Creates a graph based on options passed via an array */
PHP_FUNCTION(rrd_graph)
{
	pval *file, *args, *p_argc;
	pval *entry;
	zval *p_calcpr;
	HashTable *args_arr;
	int i, xsize, ysize, argc;
	double ymin, ymax;
	char **argv, **calcpr;
    

	if ( rrd_test_error() )
		rrd_clear_error();
    
	if ( (ZEND_NUM_ARGS() >= 3 && ZEND_NUM_ARGS() <= 6) &&
		zend_get_parameters(ht, 3, &file, &args, &p_argc) == SUCCESS)
	{
		if ( args->type != IS_ARRAY )
		{ 
			php_error(E_WARNING, "2nd Variable passed to rrd_graph is not an array!\n");
			RETURN_FALSE;
		}
        
		convert_to_long(p_argc);
		convert_to_string(file);

		convert_to_array(args);
		args_arr = args->value.ht;

		argc = p_argc->value.lval + 3;
		argv = (char **) emalloc(argc * sizeof(char *));
 
		argv[0] = "dummy";
		argv[1] = estrdup("graph");
		argv[2] = estrdup(file->value.str.val);

		for (i = 3; i < argc; i++) 
		{
			pval **dataptr;

			if ( zend_hash_get_current_data(args_arr, (void *) &dataptr) == FAILURE )
				continue;

			entry = *dataptr;

			if ( entry->type != IS_STRING )
				convert_to_string(entry);

			argv[i] = estrdup(entry->value.str.val);

			if ( i < argc )
				zend_hash_move_forward(args_arr);
		}
   
		optind = 0; opterr = 0; 
		if ( rrd_graph(argc-1, &argv[1], &calcpr, &xsize, &ysize, NULL, &ymin, &ymax) != -1 )
		{
			array_init(return_value);
			add_assoc_long(return_value, "xsize", xsize);
			add_assoc_long(return_value, "ysize", ysize);
			add_assoc_double(return_value, "ymin", ymin);
			add_assoc_double(return_value, "ymax", ymax);

			MAKE_STD_ZVAL(p_calcpr);
			array_init(p_calcpr);
    
			if (calcpr)
			{
				for (i = 0; calcpr[i]; i++)
				{
					add_next_index_string(p_calcpr, calcpr[i], 1);
					free(calcpr[i]);
				}
				free(calcpr);
			}
			zend_hash_update(return_value->value.ht, "calcpr", sizeof("calcpr"), 
							(void *)&p_calcpr, sizeof(zval *), NULL);
		}
		else
		{
			RETVAL_FALSE;
		}
		for (i = 1; i < argc; i++)
			efree(argv[i]);

		efree(argv);
	}
	else
	{ 
		WRONG_PARAM_COUNT;
	}
	return;
}
/* }}} */

/* {{{ proto mixed rrd_fetch(string file, array args_arr, int p_argc)
	Fetch info from an RRD file */
PHP_FUNCTION(rrd_fetch)
{
	pval *file, *args, *p_argc;
	pval *entry;
	pval *p_start, *p_end, *p_step, *p_ds_cnt;
	HashTable *args_arr;
	zval *p_ds_namv, *p_data;
	int i, j, argc;
	time_t start, end;
	unsigned long step, ds_cnt;
	char **argv, **ds_namv; 
	rrd_value_t *data, *datap;
    
	if ( rrd_test_error() )
		rrd_clear_error();
    
	if ( ZEND_NUM_ARGS() == 3 && 
		 zend_get_parameters(ht, 3, &file, &args, &p_argc) == SUCCESS)
	{
		if ( args->type != IS_ARRAY )
		{ 
			php_error(E_WARNING, "2nd Variable passed to rrd_fetch is not an array!\n");
			RETURN_FALSE;
		}
        
		convert_to_long(p_argc);
		convert_to_string(file);

		convert_to_array(args);
		args_arr = args->value.ht;

		argc = p_argc->value.lval + 3;
		argv = (char **) emalloc(argc * sizeof(char *));
 
		argv[0] = "dummy";
		argv[1] = estrdup("fetch");
		argv[2] = estrdup(file->value.str.val);

		for (i = 3; i < argc; i++) 
		{
			pval **dataptr;

			if ( zend_hash_get_current_data(args_arr, (void *) &dataptr) == FAILURE )
				continue;

			entry = *dataptr;

			if ( entry->type != IS_STRING )
				convert_to_string(entry);

			argv[i] = estrdup(entry->value.str.val);

			if ( i < argc )
				zend_hash_move_forward(args_arr);
		}
  
		optind = 0; opterr = 0; 

		if ( rrd_fetch(argc-1, &argv[1], &start,&end,&step,&ds_cnt,&ds_namv,&data) != -1 )
		{
			array_init(return_value);
			add_assoc_long(return_value, "start", start);
			add_assoc_long(return_value, "end", end);
			add_assoc_long(return_value, "step", step);
			add_assoc_long(return_value, "ds_cnt", ds_cnt);

			MAKE_STD_ZVAL(p_ds_namv);
			MAKE_STD_ZVAL(p_data);
			array_init(p_ds_namv);
			array_init(p_data);
   
			if (ds_namv)
			{
				for (i = 0; i < ds_cnt; i++)
				{
					add_next_index_string(p_ds_namv, ds_namv[i], 1);
					free(ds_namv[i]);
				}
				free(ds_namv);
			}

			if (data)
			{
				datap = data;
 
				for (i = start + step; i <= end; i += step)
					for (j = 0; j < ds_cnt; j++)
						add_next_index_double(p_data, *(datap++));
 
				free(data);
			}

			zend_hash_update(return_value->value.ht, "ds_namv", sizeof("ds_namv"), 
							(void *)&p_ds_namv, sizeof(zval *), NULL);
			zend_hash_update(return_value->value.ht, "data", sizeof("data"), 
							(void *)&p_data, sizeof(zval *), NULL);
		}
		else
		{
			RETVAL_FALSE;
		}
		for (i = 1; i < argc; i++)
			efree(argv[i]);

		efree(argv);
	}
	else
	{ 
		WRONG_PARAM_COUNT;
	}
	return;
}
/* }}} */

/* {{{ proto string rrd_error(void)
	Get the error message set by the last rrd tool function call */
PHP_FUNCTION(rrd_error)
{
	char *msg;

	if ( rrd_test_error() )
	{
		msg = rrd_get_error();        

		RETVAL_STRING(msg, 1);
		rrd_clear_error();
	}
	else
		return;
}
/* }}} */

/* {{{ proto void rrd_clear_error(void)
	Clear the error set by the last rrd tool function call */
PHP_FUNCTION(rrd_clear_error)
{
	if ( rrd_test_error() )
		rrd_clear_error();

	return;
}
/* }}} */

/* {{{ proto int rrd_update(string file, string opt) 
	Update an RRD file with values specified */
PHP_FUNCTION(rrd_update)
{
	pval *file, *opt;
	char **argv;

	if ( rrd_test_error() )
		rrd_clear_error();

	if ( ZEND_NUM_ARGS() == 2 && 
		 zend_get_parameters(ht, 2, &file, &opt) == SUCCESS )
	{
		convert_to_string(file);
		convert_to_string(opt);

		argv = (char **) emalloc(4 * sizeof(char *));

		argv[0] = "dummy";
		argv[1] = estrdup("update");
		argv[2] = estrdup(file->value.str.val);
		argv[3] = estrdup(opt->value.str.val);

		optind = 0; opterr = 0;
		if ( rrd_update(3, &argv[1]) != -1 )
		{
			RETVAL_TRUE;
		}
		else
		{
			RETVAL_FALSE;
		}
		efree(argv[1]); efree(argv[2]); efree(argv[3]);
		efree(argv);
	}
	else
	{
		WRONG_PARAM_COUNT;
	}
	return;
}
/* }}} */

/* {{{ proto int rrd_last(string file)
	Gets last update time of an RRD file */
PHP_FUNCTION(rrd_last)
{
	pval *file;
	unsigned long retval;

	char **argv = (char **) emalloc(3 * sizeof(char *));
    
	if ( rrd_test_error() )
		rrd_clear_error();
    
	if (zend_get_parameters(ht, 1, &file) == SUCCESS)
	{
		convert_to_string(file);

		argv[0] = "dummy";
		argv[1] = estrdup("last");
		argv[2] = estrdup(file->value.str.val);

		optind = 0; opterr = 0;
		retval = rrd_last(2, &argv[1]);

		efree(argv[1]);  efree(argv[2]);
		efree(argv);
		RETVAL_LONG(retval);
	}
	else
	{
		WRONG_PARAM_COUNT;
	}
	return;
}
/* }}} */

/* {{{ proto int rrd_lastupdate(string file)
        Gets last update time and data of an RRD file */
PHP_FUNCTION(rrd_lastupdate)
{
        pval *file;
	zval *p_ds_namv,*p_last_ds;
        time_t    last_update;
        char    **ds_namv;
        char    **last_ds;
	int argc;
	unsigned long ds_cnt, i;
	char **argv = (char **) emalloc(3 * sizeof(char *));

        //char **argv = (char **) emalloc(3 * sizeof(char *));

        if ( rrd_test_error() )
                rrd_clear_error();

	if (zend_get_parameters(ht, 1, &file) == SUCCESS)
        {
		convert_to_string(file);

                argv[0] = "dummy";
                argv[1] = estrdup("last");
                argv[2] = estrdup(file->value.str.val);

		
                if ( rrd_lastupdate(2, &argv[1],&last_update,&ds_cnt, &ds_namv, &last_ds) !=-1 ) {
			array_init(return_value);
                        add_assoc_long(return_value, "time", last_update);
			
			MAKE_STD_ZVAL(p_ds_namv);
			MAKE_STD_ZVAL(p_last_ds);
			array_init(p_ds_namv);
			array_init(p_last_ds);

			if (ds_namv) {
				for (i = 0; i < ds_cnt; i++)
				{
					add_next_index_string(p_ds_namv, ds_namv[i], 1);
					free(ds_namv[i]);
				}
				free(ds_namv);
			}

			if (last_ds) {
                                for (i = 0; i < ds_cnt; i++)
                                {
                                        add_next_index_string(p_last_ds, last_ds[i], 1);
                                        free(last_ds[i]);
                                }
                                free(last_ds);

			}
			zend_hash_update(return_value->value.ht, "ds_name", sizeof("ds_namv"),
                                                        (void *)&p_ds_namv, sizeof(zval *), NULL);
			zend_hash_update(return_value->value.ht, "last_ds", sizeof("last_ds"),
                                                        (void *)&p_last_ds, sizeof(zval *), NULL);

		}
		efree(argv);

               // RETVAL_LONG(retval);
        }
        else
        {
                WRONG_PARAM_COUNT;
        }
        return;
}

/* }}} */

/* {{{ proto int rrd_create(string file, array args_arr, int argc)
	Create an RRD file with the options passed (passed via array) */ 
PHP_FUNCTION(rrd_create)
{
	pval *file, *args, *p_argc;
	pval *entry;
	char **argv;
	HashTable *args_arr;
	int argc, i;

	if ( rrd_test_error() )
		rrd_clear_error();

	if ( ZEND_NUM_ARGS() == 3 && 
		getParameters(ht, 3, &file, &args, &p_argc) == SUCCESS )
	{
		if ( args->type != IS_ARRAY )
		{ 
			php_error(E_WARNING, "2nd Variable passed to rrd_create is not an array!\n");
			RETURN_FALSE;
		}

		convert_to_long(p_argc);
		convert_to_string(file);
		
		convert_to_array(args);
		args_arr = args->value.ht;
		zend_hash_internal_pointer_reset(args_arr);

		argc = p_argc->value.lval + 3;
		argv = (char **) emalloc(argc * sizeof(char *));

		argv[0] = "dummy";
		argv[1] = estrdup("create");
		argv[2] = estrdup(file->value.str.val);

		for (i = 3; i < argc; i++) 
		{
			pval **dataptr;

			if ( zend_hash_get_current_data(args_arr, (void *) &dataptr) == FAILURE )
				continue;

			entry = *dataptr;

			if ( entry->type != IS_STRING )
				convert_to_string(entry);

			argv[i] = estrdup(entry->value.str.val);

			if ( i < argc )
				zend_hash_move_forward(args_arr);
		}
  
		optind = 0;  opterr = 0;

		if ( rrd_create(argc-1, &argv[1]) != -1 )
		{
			RETVAL_TRUE;
		}
		else
		{
			RETVAL_FALSE;
		}
		for (i = 1; i < argc; i++)
			efree(argv[i]);

		efree(argv);
	}
	else
	{
	    WRONG_PARAM_COUNT;
	}
	return;
}
/* }}} */

PHP_FUNCTION(rrdtool_info)
{

	if (ZEND_NUM_ARGS()!=0) {
		ZEND_WRONG_PARAM_COUNT();
		RETURN_FALSE;
	}

	PUTS("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"DTD/xhtml1-transitional.dtd\">\n");
	PUTS("<html>");
	PUTS("<head>\n");
	PUTS("<style type=\"text/css\"><!--");
	PUTS("body {background-color: #ffffff; color: #000000;}");
	PUTS("body, td, th, h1, h2 {font-family: sans-serif;}");
	PUTS("pre {margin: 0px; font-family: monospace;}");
	PUTS("a:link {color: #000099; text-decoration: none; background-color: #ffffff;}");
	PUTS("a:hover {text-decoration: underline;}");
	PUTS("table {border-collapse: collapse;}");
	PUTS(".center {text-align: center;}");
	PUTS(".center table { margin-left: auto; margin-right: auto; text-align: left;}");
	PUTS(".center th { text-align: center !important; }");
	PUTS("td, th { border: 1px solid #000000; font-size: 75%; vertical-align: baseline;}");
	PUTS("h1 {font-size: 150%;}");
	PUTS("h2 {font-size: 125%;}");
	PUTS(".p {text-align: left;}");
	PUTS(".e {background-color: #ccccff; font-weight: bold; color: #000000;}");
	PUTS(".h {background-color: #9999cc; font-weight: bold; color: #000000;}");
	PUTS(".v {background-color: #cccccc; color: #000000;}");
	PUTS("i {color: #666666; background-color: #cccccc;}");
	PUTS("img {float: right; border: 0px;}");
	PUTS("hr {width: 600px; background-color: #cccccc; border: 0px; height: 1px; color: #000000;}");
	PUTS("//--></style>");
	PUTS("<title>rrdtool_info()</title>");
	PUTS("</head>\n");
	PUTS("<body><div class=\"center\">\n");

	php_info_print_box_start(1);
	PUTS("<a href=\"http://people.ee.ethz.ch/~oetiker/webtools/rrdtool/\" target=\"rrdtool\"><img border=\"0\" src=\"");
	if (SG(request_info).request_uri) {
		PUTS(SG(request_info).request_uri);
	}
	PUTS("?="RRDTOOL_LOGO_GUID"\" alt=\"ClamAV logo\" /></a>\n");
	php_printf("<h1 class=\"p\">rrdtool Version %s</h1>\n", PHP_RRD_VERSION_STRING);
	php_info_print_box_end();
	php_info_print_table_start();
	php_info_print_table_row(2, "System", PHP_UNAME );
	php_info_print_table_row(2, "Build Date", __DATE__ " " __TIME__ );
	php_info_print_table_row(2, "rrdtool Support","Enabled");
	php_info_print_table_end();

	PUTS("<h2>RRDTOOL Copyright</h2>\n");
	php_info_print_box_start(0);
	PUTS("COPYRIGHT STATEMENT FOLLOWS THIS LINE</p>\n<blockquote>\n");
	PUTS("<p>Portions copyright 2005 by Dale Walsh (buildsmart@daleenterprise.com).</p>\n");
	PUTS("<p>Portions relating to rrdtool 1999, 2000, 2001, 2002, 2003, 2004, 2005 by Tobias Oetiker.</p>\n");
	php_info_print_box_end();
	PUTS("<h2>RRDTOOL License</h2>\n");
	php_info_print_box_start(0);
	PUTS("<p><b>Permission has been granted to copy, distribute and modify rrd in any context without fee, including a commercial application, provided that this notice is present in user-accessible supporting documentation. </b></p>");
	PUTS("<p>This does not affect your ownership of the derived work itself, and the intent is to assure proper credit for the authors of rrdtool, not to interfere with your productive use of rrdtool. If you have questions, ask. \"Derived works\" ");
	PUTS("includes all programs that utilize the library. Credit must be given in user-accessible documentation.</p>\n");
	PUTS("<p><b>This software is provided \"AS IS.\"</b> The copyright holders disclaim all warranties, either express or implied, including but not limited to implied warranties of merchantability and fitness for a particular purpose, ");
	PUTS("with respect to this code and accompanying documentation.</p>\n");
	php_info_print_box_end();
	PUTS("<h2>Special Thanks</h2>\n");
	php_info_print_box_start(0);
	PUTS("<p>Perl by Larry Wall");
	PUTS("<p>gd library by Thomas Boutell");
	PUTS("<p>gifcode from David Koblas");
	PUTS("<p>libpng by Glenn Randers-Pehrson / Andreas Eric Dilger / Guy Eric Schalnat");
	PUTS("<p>cgilib by Martin Schulze");
	PUTS("<p>zlib by Jean-loup Gailly and Mark Adler");
	PUTS("<p>Portions relating to php4 and php5 bindings, Dale Walsh (buildsmart@daleenterprise.com)");
	php_info_print_box_end();

	PUTS("</div></body></html>");
}
/* }}} */

PHP_FUNCTION(rrdtool_logo_guid)
{
	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	RETURN_STRINGL(RRDTOOL_LOGO_GUID, sizeof(RRDTOOL_LOGO_GUID)-1, 1);
}
/* }}} */

#endif	/* HAVE_RRDTOOL */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
