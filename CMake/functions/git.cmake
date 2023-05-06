function(get_git_commit_hash output_var)
  execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(${output_var} ${GIT_COMMIT_HASH} PARENT_SCOPE)
endfunction(get_git_commit_hash)
