# --------------------------------------------------------------------------------
#                            Misc (no change needed).
# --------------------------------------------------------------------------------
# Have CMake parse the config file, generating the config header, with
# correct definitions. Here only used to make version number available to
# the source code. Include "general_config.h" (no .in suffix) in the source.

set(PROJECT_CONFIG_DIR           "config")
set(PROJECT_CONFIG_FILE          "${PROJECT_CONFIG_DIR}/config.toml")

# Imaging stuffs
set(IMAGING_REQUEST_KEY          "imaging-request")
set(IMAGING_READY_KEY            "imaging-ready")
set(IMAGING_DONE_KEY             "imaging-done")

# Copy conf folder
file(COPY "${PROJECT_SOURCE_DIR}/config"
     DESTINATION "${PROJECT_BINARY_DIR}")

configure_file("${PROJECT_CONFIG_DIR}/config.toml"
               "${PROJECT_CONFIG_DIR}/config.toml" COPYONLY)
# add the binary tree to the search path for include files
# so that we will find config.h
# include_directories("${PROJECT_BINARY_DIR}")

# Ask CMake to output a compile_commands.json file for use with things like Vim YCM.
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
