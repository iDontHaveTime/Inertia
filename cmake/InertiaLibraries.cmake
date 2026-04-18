# Contains name mapping libraries function

function(inr_map_name_to_library var_to_set)
    set(temp_list "")
    foreach(lib_name ${ARGN})
        list(APPEND temp_list "Inr${lib_name}")
    endforeach()

    set(${var_to_set} ${temp_list} PARENT_SCOPE)
endfunction()