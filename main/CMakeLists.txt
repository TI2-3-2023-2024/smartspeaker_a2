set(COMPONENT_SRCS ./main.c ./custom_audio.c)
set(COMPONENT_ADD_INCLUDEDIRS "./include")
set(COMPONENT_EMBED_TXTFILES music-16b-2c-44100hz.mp3)

register_component()
