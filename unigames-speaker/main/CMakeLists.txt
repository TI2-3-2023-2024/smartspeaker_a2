set(COMPONENT_SRCS ./main.c ./audio/audio_man.c ./lcd/lcd_man.c ./time/time_man.c ./clock/clock_man.c ./buttons/button_man.c ./buttons/button_tasks.c ./threads/thread_man.c)
set(COMPONENT_ADD_INCLUDEDIRS "")
set(COMPONENT_EMBED_TXTFILES music-16b-2c-8000hz.mp3 music-16b-2c-22050hz.mp3 music-16b-2c-44100hz.mp3)

register_component()
