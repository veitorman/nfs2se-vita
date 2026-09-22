#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
typedef int SDL_Scancode;
enum { SDL_SCANCODE_UP=82,SDL_SCANCODE_DOWN=81,SDL_SCANCODE_LEFT=80,SDL_SCANCODE_RIGHT=79,SDL_SCANCODE_RETURN=40,SDL_SCANCODE_ESCAPE=41,SDL_SCANCODE_F11=68,SDL_NUM_SCANCODES=512 };
enum {SCE_CTRL_UP=1,SCE_CTRL_DOWN=2,SCE_CTRL_LEFT=4,SCE_CTRL_RIGHT=8,SCE_CTRL_CROSS=16,SCE_CTRL_CIRCLE=32,SCE_CTRL_SQUARE=64,SCE_CTRL_TRIANGLE=128,SCE_CTRL_RTRIGGER=256,SCE_CTRL_LTRIGGER=512,SCE_CTRL_START=1024,SCE_CTRL_SELECT=2048};
typedef struct{unsigned buttons;unsigned char lx,ly;} SceCtrlData;
#include "input_logic.h"
int main(void){
 unsigned masks[12]={1,2,4,8,16,32,64,128,256,512,1024,2048};int codes[KEY_COUNT]={0};unsigned actions[11]={0,80,79,82,81,5,6,44,11,4,29};unsigned char out[512];InputContext state={0};SceCtrlData pad={16,128,128};
 config_defaults(&vita_config);build_input(&state,0,pad,masks,codes,actions,out);assert(out[40]&&!out[82]);
 build_input(&state,1,pad,masks,codes,actions,out);assert(!out[40]&&!out[82]);
 pad.buttons=0;build_input(&state,1,pad,masks,codes,actions,out);pad.buttons=16;build_input(&state,1,pad,masks,codes,actions,out);assert(out[82]);
 pad.buttons=0;pad.ly=0;build_input(&state,1,pad,masks,codes,actions,out);assert(!out[82]&&!out[5]);
 pad.buttons=1;build_input(&state,1,pad,masks,codes,actions,out);assert(out[5]);
 pad.buttons=0;pad.lx=0;build_input(&state,1,pad,masks,codes,actions,out);assert(out[80]);
 pad.lx=128;config_preset(&vita_config,1);pad.buttons=16|256;build_input(&state,1,pad,masks,codes,actions,out);assert(out[44]&&out[82]);
 vita_config.actions[8]=7;build_input(&state,1,pad,masks,codes,actions,out);assert(out[44]);pad.buttons=256;build_input(&state,1,pad,masks,codes,actions,out);assert(out[44]);
 pad.buttons=1024;build_input(&state,1,pad,masks,codes,actions,out);assert(out[41]);
 pad.buttons=0;pad.ly=0;build_input(&state,0,pad,masks,codes,actions,out);assert(out[82]);
 vita_config.actions[11]=12;pad.buttons=2048;pad.ly=128;state.previous_race=1;build_input(&state,1,pad,masks,codes,actions,out);assert(out[68]);

 // Reproduce the reported Zaps04 PC profile, then assert all physical preset buttons.
 config_defaults(&vita_config);vita_config.profile=2;codes[5]=40;codes[6]=41;codes[1]=82;codes[2]=81;
 state=(InputContext){1,0};pad=(SceCtrlData){32,128,128};build_input(&state,1,pad,masks,codes,actions,out);assert(out[41]);
 pad.buttons=1024;build_input(&state,1,pad,masks,codes,actions,out);assert(out[40]&&!out[41]);
 pad.buttons=2048;build_input(&state,1,pad,masks,codes,actions,out);assert(out[41]);
 const unsigned expected[2][12]={{5,0,80,79,82,6,81,11,4,29,41,0},{5,6,80,79,44,11,29,4,82,81,41,0}};
 for(int profile=0;profile<2;profile++){
  config_preset(&vita_config,profile);
  for(int i=0;i<12;i++){
   state=(InputContext){1,0};pad=(SceCtrlData){masks[i],128,128};build_input(&state,1,pad,masks,codes,actions,out);
   for(unsigned key=1;key<512;key++)assert(out[key]==(key==expected[profile][i]));
  }
 }
 // Race -> results -> next race: held accelerator must not confirm a result.
 for(int profile=0;profile<2;profile++){
  config_preset(&vita_config,profile);
  state=(InputContext){1,0};pad=(SceCtrlData){profile?SCE_CTRL_RTRIGGER:SCE_CTRL_CROSS,128,128};
  build_input(&state,1,pad,masks,codes,actions,out);assert(out[82]);
  build_input(&state,0,pad,masks,codes,actions,out);
  for(int key=1;key<512;key++)assert(!out[key]);
  pad.buttons=0;build_input(&state,0,pad,masks,codes,actions,out);
  pad.buttons=SCE_CTRL_CROSS;build_input(&state,0,pad,masks,codes,actions,out);assert(out[40]&&!out[82]&&!out[44]);
  pad.buttons=SCE_CTRL_SQUARE;build_input(&state,0,pad,masks,codes,actions,out);assert(!out[81]&&!out[29]);
  pad.buttons=SCE_CTRL_UP;build_input(&state,0,pad,masks,codes,actions,out);assert(out[82]&&!out[5]);
  pad.buttons=SCE_CTRL_DOWN;build_input(&state,0,pad,masks,codes,actions,out);assert(out[81]&&!out[6]);
  pad.buttons=SCE_CTRL_CIRCLE;build_input(&state,0,pad,masks,codes,actions,out);assert(out[41]&&!out[6]&&!out[11]);
  pad.buttons=SCE_CTRL_CROSS;build_input(&state,0,pad,masks,codes,actions,out);
  build_input(&state,1,pad,masks,codes,actions,out);assert(!out[40]&&!out[82]&&!out[44]);
  pad.buttons=0;build_input(&state,1,pad,masks,codes,actions,out);
  pad.buttons=profile?SCE_CTRL_RTRIGGER:SCE_CTRL_CROSS;build_input(&state,1,pad,masks,codes,actions,out);assert(out[82]);
 }
 puts("race -> results menu -> next race, classic/modern and held keys PASS");
 puts("reported PC-profile conflict reproduced; all classic/modern buttons PASS");
 puts("menu/race/pause, held transition, shared bindings, stick and presets PASS");
}
