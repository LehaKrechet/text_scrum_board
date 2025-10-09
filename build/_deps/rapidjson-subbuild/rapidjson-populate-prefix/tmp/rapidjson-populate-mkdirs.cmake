# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/lehakrechet/institut/proga/text_scrum_board/build/_deps/rapidjson-src")
  file(MAKE_DIRECTORY "/home/lehakrechet/institut/proga/text_scrum_board/build/_deps/rapidjson-src")
endif()
file(MAKE_DIRECTORY
  "/home/lehakrechet/institut/proga/text_scrum_board/build/_deps/rapidjson-build"
  "/home/lehakrechet/institut/proga/text_scrum_board/build/_deps/rapidjson-subbuild/rapidjson-populate-prefix"
  "/home/lehakrechet/institut/proga/text_scrum_board/build/_deps/rapidjson-subbuild/rapidjson-populate-prefix/tmp"
  "/home/lehakrechet/institut/proga/text_scrum_board/build/_deps/rapidjson-subbuild/rapidjson-populate-prefix/src/rapidjson-populate-stamp"
  "/home/lehakrechet/institut/proga/text_scrum_board/build/_deps/rapidjson-subbuild/rapidjson-populate-prefix/src"
  "/home/lehakrechet/institut/proga/text_scrum_board/build/_deps/rapidjson-subbuild/rapidjson-populate-prefix/src/rapidjson-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/lehakrechet/institut/proga/text_scrum_board/build/_deps/rapidjson-subbuild/rapidjson-populate-prefix/src/rapidjson-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/lehakrechet/institut/proga/text_scrum_board/build/_deps/rapidjson-subbuild/rapidjson-populate-prefix/src/rapidjson-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
