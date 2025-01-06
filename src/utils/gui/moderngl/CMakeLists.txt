set(utils_gui_moderngl_STAT_SRCS
    GLBufferStruct.h
    GLRenderer.h
    GLRenderer.cpp
    GLShader.h
    GLShader.cpp
    GLVertexArrayObject.h
    GLVertexArrayObject.cpp
)

add_library(utils_gui_moderngl STATIC ${utils_gui_moderngl_STAT_SRCS})
set_property(TARGET utils_gui_moderngl PROPERTY PROJECT_LABEL "z_utils_gui_moderngl")
