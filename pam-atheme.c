#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/client.h>

#include "config.h"

#define NAME "Atheme/PAM Identify Validator"
#define VERSION "1.0"

#define XMLRPC_CLEANUP xmlrpc_env_clean(&env); xmlrpc_client_cleanup();

static int evaluate_xmlrpc_auth(const char *user, const char *password)
{
	xmlrpc_env env;
	xmlrpc_value *res;

	xmlrpc_env_init(&env);
	xmlrpc_client_init2(&env, XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION,
		NULL, 0);
	if (env.fault_occurred)
	{
		fprintf(stderr, "Atheme/PAM Error: %s (%d)\n",
			env.fault_string, env.fault_code);
		XMLRPC_CLEANUP;
		return PAM_SERVICE_ERR;
	}

	res = xmlrpc_client_call(&env, NETURL, "atheme.login", 
		"(sss)", user, password, "::1");
	if (env.fault_occurred)
	{
		XMLRPC_CLEANUP;
		return PAM_AUTH_ERR;
	}

	XMLRPC_CLEANUP;
	return PAM_SUCCESS;
}

/* required in a PAM module but useless for this product */
PAM_EXTERN int pam_sm_setcred( pam_handle_t *pamh, int flags, int argc, const char **argv )
{
	return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
	printf("Acct mgmt\n");
	return PAM_SUCCESS;
}

/* expected hook, this is where custom stuff happens */
PAM_EXTERN int pam_sm_authenticate( pam_handle_t *pamh, int flags,int argc, const char **argv )
{
	struct pam_conv *conv;
	struct pam_message msg;
	const struct pam_message *msgp;
	struct pam_response *resp;
	struct passwd *pwd;
	const char *user;
	char *crypt_password, *password;
	int pam_err, retry;
/* identify user */
	if ((pam_err = pam_get_user(pamh, &user, NULL)) != PAM_SUCCESS)
		return (pam_err);

	/* get password */
	pam_err = pam_get_item(pamh, PAM_CONV, (const void **)&conv);
	if (pam_err != PAM_SUCCESS)
		return (PAM_SYSTEM_ERR);
	msg.msg_style = PAM_PROMPT_ECHO_OFF;
	msg.msg = NETNAME " Password: ";
	msgp = &msg;
	for (retry = 0; retry < 3; ++retry)
	{
		resp = NULL;
		pam_err = (*conv->conv)(1, &msgp, &resp, conv->appdata_ptr);
		if (resp != NULL)
		{
			if (pam_err == PAM_SUCCESS)
				password = resp->resp;
			else
				free(resp->resp);
			free(resp);
		}
		if (pam_err == PAM_SUCCESS)
			break;
	}
	if (pam_err == PAM_CONV_ERR)
		return (pam_err);
	if (pam_err != PAM_SUCCESS)
		return (PAM_AUTH_ERR);
    
    return evaluate_xmlrpc_auth(user, password);
}
