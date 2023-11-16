#include <stdio.h> 
#include "macros.h"
#include "app.h"

int main(int argc, char *argv[]) {
    if(argc != 2) {                         //<user>:<password>@<host>:<port>/<url-path>
        printf("Usage: %s %s \n", argv[0], "ftp://<user>:<password>@<host>/<url-path>");
        return -1;
    }
       
    printf("\n--------- Verifying FTP parameters ---------\n\n");
    // parse command line arguments
    struct FTPparameters params;
    if(parseArguments(&params, argv[1]) != 0) {
        return -1;
    }

    printf("User: %s\n", params.user);
    printf("Password: %s\n", params.password);
    printf("Host name: %s\n", params.host_name);
    printf("File path: %s\n", params.file_path);
    printf("File name: %s\n", params.file_name);
    printf("\n--------- FTP parameters verified ----------\n\n");


    struct FTP ftp;
    char reply[MAX_LENGTH];    // buffer to read the reply from commands

    // get IP Address
    char ipAddress[MAX_LENGTH];
    if (getIPAddress(ipAddress, params.host_name) < 0) {
        return -1;
    }

    printf("\n----- Connecting to new control Socket -----\n\n");
    // create and connect socket to server
    if ((ftp.control_socket_fd = openAndConnectSocket(ipAddress, FTP_PORT_NUMBER)) < 0) {
        printf("> Error opening new socket\n");
        return -1;
    }
    printf("\n----- Connected to new control Socket ------\n\n");

    // receive confirmation from server
    readReplyFromControlSocket(&ftp, reply);
    if (reply[0] != '2') { // Error
        printf("> Error in conection...\n\n");
        return -1;
    }

    printf("\n------- Logging in to the FTP server -------\n\n");
    // login in the server
    if (login(&ftp, params.user, params.password)<0) {
        printf("> Login failed...\n\n");
        return -1;
    }
    printf("\n---------- Logged in successfully ----------\n\n");



    // change working directory in server, if needed
    if (strlen(params.file_path) > 0) {
        printf("\n------- Chaning working directory ------\n\n");
        if (changeWorkingDirectory(&ftp, params.file_path) < 0)
        {
            printf("> Error while changing working directory\n");
            return -1;
        }
        printf("\n---- Directory changed successfully ----\n\n");
    }
    
    printf("\n---------- Enabling Passive Mode -----------\n\n");
    // send pasv command to get ip address and port
    // and create the data socket for the file transfer
    if (enablePassiveMode(&ftp) < 0){
        printf("> Error while enabling passive mode\n");
        return -1;
    }
    printf("\n--------- Passive Mode was enabled ---------\n\n");

    return 0;
}
