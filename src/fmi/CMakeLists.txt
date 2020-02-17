add_custom_target(create_fmi_zip 
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/fmi.xml .
    COMMAND ${CMAKE_COMMAND} -E tar "cfv" "${CMAKE_SOURCE_DIR}/bin/sumo-fmi.zip" --format=zip "fmi.xml")
