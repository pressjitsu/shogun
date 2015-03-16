/**
 * Shogun PHP Malware Analysis Toolkit
 *
 * Copyright (C) 2015 Pressjitsu - All Rights Reserved
 * Proprietary and confidential
 * Unauthorized copying of this file, via any medium is strictly prohibited
 *
 * Written by Gennady Kovshenin <gennady@kovshenin.com>
 */

#include <stdio.h>

#include "SAPI.h"

static int php_shogun_startup(sapi_module_struct *sapi_module)
{
	if (php_module_startup(sapi_module, NULL, 0) == FAILURE)
		return FAILURE;
	return SUCCESS;
}

static sapi_module_struct shogun_sapi_module = {
	"shogun",						/* name */
	"Shogun",						/* pretty name */

	php_shogun_startup,				/* startup */
	NULL,							/* shutdown */

	NULL,							/* activate */
	NULL,							/* deactivate */

	NULL,					    	/* unbuffered write */
	NULL,						    /* flush */
	NULL,							/* get uid */
	NULL,							/* getenv */

	NULL,							/* error handler */

	NULL,							/* header handler */
	NULL,							/* send headers handler */
	NULL,							/* send header handler */

	NULL,				            /* read POST data */
	NULL,							/* read Cookies */

	NULL,							/* register server variables */
	NULL,							/* Log message */
	NULL,							/* Get request time */
	NULL,							/* Child terminate */
	
	STANDARD_SAPI_MODULE_PROPERTIES
};

void print_zval(zval *z) {
	switch (z->type) {
		case IS_NULL:
			printf("null");
			break;
		case IS_LONG:
			printf("%ld", z->value.lval);
			break;
		case IS_DOUBLE:
			printf("%f", z->value.dval);
			break;
		case IS_BOOL:
			printf("%s", z->value.lval ? "true" : "false");
			break;
		case IS_ARRAY:
			printf("array[%d]", z->value.ht->nNumOfElements);
			break;
		/*
		#define IS_OBJECT   5
		*/
		case IS_STRING:
			printf("%s", z->value.str);
			break;
		/*
		#define IS_RESOURCE 7
		#define IS_CONSTANT 8
		#define IS_CONSTANT_AST 9
		#define IS_CALLABLE 10
		*/
		default:
			printf("unknown (%d)", z->type);
	}
}

void print_type(uint t) {
	switch (t) {
		case IS_NULL:
			printf("null");
			break;
		case IS_LONG:
			printf("int");
			break;
		case IS_DOUBLE:
			printf("float");
			break;
		case IS_BOOL:
			printf("bool");
		case IS_ARRAY:
			printf("array");
			break;
		case IS_OBJECT:
			printf("object");
			break;
		case IS_STRING:
			printf("string");
			break;
		case IS_RESOURCE:
			printf("resource");
			break;
		default:
			printf("unknown (%d)", t);
			break;
	}
}

int main(int argc, char *argv[])
{
	printf("Shogun PHP! Ossss!\n");
	printf("==================\n\n");

	if (argc < 2) {
		printf("Need PHP file as argument!\n");
	}

	sapi_startup(&shogun_sapi_module);
	if (shogun_sapi_module.startup(&shogun_sapi_module) == FAILURE) {
		return -1;
	}

	zend_activate(TSRMLS_C);

	zval *filename;
	ALLOC_INIT_ZVAL(filename);
	ZVAL_STRING(filename, argv[1], 1);

	printf("Running: ");
	zend_print_zval(filename, 0);
	printf("\n");

	/** Compile the file to reveal its full opcode set */
	zend_op_array *op = compile_filename(ZEND_REQUIRE, filename TSRMLS_CC);

	int i;
	printf("Variables: %d\n", op->last_var);
	for (i = 0; i < op->last_var; i++) {
		printf("\t$%s\n", op->vars[i].name);
	}

	printf("Literals: %d\n", op->last_literal);
	for (i = 0; i < op->last_literal; i++) {
		printf("\t");
		print_zval(&op->literals[i].constant);
		printf("\n");
	}

	printf("Executing across %d opcodes\n", op->last);

	/** Execute */
	EG(active_op_array) = op;
	EG(in_execution) = 1;

	zend_execute_data *execute_data = zend_create_execute_data_from_op_array(op, 0 TSRMLS_CC);

	int ret = 0;
	while (1) {
		/** Opcode */
		printf("\t%s:", zend_get_opcode_name(execute_data->opline->opcode));

		/** Extended value */
		if (execute_data->opline->extended_value) {
			switch (execute_data->opline->opcode) {
				case ZEND_CAST:
					printf(" as ");
					print_type(execute_data->opline->extended_value);
					break;
				case ZEND_RETURN:
					break;
				default:
					printf(" unknown extended value");
					break;
			}
		}

		/** Operands */
		int t = 1;
		while (t < 3) {
			int operand_type = (t == 1) ? execute_data->opline->op1_type : execute_data->opline->op2_type;
			znode_op operand = (t++ == 1) ? execute_data->opline->op1 : execute_data->opline->op2;

			printf(" ");
			switch (operand_type) {
				case IS_CONST:
					print_zval(&operand.literal->constant);
					break;
				case IS_TMP_VAR:
					print_zval(&EX_TMP_VAR(execute_data, operand.var)->tmp_var);
					break;
				case IS_VAR:
					zend_print_variable(*EX_TMP_VAR(execute_data, operand.var)->var.ptr_ptr);
					break;
				case IS_UNUSED:
					break;
				case IS_CV:
					printf("$%s", op->vars[operand.var].name);
					break;
				default:
					printf("unknown (%d)", operand_type);
			}
		}

		printf("\n");

		/** Step */
		ret = execute_data->opline->handler(execute_data TSRMLS_CC);

		if (ret == 2) { /** Nesting context */
			printf(">> ");
			execute_data = zend_create_execute_data_from_op_array(EG(active_op_array), 1 TSRMLS_CC);
		} else if (ret == 3) { /** Unnesting context */
			execute_data = EG(current_execute_data);
			printf("<< ");
		} else if (ret != 0) {
			printf("= %d\n", ret);
			break;
		}
	}


	return 0;
}
