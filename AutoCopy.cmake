function(copy_imported_shared_lib IMPORTED_TARGET CONSUMER_TARGET)
    # Get the actual imported location
    get_target_property(IMPORTED_LOCATION ${IMPORTED_TARGET} IMPORTED_LOCATION)

    add_custom_command(TARGET ${CONSUMER_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${IMPORTED_LOCATION}
            $<TARGET_FILE_DIR:${CONSUMER_TARGET}>
            COMMENT "Copying imported shared library from ${IMPORTED_LOCATION} to $<TARGET_FILE_DIR:${CONSUMER_TARGET}>"
    )
endfunction()