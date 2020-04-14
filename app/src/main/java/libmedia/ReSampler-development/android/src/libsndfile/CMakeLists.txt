cmake_minimum_required (VERSION 3.1.3)

project(libsndfile VERSION 1.0.29)

#
# Variables
#

set (CMAKE_C_STANDARD 99)
set (CMAKE_C_STANDARD_REQUIRED TRUE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set (PACKAGE_NAME ${PROJECT_NAME})
set (CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set (CPACK_PACKAGE_VERSION_STAGE "pre1")
set (CPACK_PACKAGE_VERSION_FULL "${PROJECT_VERSION}${CPACK_PACKAGE_VERSION_STAGE}")

#
# System-wide includes
#

include (GNUInstallDirs)
include (FeatureSummary)
include (CMakeDependentOption)
include (CTest)

#
# Options
#

option (BUILD_SHARED_LIBS "Build shared libraries" OFF)
if (BUILD_SHARED_LIBS AND BUILD_TESTING)
	set (BUILD_TESTING OFF)
	message ("Build testing required static libraries. To prevent build errors BUILD_TESTING disabled.")
endif ()
option (BUILD_PROGRAMS "Build programs" ON)
option (BUILD_EXAMPLES "Build examples" ON)
option (ENABLE_CPACK "Enable CPack support" ON)
option (ENABLE_EXPERIMENTAL "Enable experimental code" OFF)
option (ENABLE_BOW_DOCS "Enable black-on-white html docs" OFF)
if (MSVC OR MINGW)
	option (ENABLE_STATIC_RUNTIME "Enable static runtime" OFF)
endif ()
option (ENABLE_PACKAGE_CONFIG "Generate and install package config file" ON)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#
# Setup definitions
#

include(SndFileChecks)


cmake_dependent_option (BUILD_REGTEST "Build regtest" ON "SQLITE3_FOUND" OFF)
cmake_dependent_option (ENABLE_EXTERNAL_LIBS "Enable FLAC, Vorbis, and Opus codecs" ON "VORBISENC_FOUND;FLAC_FOUND;OPUS_FOUND" OFF)
cmake_dependent_option (ENABLE_CPU_CLIP "Enable tricky cpu specific clipper" ON "CPU_CLIPS_POSITIVE;CPU_CLIPS_NEGATIVE" OFF)
if (NOT ENABLE_CPU_CLIP)
	set (CPU_CLIPS_POSITIVE FALSE)
	set (CPU_CLIPS_NEGATIVE FALSE)
endif ()
cmake_dependent_option (ENABLE_COMPATIBLE_LIBSNDFILE_NAME "Set DLL name to libsndfile-1.dll (canonical name), sndfile.dll otherwise" OFF "WIN32;NOT MINGW;BUILD_SHARED_LIBS" OFF)

set (HAVE_EXTERNAL_XIPH_LIBS ${ENABLE_EXTERNAL_LIBS})
set (HAVE_SQLITE3 ${BUILD_REGTEST})
set (HAVE_ALSA_ASOUNDLIB_H ${ALSA_FOUND})
set (HAVE_SNDIO_H ${SNDIO_FOUND})

set (ENABLE_EXPERIMENTAL_CODE ${ENABLE_EXPERIMENTAL})
set (HAVE_SPEEX ${ENABLE_EXPERIMENTAL})
set (HAVE_OPUS ${ENABLE_EXPERIMENTAL})

add_feature_info (BUILD_SHARED_LIBS BUILD_SHARED_LIBS "build shared libraries")
add_feature_info (ENABLE_EXTERNAL_LIBS ENABLE_EXTERNAL_LIBS "enable FLAC, Vorbis, and Opus codecs")
add_feature_info (ENABLE_EXPERIMENTAL ENABLE_EXPERIMENTAL "enable experimental code")
add_feature_info (BUILD_TESTING BUILD_TESTING "build tests")
add_feature_info (BUILD_REGTEST BUILD_REGTEST "build regtest")
add_feature_info (ENABLE_CPACK ENABLE_CPACK "enable CPack support")
add_feature_info (ENABLE_CPU_CLIP ENABLE_CPU_CLIP "Enable tricky cpu specific clipper")
add_feature_info (ENABLE_BOW_DOCS ENABLE_BOW_DOCS "enable black-on-white html docs")
add_feature_info (ENABLE_PACKAGE_CONFIG ENABLE_PACKAGE_CONFIG "generate and install package config file")
if (WIN32 AND (NOT MINGW) AND BUILD_SHARED_LIBS)
	add_feature_info (ENABLE_COMPATIBLE_LIBSNDFILE_NAME ENABLE_COMPATIBLE_LIBSNDFILE_NAME "Set DLL name to libsndfile-1.dll (canonical name), sndfile.dll otherwise")
endif ()

if (MSVC OR MINGW)
	add_feature_info (ENABLE_STATIC_RUNTIME ENABLE_STATIC_RUNTIME "Enable static runtime")
endif ()

set_package_properties (Ogg PROPERTIES
	TYPE RECOMMENDED
	URL "www.xiph.org/ogg/"
	DESCRIPTION "library for manipulating ogg bitstreams"
	PURPOSE "Required to enable Vorbis, Speex, and Opus support"
	)
set_package_properties (VorbisEnc PROPERTIES
	TYPE RECOMMENDED
	URL "www.vorbis.com/"
	DESCRIPTION "open source lossy audio codec"
	PURPOSE "Enables Vorbis support"
	)
set_package_properties (FLAC PROPERTIES
	TYPE RECOMMENDED
	URL "www.xiph.org/flac/"
	DESCRIPTION "Free Lossless Audio Codec Library"
	PURPOSE "Enables FLAC support"
	)
set_package_properties(Opus PROPERTIES
	TYPE RECOMMENDED
	URL	"www.opus-codec.org/"
	DESCRIPTION "Standardized open source low-latency fullband codec"
	PURPOSE	"Enables experimental Opus support"
	)
set_package_properties(Speex PROPERTIES TYPE OPTIONAL
	URL "www.speex.org/"
	DESCRIPTION "an audio codec tuned for speech"
	PURPOSE "Enables experemental Speex support"
	)
set_package_properties (SQLite3 PROPERTIES
	TYPE OPTIONAL
	URL "www.sqlite.org/"
	DESCRIPTION "light weight SQL database engine."
	PURPOSE "Enables regtest"
	)
if (BUILD_SHARED_LIBS)
	set_package_properties (PythonInterp PROPERTIES
		TYPE REQUIRED
		URL "www.python.org/"
		DESCRIPTION "Python is a widely used high-level programming language."
		PURPOSE "Required to build shared libraries"
		)
endif()

feature_summary (WHAT ALL)

#
# Setup configuration
#

configure_file (src/config.h.cmake src/config.h)

configure_file (src/sndfile.h.in src/sndfile.h)

set (prefix ${CMAKE_INSTALL_PREFIX})
set (exec_prefix "\$\{prefix\}")
set (libdir "\$\{prefix\}/${CMAKE_INSTALL_LIBDIR}")
set (includedir "\$\{prefix\}/${CMAKE_INSTALL_INCLUDEDIR}")
set (VERSION ${PROJECT_VERSION})
if (EXTERNAL_XIPH_LIBS)
	set (PC_PRIVATE_LIBS "-lFLAC  -lvorbisenc")
endif ()
configure_file (sndfile.pc.in sndfile.pc @ONLY)

#
# libsndfile
#

# Public libsndfile headers
set (sndfile_HDRS
	src/sndfile.hh
	${CMAKE_CURRENT_BINARY_DIR}/src/sndfile.h
	)

#
# libsndfile static library
#

add_library (sndfile
	src/sfconfig.h
	src/sfendian.h
	src/sf_unistd.h
	src/common.h
	src/common.c
	src/file_io.c
	src/command.c
	src/pcm.c
	src/ulaw.c
	src/alaw.c
	src/float32.c
	src/double64.c
	src/ima_adpcm.c
	src/ms_adpcm.c
	src/gsm610.c
	src/dwvw.c
	src/vox_adpcm.c
	src/interleave.c
	src/strings.c
	src/dither.c
	src/cart.c
	src/broadcast.c
	src/audio_detect.c
 	src/ima_oki_adpcm.c
	src/ima_oki_adpcm.h
	src/alac.c
	src/chunk.c
	src/ogg.h
	src/ogg.c
	src/chanmap.h
	src/chanmap.c
	src/id3.c
	$<$<BOOL:${WIN32}>:src/windows.c>
	$<$<AND:$<BOOL:${WIN32}>,$<BOOL:${WIN32}>>:src/version-metadata.rc>
	src/sndfile.c
	src/aiff.c
	src/au.c
	src/avr.c
	src/caf.c
	src/dwd.c
	src/flac.c
	src/g72x.c
	src/htk.c
	src/ircam.c
	src/macos.c
	src/mat4.c
	src/mat5.c
	src/nist.c
	src/paf.c
	src/pvf.c
	src/raw.c
	src/rx2.c
	src/sd2.c
	src/sds.c
	src/svx.c
	src/txw.c
	src/voc.c
	src/wve.c
	src/w64.c
	src/wavlike.h
	src/wavlike.c
	src/wav.c
	src/xi.c
	src/mpc2k.c
	src/rf64.c
	src/ogg_vorbis.c
	src/ogg_speex.c
	src/ogg_pcm.c
	src/ogg_opus.c
	src/ogg_vcomment.c
	src/nms_adpcm.c
	src/GSM610/config.h
	src/GSM610/gsm.h
	src/GSM610/gsm610_priv.h
	src/GSM610/add.c
	src/GSM610/code.c
	src/GSM610/decode.c
	src/GSM610/gsm_create.c
	src/GSM610/gsm_decode.c
	src/GSM610/gsm_destroy.c
	src/GSM610/gsm_encode.c
	src/GSM610/gsm_option.c
	src/GSM610/long_term.c
	src/GSM610/lpc.c
	src/GSM610/preprocess.c
	src/GSM610/rpe.c
	src/GSM610/short_term.c
	src/GSM610/table.c
	src/G72x/g72x.h
	src/G72x/g72x_priv.h
	src/G72x/g721.c
	src/G72x/g723_16.c
	src/G72x/g723_24.c
	src/G72x/g723_40.c
	src/G72x/g72x.c
	src/ALAC/ALACAudioTypes.h
	src/ALAC/ALACBitUtilities.h
	src/ALAC/EndianPortable.h
	src/ALAC/aglib.h
	src/ALAC/dplib.h
	src/ALAC/matrixlib.h
	src/ALAC/alac_codec.h
	src/ALAC/shift.h
	src/ALAC/ALACBitUtilities.c
	src/ALAC/ag_dec.c
	src/ALAC/ag_enc.c
	src/ALAC/dp_dec.c
	src/ALAC/dp_enc.c
	src/ALAC/matrix_dec.c
	src/ALAC/matrix_enc.c
	src/ALAC/alac_decoder.c
	src/ALAC/alac_encoder.c
	${sndfile_HDRS}
	${CMAKE_CURRENT_BINARY_DIR}/src/config.h
	)

target_include_directories(sndfile
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
		$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
	)
target_link_libraries (sndfile
	PRIVATE
		$<$<BOOL:${LIBM_REQUIRED}>:m>
		$<$<BOOL:${HAVE_EXTERNAL_XIPH_LIBS}>:Vorbis::VorbisEnc>
		$<$<BOOL:${HAVE_EXTERNAL_XIPH_LIBS}>:FLAC::FLAC>
		$<$<AND:$<BOOL:${ENABLE_EXPERIMENTAL}>,$<BOOL:${HAVE_EXTERNAL_XIPH_LIBS}>,$<BOOL:${HAVE_SPEEX}>>:Speex::Speex>
		$<$<BOOL:${HAVE_EXTERNAL_XIPH_LIBS}>:Opus::Opus>
	)
set_target_properties (sndfile PROPERTIES
	PUBLIC_HEADER "${sndfile_HDRS}"
	)

if (ENABLE_COMPATIBLE_LIBSNDFILE_NAME)
	set_target_properties (sndfile PROPERTIES
		RUNTIME_OUTPUT_NAME "libsndfile-1"
		)
endif ()

if (BUILD_SHARED_LIBS)

	if (WIN32)
		set (VERSION_MAJOR ${CPACK_PACKAGE_VERSION_MAJOR})
		set (GEN_TOOL cmake)
		
		set (WIN_RC_VERSION "${CPACK_PACKAGE_VERSION_MAJOR},${CPACK_PACKAGE_VERSION_MINOR},${CPACK_PACKAGE_VERSION_PATCH}")
		set (CLEAN_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
		set (PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})

		configure_file (src/version-metadata.rc.in src/version-metadata.rc @ONLY)
	endif ()

	set_target_properties (sndfile PROPERTIES
		SOVERSION ${PROJECT_VERSION_MAJOR}
		VERSION ${PROJECT_VERSION}
		)

	# Symbol files generation

	if (WIN32)
		set (SYMBOL_FILENAME "sndfile.def")
		set (SYMBOL_OS "win32")
	elseif ((CMAKE_SYSTEM_NAME MATCHES "Darwin") OR (CMAKE_SYSTEM_NAME MATCHES "Rhapsody"))
		set (SYMBOL_FILENAME "Symbols.darwin")
		set (SYMBOL_OS "darwin")
	elseif (CMAKE_SYSTEM_NAME MATCHES "OS2")
		set (SYMBOL_FILENAME "Symbols.os2")
		set (SYMBOL_OS "os2")
	elseif (UNIX)
		set (SYMBOL_FILENAME "Symbols.gnu-binutils")
		set (SYMBOL_OS "linux")
	endif ()

	if (DEFINED SYMBOL_OS)
		add_custom_command (
			OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/src/${SYMBOL_FILENAME}
			COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/src/create_symbols_file.py ${SYMBOL_OS} ${PROJECT_VERSION_MAJOR} > ${CMAKE_CURRENT_BINARY_DIR}/src/${SYMBOL_FILENAME}
			COMMENT "Generating ${SYMBOL_FILENAME}..."
			)

		add_custom_target (GENFILES DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/src/${SYMBOL_FILENAME})
		if (SYMBOL_OS MATCHES "win32")
			target_sources (sndfile
				PRIVATE
					${CMAKE_CURRENT_BINARY_DIR}/src/${SYMBOL_FILENAME}
				)
		elseif (SYMBOL_OS MATCHES "darwin")
			add_dependencies (sndfile GENFILES)
			set_property (TARGET sndfile APPEND_STRING PROPERTY
				LINK_FLAGS " -Wl,-exported_symbols_list -Wl,${CMAKE_CURRENT_BINARY_DIR}/src/${SYMBOL_FILENAME}"
				)
		elseif (SYMBOL_OS MATCHES "os")
			add_dependencies (sndfile GENFILES)
			set_property (TARGET sndfile APPEND_STRING PROPERTY
				LINK_FLAGS " -Wl,-export-symbols ${CMAKE_CURRENT_BINARY_DIR}/src/${SYMBOL_FILENAME}"
				)
		elseif (UNIX)
			add_dependencies (sndfile GENFILES)
			set_property (TARGET sndfile APPEND_STRING PROPERTY
				LINK_FLAGS " -Wl,--version-script,${CMAKE_CURRENT_BINARY_DIR}/src/${SYMBOL_FILENAME}"
				)
		endif()
	endif()

endif ()

#
# Installation
#

if (ENABLE_PACKAGE_CONFIG)

	if (UNIX)
		 set (CMAKE_INSTALL_PACKAGEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/SndFile)
	 elseif(WIN32)
		 set (CMAKE_INSTALL_PACKAGEDIR cmake)
	 endif()


	install (TARGETS sndfile ${SNDFILE_PROGRAM_TARGETS}
		EXPORT SndFileConfig
		RUNTIME DESTINATION			${CMAKE_INSTALL_BINDIR}
		ARCHIVE DESTINATION			${CMAKE_INSTALL_LIBDIR}
		LIBRARY DESTINATION			${CMAKE_INSTALL_LIBDIR}
		PUBLIC_HEADER DESTINATION	${CMAKE_INSTALL_INCLUDEDIR}
	)

	include (CMakePackageConfigHelpers)

	write_basic_package_version_file (SndFileConfigVersion.cmake COMPATIBILITY SameMajorVersion)

	install(EXPORT SndFileConfig
		NAMESPACE SndFile::
		DESTINATION ${CMAKE_INSTALL_PACKAGEDIR}
		)
	install(
		FILES ${CMAKE_CURRENT_BINARY_DIR}/SndFileConfigVersion.cmake
		DESTINATION	${CMAKE_INSTALL_PACKAGEDIR}
		)

else ()

	install (TARGETS sndfile ${sdnfile_PROGRAMS}
		RUNTIME DESTINATION			${CMAKE_INSTALL_BINDIR}
		ARCHIVE DESTINATION			${CMAKE_INSTALL_LIBDIR}
		LIBRARY DESTINATION			${CMAKE_INSTALL_LIBDIR}
		PUBLIC_HEADER DESTINATION	${CMAKE_INSTALL_INCLUDEDIR})

endif ()

if (UNIX)
	set (man_MANS
		man/sndfile-info.1
		man/sndfile-play.1
		man/sndfile-convert.1
		man/sndfile-cmp.1
		man/sndfile-metadata-get.1
		#man/sndfile-metadata-set.1
		man/sndfile-concat.1
		man/sndfile-interleave.1
		#man/sndfile-deinterleave.1
		man/sndfile-salvage.1
		)
	install (FILES ${man_MANS} DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
endif ()

if (ENABLE_BOW_DOCS)
	set (HTML_BGCOLOUR "white")
	set (HTML_FGCOLOUR "black")
else ()
	set (HTML_BGCOLOUR "black")
	set (HTML_FGCOLOUR "white")
endif ()
configure_file (doc/libsndfile.css.in doc/libsndfile.css)
set (html_DATA
	doc/index.html
	doc/libsndfile.jpg
	"${CMAKE_CURRENT_BINARY_DIR}/doc/libsndfile.css"
	doc/print.css
	doc/api.html
	doc/command.html
	doc/bugs.html
	doc/sndfile_info.html
	doc/new_file_type.HOWTO
	doc/win32.html
	doc/FAQ.html
	doc/lists.html
	doc/embedded_files.html
	doc/octave.html
	doc/tutorial.html
	)
install (FILES ${html_DATA} DESTINATION ${CMAKE_INSTALL_DOCDIR})

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/sndfile.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
