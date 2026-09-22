#pragma once
void vita_log(const char *message);
void vita_start(void);
void vita_graphics_init(void);
void vita_graphics_swap(void);
void vita_input_start(void);

void vita_stop(const char *reason) __attribute__((noreturn));

void vita_texture_count(void);
void vita_audio_measure(unsigned start,unsigned end);
