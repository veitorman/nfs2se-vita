#pragma once
#include <string.h>
typedef struct { int previous_race;unsigned blocked; } InputContext;
static void build_input(InputContext *state,int race,SceCtrlData pad,const unsigned *masks,const SDL_Scancode *codes,const unsigned *action_keys,unsigned char *pressed){
 memset(pressed,0,SDL_NUM_SCANCODES);


 if(race!=state->previous_race){state->blocked=pad.buttons;state->previous_race=race;}
 state->blocked&=pad.buttons;unsigned buttons=pad.buttons&~state->blocked;
 if(!race){
  if(buttons&SCE_CTRL_UP)pressed[SDL_SCANCODE_UP]=1;
  if(buttons&SCE_CTRL_DOWN)pressed[SDL_SCANCODE_DOWN]=1;
  if(buttons&SCE_CTRL_LEFT)pressed[SDL_SCANCODE_LEFT]=1;
  if(buttons&SCE_CTRL_RIGHT)pressed[SDL_SCANCODE_RIGHT]=1;
  if(buttons&SCE_CTRL_CROSS)pressed[SDL_SCANCODE_RETURN]=1;
  if(buttons&(SCE_CTRL_CIRCLE|SCE_CTRL_START|SCE_CTRL_SELECT))pressed[SDL_SCANCODE_ESCAPE]=1;
 }else for(unsigned i=0;i<BUTTON_COUNT;i++)if(buttons&masks[i]){
  unsigned key=0;
  if(vita_config.profile==2)key=codes[vita_config.keys[i]];
  else {int action=vita_config.actions[i];key=action==11?SDL_SCANCODE_ESCAPE:(action==12?SDL_SCANCODE_F11:(action?action_keys[action]:0));}
  if(key>0&&key<SDL_NUM_SCANCODES)pressed[key]=1;
 }
 if(!race){if(pad.ly<80)pressed[SDL_SCANCODE_UP]=1;if(pad.ly>176)pressed[SDL_SCANCODE_DOWN]=1;}
 if(pad.lx<80){unsigned key=race?action_keys[1]:SDL_SCANCODE_LEFT;if(key>0&&key<SDL_NUM_SCANCODES)pressed[key]=1;}
 if(pad.lx>176){unsigned key=race?action_keys[2]:SDL_SCANCODE_RIGHT;if(key>0&&key<SDL_NUM_SCANCODES)pressed[key]=1;}
}
