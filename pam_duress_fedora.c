#include <syslog.h>
#include <security/pam_modules.h>
#include <security/pam_ext.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <crypt.h>
#include <unistd.h>

#define DURESS_DIR_TEMPLATE "/home/%s/.duress"

void execute_duress_actions(const char *username, char *output) {
    char command[256];
    FILE *fp;
    char temp_output[256];

    snprintf(command, sizeof(command), DURESS_DIR_TEMPLATE "/duress_action.sh", username);

    fp = popen(command, "r");
    if (fp == NULL) {
        strcpy(output, "Error executing file.");
        return;
    }

    if (fgets(temp_output, sizeof(temp_output), fp) != NULL) {
        snprintf(output, 256, "File executed with result: %s", temp_output);
    } else {
        strcpy(output, "No output from command.");
    }
    
    pclose(fp);
}

void hash_password(const char *password, char *hashed_password) {
    FILE *fp;
    char command[256];

    snprintf(command, sizeof(command), "echo -n '%s' | sha256sum | awk '{print $1}'", password);
    
    fp = popen(command, "r");
    if (fp == NULL) {
        strcpy(hashed_password, "");
        return;
    }

    fgets(hashed_password, 64, fp);
    pclose(fp);
}

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    const char *password;
    char *stored_hash;
    char output[256] = {0};
    char hash_file[256];
    FILE *hash_file_handle;
    char correct_hashed_password[64];
    char input_hashed_password[64];
    char password_file[256];
    const char *username;
    int retval;

    openlog("pam_duress", LOG_PID | LOG_CONS, LOG_AUTH);
    
    retval = pam_get_user(pamh, &username, NULL);
    if (retval != PAM_SUCCESS) {
        return retval;
    }
    
    retval = pam_get_item(pamh, PAM_AUTHTOK, (const void **)&password);
    if (retval != PAM_SUCCESS) {
        return retval;
    }
    
    char duress_dir[256];
    snprintf(duress_dir, sizeof(duress_dir), DURESS_DIR_TEMPLATE, username);

    snprintf(hash_file, sizeof(hash_file), "%s/password_hash", duress_dir);
    hash_file_handle = fopen(hash_file, "r");
    if (hash_file_handle == NULL) {
        return PAM_SYSTEM_ERR;
    }  
    if (fgets(correct_hashed_password, sizeof(correct_hashed_password), hash_file_handle) == NULL) {
        fclose(hash_file_handle);
        return PAM_SYSTEM_ERR;
    }
    fclose(hash_file_handle);

    hash_password(password, input_hashed_password);
    
    if (strcmp(input_hashed_password, correct_hashed_password) == 0) {
        syslog(LOG_INFO, "Duress password activated!");
        execute_duress_actions(username,output);
        return PAM_SUCCESS;
    }

    return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return PAM_SUCCESS;
}
