#include "ibuddy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

enum cmd_type {
  CMD_ERROR = 0,
  CMD_RESET,
  CMD_FLIP,
  CMD_FLAP,
  CMD_HEART,
  CMD_HEAD,
};

void print_about()
{
  printf( "ibuddycmd usage:\n"
          "ibuddycmd [id] <command> [args]\n"
          "id: iBuddy id from 0 to n based on usb enumeration\n"
          "command: [reset|flip|flap|heart|headcolor]\n"
          "- reset takes no args\n"
          "- flip,flap and heart may have two arg,\n"
          "the number of times the action is performed\n"
          "and the delay between each action\n"
          "- headcolor has one argument, the color expressed\n"
          "  by a combinaison of 3 letters :\n"
          "  R for red\n"
          "  G for green\n"
          "  B for blue\n"
          "  If no color is give, the head light is turned off\n\n"
          "Example:\n"
          "  ibuddycmd flip 3    #eBuddy 0 flip 3 times\n"
          "  ibuddycmd head RB   #eBuddy 0 head color red + blue\n"
          "  ibuddycmd 1 flap 3  #eBuddy 1 flap 3 times\n"
          );
}

int test_int(char * arg)
{
  int i;
  for(i=0; i<strlen(arg); i++) {
    if(!isdigit(arg[i])) return 0;
  }
  return 1;
}

int ibuddy_run_command(int command,int value,int delay)
{
	ibuddy_ref * ibuddy;

	ibuddy = ibuddy_open(0);
	if(ibuddy == NULL){
		printf("can't open i-buddy device %d , exit\n",0);
		exit(1);
	}
  switch(command){
    case CMD_RESET:
      ibuddy_reset_state(ibuddy);
      break;
    case CMD_FLIP:
      ibuddy_flick_body(ibuddy,value,delay);
      break;
    case CMD_FLAP:
      ibuddy_flap_wings(ibuddy,value,delay);
      break;
    case CMD_HEART:
      ibuddy_blink_heart(ibuddy,value,delay);
      break;
    case CMD_HEAD:
      ibuddy_set_head_color(ibuddy,value);
      break;
  }

	ibuddy_close(ibuddy);

}

int count_delay_command(int command,int argc,char ** argv)
{
  int count = 1;
  int delay = 0;
  int i = 0;

  if(argc > 2){
    fprintf(stderr,"error: too much arguments");
    return EXIT_FAILURE;
  }

  for(i=0;i<argc;i++) {
    if(test_int(argv[i])){
      if(i == 0)
        count = atoi(argv[i]);
      if(i == 1)
        delay = atoi(argv[i]);
    } else {
      fprintf(stderr,"error: invalid arguments %s",argv[i]);
      return EXIT_FAILURE;
    }
  }
  return ibuddy_run_command(command,count,delay);

}

int do_reset(int argc,char ** argv)
{
  if(argc != 0){
    fprintf(stderr,"error: too much arguments");
    return EXIT_FAILURE;
  }

  return ibuddy_run_command(CMD_RESET,0,0);

}

int do_head(int argc,char ** argv)
{
  int i;
  unsigned short color = 0x0;

  if(argc > 1){
    fprintf(stderr,"error: too much arguments");
    return EXIT_FAILURE;
  }

  if(argc){
    /* this must be the color */
    for(i=0;i<strlen(argv[0]);i++) {
      switch(tolower(argv[0][i])){
        case 'r':
          color |= IBUDDY_HEAD_RED;
          break;
        case 'g':
          color |= IBUDDY_HEAD_GREEN;
          break;
        case 'b':
          color |= IBUDDY_HEAD_BLUE;
          break;
        default:
          fprintf(stderr,"error: too much arguments");
          return EXIT_FAILURE;
      }
    }
  }

  return ibuddy_run_command(CMD_HEAD,color,0);
}

int test_command(char * command_str)
{
  if(0 == strcmp("reset",command_str))
    return CMD_RESET;
  if(0 == strcmp("flip",command_str))
    return CMD_FLIP;
  if(0 == strcmp("flap",command_str))
    return CMD_FLAP;
  if(0 == strcmp("heart",command_str))
    return CMD_HEART;
  if(0 == strcmp("headcolor",command_str))
    return CMD_HEAD;
  /* unknown command */
  return CMD_ERROR;
}


int main(int argc, char * argv[])
{
  int cur_arg = 1;
  int ibuddy_num = 0;
  int ret = EXIT_FAILURE;
  int command_id;

  if(argc < 2){
    print_about();
    return EXIT_FAILURE;
  }

  /* The first arg can be the i-buddy id,
     otherwise we uses the first one */
  if(test_int(argv[cur_arg])){
    ibuddy_num = atoi(argv[cur_arg]);
    cur_arg++;
  }

  /* extract the command passed */
  command_id = test_command(argv[cur_arg]);
  if(command_id == CMD_ERROR){
      fprintf(stderr,"Invalid command %s\n",argv[cur_arg]);
      print_about();
      return EXIT_FAILURE;
  } else {
    cur_arg++;
  }

  switch(command_id){
    case CMD_RESET:
      ret = do_reset(argc-cur_arg,&argv[cur_arg]);
      break;
    case CMD_FLIP:
    case CMD_FLAP:
    case CMD_HEART:
      ret = count_delay_command(command_id
      ,argc-cur_arg
      ,&argv[cur_arg]);
      break;
    case CMD_HEAD:
      ret = do_head(argc-cur_arg,&argv[cur_arg]);
      break;
    default:
      /* unrecheable */
      fprintf(stderr,"Uninplemented command %s\n",argv[cur_arg]);
      print_about();
      break;
  }

  return ret;
}


