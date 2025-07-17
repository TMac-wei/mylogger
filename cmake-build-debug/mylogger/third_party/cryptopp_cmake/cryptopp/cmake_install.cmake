# Install script for directory: D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/myLogger")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "D:/msys64/mingw64/bin/objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "cryptopp_dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/c++_projects/mylogger/cmake-build-debug/mylogger/third_party/cryptopp_cmake/cryptopp/libcryptopp.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "cryptopp_dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/cryptopp" TYPE FILE FILES
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/3way.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/adler32.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/adv_simd.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/aes.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/aes_armv4.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/algebra.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/algparam.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/allocate.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/arc4.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/argnames.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/aria.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/arm_simd.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/asn.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/authenc.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/base32.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/base64.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/basecode.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/blake2.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/blowfish.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/blumshub.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/camellia.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/cast.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/cbcmac.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/ccm.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/chacha.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/chachapoly.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/cham.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/channels.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/cmac.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/config.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/config_align.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/config_asm.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/config_cpu.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/config_cxx.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/config_dll.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/config_int.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/config_misc.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/config_ns.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/config_os.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/config_ver.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/cpu.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/crc.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/cryptlib.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/darn.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/default.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/des.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/dh.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/dh2.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/dll.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/dmac.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/donna.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/donna_32.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/donna_64.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/donna_sse.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/drbg.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/dsa.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/eax.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/ec2n.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/eccrypto.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/ecp.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/ecpoint.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/elgamal.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/emsa2.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/eprecomp.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/esign.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/fhmqv.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/files.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/filters.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/fips140.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/fltrimpl.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/gcm.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/gf256.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/gf2_32.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/gf2n.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/gfpcrypt.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/gost.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/gzip.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/hashfwd.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/hc128.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/hc256.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/hex.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/hight.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/hkdf.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/hmac.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/hmqv.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/hrtimer.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/ida.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/idea.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/integer.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/iterhash.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/kalyna.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/keccak.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/lea.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/lsh.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/lubyrack.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/luc.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/mars.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/md2.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/md4.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/md5.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/mdc.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/mersenne.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/misc.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/modarith.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/modes.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/modexppc.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/mqueue.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/mqv.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/naclite.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/nbtheory.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/nr.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/oaep.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/oids.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/osrng.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/ossig.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/padlkrng.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/panama.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/pch.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/pkcspad.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/poly1305.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/polynomi.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/ppc_simd.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/pssr.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/pubkey.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/pwdbased.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/queue.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/rabbit.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/rabin.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/randpool.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/rc2.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/rc5.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/rc6.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/rdrand.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/rijndael.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/ripemd.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/rng.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/rsa.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/rw.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/safer.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/salsa.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/scrypt.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/seal.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/secblock.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/secblockfwd.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/seckey.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/seed.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/serpent.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/serpentp.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/sha.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/sha1_armv4.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/sha256_armv4.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/sha3.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/sha512_armv4.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/shacal2.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/shake.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/shark.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/simeck.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/simon.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/simple.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/siphash.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/skipjack.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/sm3.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/sm4.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/smartptr.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/sosemanuk.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/speck.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/square.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/stdcpp.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/strciphr.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/tea.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/threefish.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/tiger.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/trap.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/trunhash.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/ttmac.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/tweetnacl.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/twofish.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/vmac.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/wake.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/whrlpool.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/words.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/xed25519.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/xtr.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/xtrcrypt.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/xts.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/zdeflate.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/zinflate.h"
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/zlib.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "cryptopp_dev" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/cryptopp/cryptopp-static-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/cryptopp/cryptopp-static-targets.cmake"
         "D:/c++_projects/mylogger/cmake-build-debug/mylogger/third_party/cryptopp_cmake/cryptopp/CMakeFiles/Export/b2240bf58d48ab81379cb5dc4149e5db/cryptopp-static-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/cryptopp/cryptopp-static-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/cmake/cryptopp/cryptopp-static-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/cryptopp" TYPE FILE FILES "D:/c++_projects/mylogger/cmake-build-debug/mylogger/third_party/cryptopp_cmake/cryptopp/CMakeFiles/Export/b2240bf58d48ab81379cb5dc4149e5db/cryptopp-static-targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/cryptopp" TYPE FILE FILES "D:/c++_projects/mylogger/cmake-build-debug/mylogger/third_party/cryptopp_cmake/cryptopp/CMakeFiles/Export/b2240bf58d48ab81379cb5dc4149e5db/cryptopp-static-targets-debug.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "cryptopp_dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/cmake/cryptopp" TYPE FILE FILES
    "D:/c++_projects/mylogger/mylogger/third_party/cryptopp_cmake/cryptopp/cryptoppConfig.cmake"
    "D:/c++_projects/mylogger/cmake-build-debug/mylogger/third_party/cryptopp_cmake/cryptopp/cryptoppConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "cryptopp_dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pkgconfig" TYPE FILE FILES "D:/c++_projects/mylogger/cmake-build-debug/mylogger/third_party/cryptopp_cmake/cryptopp/cryptopp.pc")
endif()

