add_executable(test_easy_display_device
    test_device_api.c
)

target_link_libraries(test_easy_display_device PRIVATE
    easy_display
)

add_test(NAME easy_display_device_api COMMAND test_easy_display_device)

