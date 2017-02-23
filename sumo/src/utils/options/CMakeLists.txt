message("entering options")

set(options_files
	Option.cpp Option.h
	OptionsCont.cpp OptionsCont.h
	OptionsIO.cpp OptionsIO.h
	OptionsLoader.cpp OptionsLoader.h
	OptionsParser.cpp OptionsParser.h)

add_library(utoptions STATIC ${options_files})
