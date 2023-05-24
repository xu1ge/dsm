/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "rpc_dsm.h"
#include "../../config.h"
#include "../../main/window.h"
extern window *win[];

int *
write_data_1_svc(para_1 *argp, struct svc_req *rqstp)
{
	static int  result;

	/*
	 * insert server code here
	 */
	result = windowDataIn(win[argp->node], argp->buff, argp->len);
	INFO("write_data_remote, node = %d, len = %d, buff = %c",argp->node, argp->len, (argp->buff)[0]);
	return &result;
}

int *
read_data_1_svc(para_2 *argp, struct svc_req *rqstp)
{
	static int  result;

	/*
	 * insert server code here
	 */
	INFO("read_data_1_svc, win[%d]->third = %d", argp->node, win[argp->node]->third);
	result = 0;
	return &result;
}

int *
delete_data_1_svc(para_3 *argp, struct svc_req *rqstp)
{
	static int  result;

	/*
	 * insert server code here
	 */

	return &result;
}

int *
recover_data_1_svc(PARA_4 *argp, struct svc_req *rqstp)
{
	static int  result;

	/*
	 * insert server code here
	 */

	return &result;
}