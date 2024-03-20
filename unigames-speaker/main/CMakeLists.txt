set(COMPONENT_SRCS 
    ./main.c 
    ./audio/audio_man.c 
    ./buttons/button_man.c ./buttons/button_tasks.c 
    ./clock/clock_man.c 
    ./interface/user_interface.c
    ./lcd/lcd_man.c 
    ./threads/thread_man.c
    ./time/time_man.c 
    ./microphone/mic_man.c ./microphone/goertzel_filter.c
)
set(COMPONENT_ADD_INCLUDEDIRS "")
set(COMPONENT_EMBED_TXTFILES)

register_component()
