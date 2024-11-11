/*
 * mw-rf-init application
 *
 * Initialize RFTOOL with parameters read from cfg file at boot-time
 *
 */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define TXT_LINE_SIZE 4096
#define RPLY_LINE_SIZE 4096


typedef struct {
    int socket_desc_ctrl;
    int socket_desc_data;
    struct sockaddr_in rftool_ctrl;
    struct sockaddr_in rftool_data;
} socketStruct ;

char logbuf[RPLY_LINE_SIZE + 100] = {0};

// declarations
int setupComms(socketStruct *socketInput, FILE * fhlog);
void writeToLogAndTerminal(const char * inputMsg, FILE* fh);
void printToTerminal(const char * str);

void printToTerminal(const char * str) {
    fprintf(stdout, "%s", str);
}

int main()
{
    int status = 0;
    socketStruct rftool_socket;
    char rftool_reply[RPLY_LINE_SIZE + 1];
    FILE* fh = NULL;
    FILE* fhlog = NULL;
    char textbuf[TXT_LINE_SIZE] = {0};
    char blank_line[] = "\n";
    char pause_line[] = "PAUSE\n";
    char CONFIG_FILE_LOC[] = "/mnt/hdlcoder_rd/RF_Init.cfg";
    char RF_INIT_LOG_LOC[] = "/mnt/rf_init.log";
    memset(rftool_reply, '\0', sizeof(rftool_reply));

    fhlog = fopen(RF_INIT_LOG_LOC,"w");
    if (fhlog == NULL)
    {
        sprintf(logbuf,"rf_init: Could not open log file %s ! Exiting...\n", RF_INIT_LOG_LOC);
        printToTerminal(logbuf);
        goto TERM_ERR;
    }
    
    fh = fopen(CONFIG_FILE_LOC,"r");
    if (fh == NULL)
    {
        sprintf(logbuf,"rf_init: Could not locate %s ! Exiting...\n",CONFIG_FILE_LOC);
        writeToLogAndTerminal(logbuf,fhlog);
        goto TERM_ERR;
    }

    if (setupComms(&rftool_socket,fhlog) < 0)
    {
        sprintf(logbuf,"rf_init: Could not connect to RFTOOL. Exiting...\n");
        perror(logbuf);
        writeToLogAndTerminal(logbuf,fhlog);
        goto TERM_ERR;
    }

    while (!feof(fh))
    {
        if (fgets(textbuf, sizeof(textbuf), fh) != NULL) {
            int compare2 = strncmp(textbuf,blank_line, 2);
            int compare3 = strcmp(textbuf,pause_line);
            if (compare2==0)
            {
                const char * skipMsg = "-----SKIP BLANK LINE-----..\n";
                writeToLogAndTerminal(skipMsg, fhlog);
            }
            else if (compare3 == 0)
            {
                const char * pauseMsg = "rf_init: PAUSING \n";
                sleep(1);
                writeToLogAndTerminal(pauseMsg, fhlog);
            }
            else
            {
                sprintf(logbuf,"rf_init: SENDING: %s",textbuf);
                writeToLogAndTerminal(logbuf, fhlog);
                if ( send(rftool_socket.socket_desc_ctrl,
                        textbuf,
                        strlen(textbuf),
                        MSG_CONFIRM) < 0 )
                {
                    const char * errMsg = "rf_init: Failed to send command to RFTOOL - EXITING!";
                    perror(errMsg);
                    writeToLogAndTerminal(errMsg, fhlog);
                    goto TERM_ERR;
                } else {
                    const char * waitMsg = "rf_init: Waiting for response...\n"; 
                    printToTerminal(waitMsg);
                    do
                    {   //empty buffer
                        status = recv(rftool_socket.socket_desc_ctrl,rftool_reply,RPLY_LINE_SIZE,0);
                        //sprintf(logbuf, "rf_init: STATUS: %d\n", status);
                        //writeToLogAndTerminal(logbuf, fhlog);
                        if ((status > 0) && (status != 2))  // != 2 is to ignore RFTool's \r\n or \n\r output
                        {
                            sprintf(logbuf,"rf_init: RECEIVED: %.*s\n",status,rftool_reply);
                            writeToLogAndTerminal(logbuf,fhlog);
                        }
                    } while ((status > 0)  && (status == 2));  // Wait until we get a valid response
                    // If we timed out, -1 will be returned other TCP errors return < 0 too
                    if (status < 0) {
                        const char * errMsg = "rf_init: Failed to receive command from RFTOOL - EXITING!";
                        perror(errMsg);
                        writeToLogAndTerminal(errMsg, fhlog);
                        goto TERM_ERR;
                    }
                }
            }
        }
    }

    const char * happyMsg =  "rf_init: finished initializing rftool\n";
    writeToLogAndTerminal(happyMsg, fhlog);
    fflush(fh); fclose(fh);
    fflush(fhlog); fclose(fhlog);
    return(0);

TERM_ERR:
    {
        const char * termErrMsg = "rf_init: Terminating error!\n";
        if (fh != NULL) { fflush(fh); fclose(fh); }
        if (fhlog != NULL) {
            writeToLogAndTerminal(termErrMsg, fhlog);
            fflush(fhlog); fclose(fhlog);
        } else
            printToTerminal("termErrMsg");

        return(-1);
    }
}

