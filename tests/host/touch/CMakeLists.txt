add_executable(test_touch_cst816
    test_touch_cst816.c
)
target_link_libraries(test_touch_cst816 PRIVATE
    easy_display_touch_cst816
)
add_test(NAME touch_cst816 COMMAND test_touch_cst816)

add_executable(test_touch_xpt2046
    test_touch_xpt2046.c
)
target_link_libraries(test_touch_xpt2046 PRIVATE
    easy_display_touch_xpt2046
)
add_test(NAME touch_xpt2046 COMMAND test_touch_xpt2046)
