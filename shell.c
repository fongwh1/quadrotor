#include "string-util.h"
#include "module/module_nrf24l01.h"

#include "sensor.h"
#define MAX_COMMAND_LEN 30
#define BACKSPACE 0x7f
#define ESC 0x1b
#define BUF_SIZE 50
#define MAX_ARGV 10

void hello_func(int argc, char *argv[]);
void help_func(int argc, char *argv[]);
void check_func(int argc, char *argv[]);
void receive_func(int argc, char *argv[]);
void send_func(int argc, char *argv[]);
void sensor_func(int argc, char *argv[]);

extern char receive_byte();
extern char non_block_receive_byte();

u8 rec_word[10];
u8 send_word[10];

enum {
	HELP = 0,
	HELLO,
	CHECK,
	RECEIVE,
	SEND,
	SENSOR,
	MAX_COMMANDS
};

typedef struct{
	char *name;
	char *description;
	void (*function)(int, char**); 
} shell_cmd;

shell_cmd commands[] = {
	{
		.name = "help",
		.description = "show the introduction to the commands",
		.function = help_func
	},
	{
		.name = "hello",
		.description = "saying hello",
		.function = hello_func
	},
	{
		.name = "check",
		.description = "check nrf24l01",
		.function = check_func
	},
	{
		.name = "receive",
		.description = "receive from nrf24l01",
		.function = receive_func
	},
	{
		.name = "send",
		.description = "send via nrf24l01",
		.function = send_func
	},
	{
		.name = "sensor",
		.description = "print all data from IMU",
		.function = sensor_func
	}
};	

void read_string(char *command){
	char ch[2] = {0};
	int curr_char = 0;
	int curr_len = 0;
	char done = 0;

	do{
		ch[0] = non_block_receive_byte();
		if(curr_char >= MAX_COMMAND_LEN || (ch[0] == '\r') || (ch[0] == '\n')){
			command[curr_len] = '\0';
			printf("\n");
			done = 1;
		}else if(ch[0] == BACKSPACE){
			if(curr_char > 0){
				int i;
				curr_char--;
				for(i = curr_char; i < curr_len; i++){
					command[i] = command[i+1];
				}
				curr_len--;
				command[curr_len] = '\0';
				printf("\b%s \b", command+curr_char);
				i = curr_len - curr_char;
				while(i){
					printf("\b");
					i--;
				}
			}
		}else if(ch[0] == ESC){
			ch[0] = non_block_receive_byte();
			if(ch[0] == '['){
				ch[0] = non_block_receive_byte();
				if(ch[0] == 'C'){
					if(curr_len > curr_char){
						printf("%c",command[curr_char]);
						curr_char++;
					}
				}else if(ch[0] == 'D'){
					if(curr_char > 0){
						curr_char--;
						printf("\b");
					}
				}
				continue;
			}
		}else if(ch[0] != '\0'){
			if(curr_char < curr_len){
				int i;
				for(i = curr_len; i > curr_char; i--){
					command[i] = command[i-1];
				}
				curr_len++;
				command[curr_len] = '\0';
				command[curr_char] =ch[0];
				printf("%s", command+curr_char);
				curr_char++;
				i = curr_len - curr_char;
				while(i){
					printf("\b");
					i--;
				}
			}else{
				command[curr_char++] = ch[0];
				curr_len++;
				printf("%c", ch[0]);
			}
		}
	}while(!done);

}

void help_func(int argc, char *argv[]){
	int i;
	printf("This shell supports the commands following:\n");
	for(i = 0; i < MAX_COMMANDS; i++){
		printf("%s: %s\n", commands[i].name, commands[i].description);
	}
}

void hello_func(int argc, char *argv[]){
	printf("Hello World\n");
}

void check_func(int argc, char *argv[]){
	u8 Sta = ERROR;
	printf("Check.....\n");
	Sta = nRF_Check();
	if(Sta == ERROR){
		printf("Fail.....\n");
	}else{
		printf("Success!!!!!!\n");
	}
}

void receive_func(int argc, char *argv[]){
	u8 Sta;
	printf("Receive:");
	nRF_RX_Mode();
	Sta = nRF_Rx_Data(rec_word);
/*	if(Sta == RX_DR){
		printf("RX_DR\n");
	}*/
	printf("%s\n", rec_word);
	printf("Success\n");
}

void send_func(int argc, char *argv[]){
	u8 Sta;
	int i;
	char *ptr;
	nRF_TX_Mode();
	ptr = send_word;
	for(i = 1; i < argc; i++){
		strcpy(ptr, argv[i]);
		ptr += strlen(argv[i]);
		strcpy(ptr, " ");
		ptr += 1;
	}
	printf("Send:%s\n", send_word);
	do{
		Sta = nRF_Tx_Data(send_word);
	}while(Sta == MAX_RT);
	printf("Success\n");
}

void sensor_func(int argc, char *argv[]){
// read from ./module/module_sensor.h for the IMU data


}

void user_shell(void * pvParameters){
	char command[MAX_COMMAND_LEN];
	int i;
	char *argv[MAX_ARGV] = {NULL};
	int argc = 0;
	while(1){
		argc = 0;
		printf("evshary->");
		read_string(command);
		argv[argc++] = (char *)strtok(command, ' ');
		while(1){
			argv[argc] = (char *)strtok(NULL, ' ');
			if(argv[argc] == NULL)break;
			argc++;
		}
		for(i = 0; i < MAX_COMMANDS; i++){
			if(!strcmp(commands[i].name, argv[0])){
				commands[i].function(argc, argv);
				break;
			}
		}
	}
}
