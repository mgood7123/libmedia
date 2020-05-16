add_library(terminal_jni SHARED terminal_jni.c env.c regex_str.c)
target_link_libraries(terminal_jni log)