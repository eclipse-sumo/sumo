set(utils_gui_moderngl_STAT_SRCS
    GLRenderer.h
    GLRenderer.cpp
    GLShader.h
    GLShader.cpp
    GLStructs.h
    GLTransformStack.h
    GLTransformStack.cpp
    GLVertexArrayObject.h
    GLVertexArrayObject.cpp
)

add_library(utils_gui_moderngl STATIC ${utils_gui_moderngl_STAT_SRCS})
set_property(TARGET utils_gui_moderngl PROPERTY PROJECT_LABEL "z_utils_gui_moderngl")
