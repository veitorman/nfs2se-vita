#include "platform.h"
#include "config.h"
#include <psp2/io/fcntl.h>
#include <psp2/ctrl.h>
#include <psp2/io/stat.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <SDL2/SDL.h>
#include <vitaGL.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input_logic.h"
unsigned int _newlib_heap_size_user=96*1024*1024;
unsigned int _pthread_stack_default_user=1024*1024;
void vita_log(const char *s) {
 int fd=sceIoOpen("ux0:data/nfs2-recomp/boot.log",SCE_O_WRONLY|SCE_O_CREAT|SCE_O_APPEND,0666);
 if(fd>=0){sceIoWrite(fd,s,strlen(s));sceIoWrite(fd,"\n",1);sceIoClose(fd);}
}
void vita_stop(const char *s){vita_log(s);sceKernelDelayThread(5000000);sceKernelExitProcess(1);for(;;)sceKernelDelayThread(1000000);}
void vita_start(void){
 sceIoMkdir("ux0:data/nfs2-recomp",0777);
 int fd=sceIoOpen("ux0:data/nfs2-recomp/boot.log",SCE_O_WRONLY|SCE_O_CREAT|SCE_O_TRUNC,0666);if(fd>=0)sceIoClose(fd);
 vita_log("NFS II SE Vita 1.00; native Glide -> vitaGL");
 if(chdir("ux0:data/nfs2-recomp/game")<0)vita_stop("Missing game directory");
 freopen("ux0:data/nfs2-recomp/stdout.log","w",stdout);setvbuf(stdout,NULL,_IONBF,0);
 freopen("ux0:data/nfs2-recomp/stderr.log","w",stderr);setvbuf(stderr,NULL,_IONBF,0);
 vita_launcher();
}
void vita_graphics_init(void){
 static int initialized; if(initialized)return;
 SceIoStat st;
 if(sceIoGetstat("ur0:data/libshacccg.suprx",&st)<0)vita_stop("Missing ur0:data/libshacccg.suprx shader compiler");
 vita_log("Glide grSstWinOpen: initializing vitaGL");
 /* Return value reports resolution fallback, not initialization success. */
 if(vglInitExtended(0x800000,960,544,0x1800000,SCE_GXM_MULTISAMPLE_NONE))vita_log("vitaGL resolution fallback");
 initialized=1;vita_log("vitaGL initialized");
}
static unsigned draw_calls,triangles,uploads;
static unsigned audio_gap_max,audio_work_max;
void vita_draw_count(unsigned n){++draw_calls;triangles+=n;}
void vita_texture_count(void){++uploads;}
static void atomic_max(unsigned *p,unsigned n){unsigned v=__atomic_load_n(p,__ATOMIC_RELAXED);while(n>v&&!__atomic_compare_exchange_n(p,&v,n,0,__ATOMIC_RELAXED,__ATOMIC_RELAXED)){} }
void vita_audio_measure(unsigned start,unsigned end){static unsigned previous;if(previous)atomic_max(&audio_gap_max,start-previous);previous=start;atomic_max(&audio_work_max,end-start);}
void vita_graphics_swap(void){
 static unsigned frames,last,previous,max_frame;static unsigned long long swap_sum,between_sum;
 unsigned begin=(unsigned)sceKernelGetProcessTimeWide();
 if(previous){unsigned elapsed=begin-previous;between_sum+=elapsed;if(elapsed>max_frame)max_frame=elapsed;}
 vglSwapBuffers(GL_FALSE);unsigned end=(unsigned)sceKernelGetProcessTimeWide();swap_sum+=end-begin;previous=end;
 if(!last)last=begin;
 ++frames;
 if(end-last>=2000000){
  char b[320];snprintf(b,sizeof(b),"PERF elapsed_us=%u frames=%u fps_x100=%u between_avg_us=%llu swap_avg_us=%llu between_max_us=%u draws=%u triangles=%u uploads=%u audio_gap_max_us=%u audio_work_max_us=%u",end-last,frames,(unsigned)((unsigned long long)frames*100000000/(end-last)),between_sum/frames,swap_sum/frames,max_frame,draw_calls,triangles,uploads,__atomic_exchange_n(&audio_gap_max,0,__ATOMIC_RELAXED),__atomic_exchange_n(&audio_work_max,0,__ATOMIC_RELAXED));vita_log(b);
  last=end;frames=draw_calls=triangles=uploads=max_frame=0;swap_sum=between_sum=0;
 }
}

