message("entering cpp")

set(cpp_files
	CEP.cpp CEP.h CEPHandler.cpp CEPHandler.h
	Constants.cpp Constants.h Helpers.cpp Helpers.h)

add_library(foreignphemlight STATIC ${cpp_files})