int setupComms(socketStruct *socketInput, FILE * fhlog)
{
    struct timeval tv;
    tv.tv_sec = 12;
    tv.tv_usec = 0;
    int MaxRetry = 20;
    int err = 0;

    //Create socket
    socketInput->socket_desc_ctrl = socket(AF_INET , SOCK_STREAM , 0);
    socketInput->socket_desc_data = socket(AF_INET , SOCK_STREAM , 0);
    if (socketInput->socket_desc_ctrl == -1)
    {
        sprintf(logbuf,"rf_init: Could not create socket");
        writeToLogAndTerminal(logbuf,fhlog);
    }

    socketInput->rftool_ctrl.sin_addr.s_addr = inet_addr("127.0.0.1");
    socketInput->rftool_ctrl.sin_family = AF_INET;
    socketInput->rftool_ctrl.sin_port = htons( 8081 );

    socketInput->rftool_data.sin_addr.s_addr = inet_addr("127.0.0.1");
    socketInput->rftool_data.sin_family = AF_INET;
    socketInput->rftool_data.sin_port = htons( 8082 );

    //Connect to data plane
    int connCount = 0;
    do {
        err = connect(socketInput->socket_desc_data,
        (struct sockaddr *)&socketInput->rftool_data,
        sizeof(socketInput->rftool_data));
        if (err<0)
        {
            connCount++;
            sprintf(logbuf,"rf_init: Could not connect to RFTOOL data...Iteration:%d \n",connCount);
            writeToLogAndTerminal(logbuf,fhlog);
            sleep(1);
        }
    }
    while(err<0 && connCount<MaxRetry);
    
    if (err<0 && connCount>=MaxRetry)
    {
        sprintf(logbuf,"rf_init: Failed to establish data connection after %d retries \n",connCount);
        writeToLogAndTerminal(logbuf,fhlog);
        return(-1);
    }
    else
    {
        sprintf(logbuf,"rf_init: Connected to data plane \n");
        writeToLogAndTerminal(logbuf,fhlog);
    }

    //Connect to control plane
    connCount = 0;
    do {
        err = connect(socketInput->socket_desc_ctrl ,
        (struct sockaddr *)&socketInput->rftool_ctrl ,
        sizeof(socketInput->rftool_ctrl));
        if (err<0)
        {
            connCount++;
            sprintf(logbuf,"rf_init: Could not connect to RFTOOL command...Iteration:%d \n",connCount);
            writeToLogAndTerminal(logbuf,fhlog);
            sleep(1);
        }
    }
    while(err<0 && connCount<MaxRetry);

    if (err<0 && connCount>=MaxRetry)
    {
        sprintf(logbuf,"rf_init: Failed to establish command connection after %d retries \n",connCount);
        writeToLogAndTerminal(logbuf,fhlog);
        return(-1);
    }
    else
    {
        sprintf(logbuf,"rf_init: Connected to command plane \n");
        writeToLogAndTerminal(logbuf,fhlog);
    }


    // Apply timeout
    setsockopt(socketInput->socket_desc_ctrl, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    setsockopt(socketInput->socket_desc_data, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    return(err);

}

void writeToLogAndTerminal(const char * inputMsg, FILE* fh)
{
    fprintf(stdout, "%s", inputMsg);
    if (fh != NULL) {
        fwrite(inputMsg,strlen(inputMsg),1,fh);
    } else {
        printToTerminal("rf_init: writing to invalid log file!\n");
    }
    fflush(stdout); fflush(stderr);
}