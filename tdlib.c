#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_tdlib.h"
#include "ext/json/php_json.h"
#include "zend_exceptions.h"
#include "ext/standard/info.h"
#include "td/telegram/td_json_client.h"

zend_class_entry *tdlib_ce;

static PHP_MINFO_FUNCTION(tdlib);
static PHP_MINIT_FUNCTION(tdlib);
static PHP_MSHUTDOWN_FUNCTION(tdlib);

static PHP_METHOD(TdLib, __construct);
static PHP_METHOD(TdLib, __destruct);
static PHP_METHOD(TdLib, receive);
static PHP_METHOD(TdLib, send);
static PHP_METHOD(TdLib, execute);

ZEND_BEGIN_ARG_INFO_EX(arginfo_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_receive, 0, 1, IS_ARRAY, 0)
                ZEND_ARG_TYPE_INFO(0, timeout, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_send, 0, 1, IS_VOID, 0)
                ZEND_ARG_INFO(0, request)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_execute, 0, 1, IS_ARRAY, 0)
                ZEND_ARG_INFO(0, request)
ZEND_END_ARG_INFO()

static const zend_function_entry functions[] = {
        PHP_ME(TdLib, __construct, arginfo_void, ZEND_ACC_PUBLIC)
        PHP_ME(TdLib, __destruct, arginfo_void, ZEND_ACC_PUBLIC)
        PHP_ME(TdLib, receive, arginfo_receive, ZEND_ACC_PUBLIC)
        PHP_ME(TdLib, send, arginfo_send, ZEND_ACC_PUBLIC)
        PHP_ME(TdLib, execute, arginfo_execute, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
        PHP_FE_END
};

static const zend_module_dep tdlib_deps[] = {
        ZEND_MOD_REQUIRED("json")
        ZEND_MOD_END
};

zend_module_entry tdlib_module_entry = {
        STANDARD_MODULE_HEADER_EX,
        NULL,
        tdlib_deps,
        PHP_TDLIB_EXTNAME,
        NULL,
        PHP_MINIT(tdlib),
        PHP_MSHUTDOWN(tdlib),
        NULL,
        NULL,
        PHP_MINFO(tdlib),
        PHP_TDLIB_VERSION,
        STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_TDLIB
ZEND_GET_MODULE(tdlib)
#endif

zend_object_handlers tdlib_object_handlers;

static zend_always_inline void *tdlib_client_get_instance(zval *id) {
    tdlib_object *tdlib;

    if (Z_TYPE_P(id) == IS_OBJECT) {
        tdlib = TDLIB_ZVAL_GET_OBJECT(tdlib_object, id);
        if (tdlib->client) {
            return tdlib->client;
        }
    }

    return NULL;
}

void free_tdlib_object(zend_object *object) {
    tdlib_object *tdlib = TDLIB_GET_OBJECT(tdlib_object, object);

    zend_object_std_dtor(&tdlib->std);
    if (tdlib->client) {
        td_json_client_destroy(tdlib->client);
    }
}

zend_object *create_tdlib_object(zend_class_entry *ce) {
    tdlib_object *tdlib = ecalloc(1, sizeof(tdlib_object) + zend_object_properties_size(ce));

    tdlib->client = td_json_client_create();

    zend_object_std_init(&tdlib->std, ce);
    object_properties_init(&tdlib->std, ce);

    memcpy(&tdlib_object_handlers, zend_get_std_object_handlers(), sizeof(tdlib_object_handlers));
    tdlib_object_handlers.offset = XtOffsetOf(tdlib_object, std);
    tdlib_object_handlers.free_obj = free_tdlib_object;
    tdlib->std.handlers = &tdlib_object_handlers;

    return &tdlib->std;
}

static PHP_MINFO_FUNCTION(tdlib)
{
    php_info_print_table_start();
    php_info_print_table_row(2, "TdLib", "enabled");
    php_info_print_table_row(2, "Version", PHP_TDLIB_VERSION);
    php_info_print_table_end();
}

/**
 * PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION (tdlib) {
    zend_class_entry tdlib_class_entry;

    /* TdLib class */
    INIT_CLASS_ENTRY(tdlib_class_entry, "TdLib", functions);
    tdlib_ce = zend_register_internal_class(&tdlib_class_entry);
    tdlib_ce->create_object = create_tdlib_object;

    return SUCCESS;
}

/**
 * PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION (tdlib) {
    return SUCCESS;
}

/* {{{ proto TdLib TdLib::__construct()
    Public constructor */
PHP_METHOD (TdLib, __construct) {
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "") == FAILURE) {
        RETURN_FALSE;
    }
}
/* }}} */

/* {{{ proto TdLib TdLib::__destruct()
    Public Destructor */
PHP_METHOD (TdLib, __destruct) {
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "") == FAILURE) {
        RETURN_FALSE;
    }
}

PHP_METHOD (TdLib, receive) {
    void *tdClient = tdlib_client_get_instance(getThis());
    double timeout;

    ZEND_PARSE_PARAMETERS_START(0, 1)
            Z_PARAM_DOUBLE(timeout);
    ZEND_PARSE_PARAMETERS_END();

    if (tdClient) {
        const char *response = td_json_client_receive(tdClient, timeout);

        if (!response) {
            RETURN_NULL();
        }

        php_json_decode(return_value, (char *) response, strlen(response), 1, PHP_JSON_PARSER_DEFAULT_DEPTH);

        return;
    }

    RETURN_NULL();
}

PHP_METHOD (TdLib, send) {
    void *tdClient = tdlib_client_get_instance(getThis());

    zval *parameter;
    smart_str buf = {0};

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_ZVAL(parameter)
    ZEND_PARSE_PARAMETERS_END();

    if (tdClient) {
        php_json_encode(&buf, parameter, 0);

        td_json_client_send(tdClient, ZSTR_VAL(buf.s));
    }
}

PHP_METHOD (TdLib, execute) {
    zval *parameter;
    smart_str buf = {0};

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_ZVAL(parameter)
    ZEND_PARSE_PARAMETERS_END();


    php_json_encode(&buf, parameter, 0);

    const char *response = td_json_client_execute(NULL, ZSTR_VAL(buf.s));

    if (!response) {
        RETURN_NULL();
    }

    php_json_decode(return_value, (char *) response, strlen(response), 1, PHP_JSON_PARSER_DEFAULT_DEPTH);
}