extern int nfsVitaInputSnapshot(unsigned *bindings);
extern int vita_scancode_from_guest(unsigned scan);
static unsigned live_actions[11],live_race;
void vita_input_snapshot(void){
 unsigned bindings[10];int race=nfsVitaInputSnapshot(bindings);
 for(int i=0;i<10;i++){
  unsigned key=((bindings[i]&255)==4)?vita_scancode_from_guest((bindings[i]>>8)&255):0;
  __atomic_store_n(&live_actions[i+1],key,__ATOMIC_RELAXED);
 }
 static int previous=-1;if(race!=previous){
  char msg[160];snprintf(msg,sizeof(msg),"INPUT context=%s profile=%d t_ms=%u",race?"RACE":"MENU_OR_PAUSE",vita_config.profile,SDL_GetTicks());vita_log(msg);
  if(race){snprintf(msg,sizeof(msg),"INPUT bindings=%08x,%08x,%08x,%08x,%08x,%08x,%08x,%08x,%08x,%08x",bindings[0],bindings[1],bindings[2],bindings[3],bindings[4],bindings[5],bindings[6],bindings[7],bindings[8],bindings[9]);vita_log(msg);}
  previous=race;
 }
 __atomic_store_n(&live_race,race,__ATOMIC_RELEASE);
}

static Uint32 input_tick(Uint32 interval,void *unused){
 (void)unused; SceCtrlData pad={0};
 sceCtrlPeekBufferPositive(0,&pad,1);
 const unsigned masks[]={SCE_CTRL_UP,SCE_CTRL_DOWN,SCE_CTRL_LEFT,SCE_CTRL_RIGHT,SCE_CTRL_CROSS,SCE_CTRL_CIRCLE,SCE_CTRL_SQUARE,SCE_CTRL_TRIANGLE,SCE_CTRL_RTRIGGER,SCE_CTRL_LTRIGGER,SCE_CTRL_START,SCE_CTRL_SELECT};
 const SDL_Scancode codes[]={SDL_SCANCODE_UNKNOWN,SDL_SCANCODE_UP,SDL_SCANCODE_DOWN,SDL_SCANCODE_LEFT,SDL_SCANCODE_RIGHT,SDL_SCANCODE_RETURN,SDL_SCANCODE_ESCAPE,SDL_SCANCODE_SPACE,SDL_SCANCODE_F1,SDL_SCANCODE_F2,SDL_SCANCODE_F3,SDL_SCANCODE_F4,SDL_SCANCODE_F5,SDL_SCANCODE_F6,SDL_SCANCODE_F7,SDL_SCANCODE_F8,SDL_SCANCODE_F9,SDL_SCANCODE_F10,SDL_SCANCODE_F11,SDL_SCANCODE_F12,SDL_SCANCODE_TAB,SDL_SCANCODE_LSHIFT,SDL_SCANCODE_LCTRL,SDL_SCANCODE_LALT,SDL_SCANCODE_A,SDL_SCANCODE_B,SDL_SCANCODE_C,SDL_SCANCODE_D,SDL_SCANCODE_E,SDL_SCANCODE_F,SDL_SCANCODE_G,SDL_SCANCODE_H,SDL_SCANCODE_I,SDL_SCANCODE_J,SDL_SCANCODE_K,SDL_SCANCODE_L,SDL_SCANCODE_M,SDL_SCANCODE_N,SDL_SCANCODE_O,SDL_SCANCODE_P,SDL_SCANCODE_Q,SDL_SCANCODE_R,SDL_SCANCODE_S,SDL_SCANCODE_T,SDL_SCANCODE_U,SDL_SCANCODE_V,SDL_SCANCODE_W,SDL_SCANCODE_X,SDL_SCANCODE_Y,SDL_SCANCODE_Z,SDL_SCANCODE_BACKSPACE,SDL_SCANCODE_PAUSE,SDL_SCANCODE_0,SDL_SCANCODE_1,SDL_SCANCODE_2,SDL_SCANCODE_3,SDL_SCANCODE_4,SDL_SCANCODE_5,SDL_SCANCODE_6,SDL_SCANCODE_7,SDL_SCANCODE_8,SDL_SCANCODE_9};
 static unsigned char oldkeys[SDL_NUM_SCANCODES];unsigned char pressed[SDL_NUM_SCANCODES];
 static InputContext state;unsigned action_keys[11];
 int race=__atomic_load_n(&live_race,__ATOMIC_ACQUIRE);
 for(int i=0;i<11;i++)action_keys[i]=__atomic_load_n(&live_actions[i],__ATOMIC_RELAXED);
 build_input(&state,race,pad,masks,codes,action_keys,pressed);
 // Release the previous context before pressing keys in the new one.
 for(int down=0;down<=1;down++)for(unsigned i=1;i<SDL_NUM_SCANCODES;++i)if(pressed[i]!=oldkeys[i]&&pressed[i]==down){
  SDL_Event e;memset(&e,0,sizeof(e));e.type=pressed[i]?SDL_KEYDOWN:SDL_KEYUP;
  e.key.state=pressed[i]?SDL_PRESSED:SDL_RELEASED;e.key.keysym.scancode=i;e.key.keysym.sym=SDL_GetKeyFromScancode(i);
  if(SDL_PushEvent(&e)==1)oldkeys[i]=pressed[i];
 }
 return interval;
}
void vita_input_start(void){sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);if(!SDL_AddTimer(8,input_tick,NULL))vita_stop("Input timer failed");vita_log("Native input ready: configurable menu/race controls");}
