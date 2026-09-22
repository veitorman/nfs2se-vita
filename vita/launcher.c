#include "platform.h"
#include "config.h"
#include <vitaGL.h>
#include <psp2/ctrl.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/power.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "launcher_ui.h"
#include "font.h"
static const char *path="ux0:data/nfs2-recomp/launcher04.cfg";
static const char *languages[]={"ENGLISH","ESPANOL","FRANCAIS","DEUTSCH","ITALIANO","SVENSKA"};
static const char *tokens[]={"english","spanish","french","german","italian","swedish"};
static const char *buttons[]={"ARRIBA","ABAJO","IZQUIERDA","DERECHA","X","CIRCULO","CUADRADO","TRIANGULO","R","L","START","SELECT"};
static const char *actions[]={"SIN ASIGNAR","GIRAR IZQUIERDA","GIRAR DERECHA","ACELERAR","FRENAR","MIRAR ATRAS","CAMBIAR CAMARA","FRENO DE MANO","BOCINA","SUBIR MARCHA","BAJAR MARCHA","PAUSA / VOLVER","RECOLOCAR AUTO"};
static const char *keys[]={"SIN ASIGNAR","FLECHA ARRIBA","FLECHA ABAJO","FLECHA IZQUIERDA","FLECHA DERECHA","ENTER","ESC","ESPACIO","F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12","TAB","SHIFT","CTRL","ALT","A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","RETROCESO","PAUSA","0","1","2","3","4","5","6","7","8","9"};
static const char *categories[]={"IDIOMA","PANTALLA","RENDIMIENTO","CONTROLES","INICIO","GUARDAR Y JUGAR"};
static const char *profile_name(void){return vita_config.profile==2?"TECLAS PC":(vita_config.profile?"MODERNO":"CLASICO");}
static void glyph_text(int x,int y,const char *s,float r,float g,float b){
 glColor3f(r,g,b);glBegin(GL_QUADS);
 for(;*s;s++,x+=12){unsigned c=(unsigned char)*s;if(c>127)c='?';
 for(int row=0;row<7;row++)for(int col=0;col<5;col++)if(font[c][row]&(1<<(4-col))){float xx=x+col*2,yy=y+row*2;glVertex2f(xx,yy);glVertex2f(xx+2,yy);glVertex2f(xx+2,yy+2);glVertex2f(xx,yy+2);}}
 glEnd();
}
static void text(int x,int y,const char *s,int selected){glyph_text(x+1,y+1,s,0,0,0);glyph_text(x,y,s,selected?1.0f:0.92f,selected?0.85f:0.92f,selected?0.2f:0.92f);}
static GLuint load_background(void){
 const size_t size=960*544*4;FILE *f=fopen("app0:menu.rgba","rb");if(!f)vita_stop("Missing launcher background in VPK");
 unsigned char *pixels=malloc(size);if(!pixels){fclose(f);vita_stop("Cannot allocate launcher background");}
 size_t n=fread(pixels,1,size,f);fclose(f);if(n!=size){free(pixels);vita_stop("Invalid launcher background size");}
 GLuint id;glGenTextures(1,&id);glBindTexture(GL_TEXTURE_2D,id);glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,960,544,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);free(pixels);
 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);glBindTexture(GL_TEXTURE_2D,0);return id;
}
static void background(GLuint id){
 glEnable(GL_TEXTURE_2D);glBindTexture(GL_TEXTURE_2D,id);glColor3f(1,1,1);glBegin(GL_QUADS);
 glTexCoord2f(0,0);glVertex2f(0,0);glTexCoord2f(1,0);glVertex2f(960,0);glTexCoord2f(1,1);glVertex2f(960,544);glTexCoord2f(0,1);glVertex2f(0,544);glEnd();glBindTexture(GL_TEXTURE_2D,0);glDisable(GL_TEXTURE_2D);
}
static void draw_ui(const LauncherUI *u,GLuint bg,const char *error){
 background(bg);
 // Bounds from safeareas.png: left (25,156)-(261,522), right (403,89)-(938,456).
 for(int i=0;i<6;i++){if(i==u->category)text(29,183+i*40,">",1);text(43,183+i*40,categories[i],i==u->category);}
 text(40,464,"START: JUGAR",0);text(40,490,"L: AL INICIAR",0);
 char b[96];text(420,107,u->editor?(u->editor==1?"ACCIONES DE CONDUCCION":"TECLAS PC / AVANZADO"):categories[u->category],1);
 int count=ui_count(u),first=u->row>=10?u->row-9:0;
 const char *controls[]={"PREDETERMINADO CLASICO","PREDETERMINADO MODERNO","PERSONALIZAR BOTONES","AVANZADO: TECLAS DE PC","RESTABLECER PERFIL","VOLVER"};
 const char *res[]={"320X240","480X360","640X480"};
 for(int i=first;i<count&&i<first+10;i++){
  switch(u->category){
   case 0:snprintf(b,sizeof(b),"%s%s",vita_config.language==i?"> ":"  ",languages[i]);break;
   case 1:if(!i)snprintf(b,sizeof(b),"RESOLUCION: %s",res[vita_config.resolution]);else snprintf(b,sizeof(b),"FORMATO: %s",vita_config.stretch?"COMPLETO ESTIRADO":"ORIGINAL 4:3");break;
   case 2:if(!i)snprintf(b,sizeof(b),"PERFIL: %s",vita_config.performance?"ALTO 444/222 MHZ":"NORMAL 333/166 MHZ");else if(i==1)snprintf(b,sizeof(b),"VSYNC: %s",vita_config.vsync?"SI":"NO");else snprintf(b,sizeof(b),"MOSTRAR FPS: %s",vita_config.fps?"SI":"NO");break;
   case 3:if(!u->editor)snprintf(b,sizeof(b),"%s",controls[i]);else if(i<12)snprintf(b,sizeof(b),"%s: %s",buttons[i],u->editor==1?actions[vita_config.actions[i]]:keys[vita_config.keys[i]]);else snprintf(b,sizeof(b),"%s",i==13?"VOLVER":(u->editor==1?"USAR MAPA DE ACCIONES":"USAR TECLAS PC"));break;
   case 4:snprintf(b,sizeof(b),"MOSTRAR: %s",vita_config.show?"SIEMPRE":"SOLO PRIMERA VEZ");break;
   default:snprintf(b,sizeof(b),"X O START PARA GUARDAR E INICIAR");break;
  }
  text(420,153+(i-first)*25,b,u->focus&&i==u->row);
 }
 if(u->category==3&&!u->editor){snprintf(b,sizeof(b),"ACTIVO: %s",profile_name());text(420,335,b,0);}
 if(u->category==5){snprintf(b,sizeof(b),"CONTROLES: %s",profile_name());text(420,203,b,0);snprintf(b,sizeof(b),"IDIOMA: %s",languages[vita_config.language]);text(420,228,b,0);}
 if(*error)text(420,410,error,1);
 else if(u->editor){snprintf(b,sizeof(b),"%d-%d / %d   ACTIVO: %s",first+1,first+10<count?first+10:count,count,profile_name());text(420,410,b,0);}
 else if(u->notice){const char *messages[]={"","CLASICO CARGADO","MODERNO CARGADO","TECLAS PC ACTIVADAS","ACCIONES ACTIVADAS","PERFIL RESTABLECIDO"};text(420,410,messages[u->notice],1);}
 text(420,434,u->focus?"X: ELEGIR   CIRCULO: VOLVER":"CRUCETA/STICK: MOVER   X: ENTRAR",0);
}
static void language_install(void){
 FILE *in=fopen("install.win","rb");if(!in)vita_stop("Cannot read install.win");
 FILE *out=fopen("ux0:data/nfs2-recomp/install03.win.tmp","wb");if(!out){fclose(in);vita_stop("Cannot write language configuration");}
 int ch;while((ch=fgetc(in))!=EOF&&ch!='\n'){}
 int ok=fprintf(out,"4nn%s\r\n",tokens[vita_config.language])>0;
 char b[1024];size_t n;while((n=fread(b,1,sizeof(b),in)))if(fwrite(b,1,n,out)!=n)ok=0;
 if(ferror(in))ok=0;fclose(in);if(fclose(out))ok=0;
 if(!ok||!config_commit("ux0:data/nfs2-recomp/install03.win.tmp","ux0:data/nfs2-recomp/install03.win"))vita_stop("Cannot commit language configuration");
}
void vita_launcher(void){
 int loaded=config_load(&vita_config,path);if(!loaded){config_load(&vita_config,"ux0:data/nfs2-recomp/launcher03.cfg");vita_config.show=1;}SceCtrlData pad={0};sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);sceCtrlPeekBufferPositive(0,&pad,1);
 if(!loaded||vita_config.show||(pad.buttons&SCE_CTRL_LTRIGGER)){
 vita_graphics_init();glUseProgram(0);glDisable(GL_DEPTH_TEST);glDisable(GL_BLEND);glDisable(GL_SCISSOR_TEST);glDisable(GL_TEXTURE_2D);
 glViewport(0,0,960,544);glMatrixMode(GL_PROJECTION);glLoadIdentity();glOrtho(0,960,544,0,-1,1);glMatrixMode(GL_MODELVIEW);glLoadIdentity();
 GLuint bg=load_background();LauncherUI ui={0};unsigned old=pad.buttons,held=0,repeat_at=0;int done=0;const char *error="";
 while(!done){
  sceCtrlPeekBufferPositive(0,&pad,1);
  if(pad.lx<80)pad.buttons|=SCE_CTRL_LEFT;if(pad.lx>176)pad.buttons|=SCE_CTRL_RIGHT;
  if(pad.ly<80)pad.buttons|=SCE_CTRL_UP;if(pad.ly>176)pad.buttons|=SCE_CTRL_DOWN;
  unsigned pressed=pad.buttons&~old;old=pad.buttons;
  unsigned dirs=pad.buttons&(SCE_CTRL_UP|SCE_CTRL_DOWN|SCE_CTRL_LEFT|SCE_CTRL_RIGHT);unsigned now=(unsigned)(sceKernelGetProcessTimeWide()/1000);
  if(dirs!=held){held=dirs;repeat_at=now+400;}else if(dirs&&(int)(now-repeat_at)>=0){pressed|=dirs;repeat_at=now+120;}
  ui_input(&ui,pressed);
  if(ui.save){if(config_save(&vita_config,path))done=1;else error="ERROR AL GUARDAR. REINTENTA.";ui.save=0;}
  draw_ui(&ui,bg,error);vglSwapBuffers(GL_FALSE);sceKernelDelayThread(16000);
 }
 glFinish();glDeleteTextures(1,&bg);
 // Release keys before the game starts; the launcher never feeds game events.
 do{sceCtrlPeekBufferPositive(0,&pad,1);sceKernelDelayThread(10000);}while(pad.buttons);
 glClearColor(0,0,0,1);glMatrixMode(GL_PROJECTION);glLoadIdentity();glMatrixMode(GL_MODELVIEW);glLoadIdentity();
 }
 {char map[180];snprintf(map,sizeof(map),"INPUT CONFIG profile=%d (%s) actions=%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",vita_config.profile,vita_config.profile==2?"PC_KEYS":(vita_config.profile?"MODERN":"CLASSIC"),vita_config.actions[0],vita_config.actions[1],vita_config.actions[2],vita_config.actions[3],vita_config.actions[4],vita_config.actions[5],vita_config.actions[6],vita_config.actions[7],vita_config.actions[8],vita_config.actions[9],vita_config.actions[10],vita_config.actions[11]);vita_log(map);}
 language_install();
 int cpu=vita_config.performance?444:333,gpu=vita_config.performance?222:166;
 int rc=scePowerSetArmClockFrequency(cpu),rg=scePowerSetGpuClockFrequency(gpu),rb=scePowerSetBusClockFrequency(222),rx=scePowerSetGpuXbarClockFrequency(166);
 char b[180];snprintf(b,sizeof(b),"CONFIG lang=%s resolution=%d stretch=%d vsync=%d CPU=%d GPU=%d clock_rc=%d,%d,%d,%d",tokens[vita_config.language],vita_config.resolution,vita_config.stretch,vita_config.vsync,scePowerGetArmClockFrequency(),scePowerGetGpuClockFrequency(),rc,rg,rb,rx);vita_log(b);
}
void vita_apply_config(void){
 extern int initialWinWidth,initialWinHeight,keepAspectRatio,fixedFramebufferSize,vSync;
 const int w[]={320,480,640},h[]={240,360,480};initialWinWidth=w[vita_config.resolution];initialWinHeight=h[vita_config.resolution];keepAspectRatio=!vita_config.stretch;fixedFramebufferSize=1;vSync=vita_config.vsync;
}
