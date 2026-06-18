# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/Users/emath/Downloads/nrf5340_audio_truecode")
  file(MAKE_DIRECTORY "C:/Users/emath/Downloads/nrf5340_audio_truecode")
endif()
file(MAKE_DIRECTORY
  "C:/Users/emath/Downloads/nrf5340_audio_truecode/build/nrf5340_audio_truecode"
  "C:/Users/emath/Downloads/nrf5340_audio_truecode/build/_sysbuild/sysbuild/images/nrf5340_audio_truecode-prefix"
  "C:/Users/emath/Downloads/nrf5340_audio_truecode/build/_sysbuild/sysbuild/images/nrf5340_audio_truecode-prefix/tmp"
  "C:/Users/emath/Downloads/nrf5340_audio_truecode/build/_sysbuild/sysbuild/images/nrf5340_audio_truecode-prefix/src/nrf5340_audio_truecode-stamp"
  "C:/Users/emath/Downloads/nrf5340_audio_truecode/build/_sysbuild/sysbuild/images/nrf5340_audio_truecode-prefix/src"
  "C:/Users/emath/Downloads/nrf5340_audio_truecode/build/_sysbuild/sysbuild/images/nrf5340_audio_truecode-prefix/src/nrf5340_audio_truecode-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/emath/Downloads/nrf5340_audio_truecode/build/_sysbuild/sysbuild/images/nrf5340_audio_truecode-prefix/src/nrf5340_audio_truecode-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/emath/Downloads/nrf5340_audio_truecode/build/_sysbuild/sysbuild/images/nrf5340_audio_truecode-prefix/src/nrf5340_audio_truecode-stamp${cfgdir}") # cfgdir has leading slash
endif()
