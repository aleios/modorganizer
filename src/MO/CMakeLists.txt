cmake_minimum_required (VERSION 3.9 FATAL_ERROR)
project(MO VERSION 0.1 LANGUAGES CXX)
set(library_name "MO")

## Files ##
# Main
#if HDRs
set(SHARED_HDRS
	include/${library_name}/shared/inject.h
    include/${library_name}/shared/windows_error.h
    include/${library_name}/shared/error_report.h
    include/${library_name}/shared/directoryentry.h
    include/${library_name}/shared/util.h
    include/${library_name}/shared/appconfig.h
    # include/${library_name}/shared/appconfig.inc
    include/${library_name}/shared/leaktrace.h
    include/${library_name}/shared/stackdata.h
)
SET(HDRS
	include/${library_name}/transfersavesdialog.h
    include/${library_name}/syncoverwritedialog.h
    include/${library_name}/spawn.h
    include/${library_name}/singleinstance.h
    include/${library_name}/settingsdialog.h
    include/${library_name}/settings.h
    include/${library_name}/selfupdater.h
    include/${library_name}/selectiondialog.h
    include/${library_name}/queryoverwritedialog.h
    include/${library_name}/profilesdialog.h
    include/${library_name}/profile.h
    include/${library_name}/pluginlistsortproxy.h
    include/${library_name}/pluginlist.h
    include/${library_name}/overwriteinfodialog.h
    include/${library_name}/nxmaccessmanager.h
    include/${library_name}/nexusinterface.h
    include/${library_name}/motddialog.h
    include/${library_name}/modlistsortproxy.h
    include/${library_name}/modlist.h
    include/${library_name}/modinfodialog.h
    include/${library_name}/modinfo.h
	include/${library_name}/modinfobackup.h
	include/${library_name}/modinfoforeign.h
	include/${library_name}/modinfooverwrite.h
	include/${library_name}/modinforegular.h
	include/${library_name}/modinfowithconflictinfo.h
    include/${library_name}/messagedialog.h
    include/${library_name}/mainwindow.h
    include/${library_name}/loghighlighter.h
    include/${library_name}/logbuffer.h
    include/${library_name}/lockeddialog.h
    include/${library_name}/loadmechanism.h
    include/${library_name}/installationmanager.h
    include/${library_name}/helper.h
    include/${library_name}/filedialogmemory.h
    include/${library_name}/executableslist.h
    include/${library_name}/editexecutablesdialog.h
    include/${library_name}/downloadmanager.h
    include/${library_name}/downloadlistwidgetcompact.h
    include/${library_name}/downloadlistwidget.h
    include/${library_name}/downloadlistsortproxy.h
    include/${library_name}/downloadlist.h
    include/${library_name}/directoryrefresher.h
    include/${library_name}/credentialsdialog.h
    include/${library_name}/categoriesdialog.h
    include/${library_name}/categories.h
    include/${library_name}/bbcode.h
    include/${library_name}/activatemodsdialog.h
    include/${library_name}/moapplication.h
    include/${library_name}/profileinputdialog.h
    include/${library_name}/icondelegate.h
    include/${library_name}/csvbuilder.h
    include/${library_name}/savetextasdialog.h
    include/${library_name}/qtgroupingproxy.h
    include/${library_name}/modlistview.h
    include/${library_name}/problemsdialog.h
    include/${library_name}/serverinfo.h
    include/${library_name}/browserview.h
    include/${library_name}/browserdialog.h
    include/${library_name}/persistentcookiejar.h
    include/${library_name}/noeditdelegate.h
    include/${library_name}/previewgenerator.h
    include/${library_name}/previewdialog.h
    include/${library_name}/aboutdialog.h
    include/${library_name}/json.h
    include/${library_name}/safewritefile.h
    include/${library_name}/modflagicondelegate.h
    include/${library_name}/genericicondelegate.h
    include/${library_name}/organizerproxy.h
    include/${library_name}/viewmarkingscrollbar.h
    include/${library_name}/plugincontainer.h
    include/${library_name}/organizercore.h
    include/${library_name}/iuserinterface.h
)
#endif
#if SRCs
set(SHARED_SRCS
	src/shared/inject.cpp
    src/shared/windows_error.cpp
    src/shared/error_report.cpp
    src/shared/directoryentry.cpp
    src/shared/util.cpp
    src/shared/appconfig.cpp
    src/shared/leaktrace.cpp
    src/shared/stackdata.cpp
)
SET(SRCS
    src/transfersavesdialog.cpp
    src/syncoverwritedialog.cpp
    src/spawn.cpp
    src/singleinstance.cpp
    src/settingsdialog.cpp
    src/settings.cpp
    src/selfupdater.cpp
    src/selectiondialog.cpp
    src/queryoverwritedialog.cpp
    src/profilesdialog.cpp
    src/profile.cpp
    src/pluginlistsortproxy.cpp
    src/pluginlist.cpp
    src/overwriteinfodialog.cpp
    src/nxmaccessmanager.cpp
    src/nexusinterface.cpp
    src/motddialog.cpp
    src/modlistsortproxy.cpp
    src/modlist.cpp
    src/modinfodialog.cpp
    src/modinfo.cpp
	src/modinfobackup.cpp
	src/modinfoforeign.cpp
	src/modinfooverwrite.cpp
	src/modinforegular.cpp
	src/modinfowithconflictinfo.cpp
    src/messagedialog.cpp
    src/mainwindow.cpp
    src/main.cpp
    src/loghighlighter.cpp
    src/logbuffer.cpp
    src/lockeddialog.cpp
    src/loadmechanism.cpp
    src/installationmanager.cpp
    src/helper.cpp
    src/filedialogmemory.cpp
    src/executableslist.cpp
    src/editexecutablesdialog.cpp
    src/downloadmanager.cpp
    src/downloadlistwidgetcompact.cpp
    src/downloadlistwidget.cpp
    src/downloadlistsortproxy.cpp
    src/downloadlist.cpp
    src/directoryrefresher.cpp
    src/credentialsdialog.cpp
    src/categoriesdialog.cpp
    src/categories.cpp
    src/bbcode.cpp
    src/activatemodsdialog.cpp
    src/moapplication.cpp
    src/profileinputdialog.cpp
    src/icondelegate.cpp
    src/csvbuilder.cpp
    src/savetextasdialog.cpp
    src/qtgroupingproxy.cpp
    src/modlistview.cpp
    src/problemsdialog.cpp
    src/serverinfo.cpp
    src/browserview.cpp
    src/browserdialog.cpp
    src/persistentcookiejar.cpp
    src/noeditdelegate.cpp
    src/previewgenerator.cpp
    src/previewdialog.cpp
    src/aboutdialog.cpp
    src/json.cpp
    src/safewritefile.cpp
    src/modflagicondelegate.cpp
    src/genericicondelegate.cpp
    src/organizerproxy.cpp
    src/viewmarkingscrollbar.cpp
    src/plugincontainer.cpp
    src/organizercore.cpp
)
#endif

#if QTs
SET(QTs
	src/transfersavesdialog.ui
    src/syncoverwritedialog.ui
    src/simpleinstalldialog.ui
    src/settingsdialog.ui
    src/selectiondialog.ui
    src/queryoverwritedialog.ui
    src/profilesdialog.ui
    src/overwriteinfodialog.ui
    src/motddialog.ui
    src/modinfodialog.ui
    src/messagedialog.ui
    src/mainwindow.ui
    src/lockeddialog.ui
    src/installdialog.ui
    src/finddialog.ui
    src/editexecutablesdialog.ui
    src/downloadlistwidgetcompact.ui
    src/downloadlistwidget.ui
    src/credentialsdialog.ui
    src/categoriesdialog.ui
    src/activatemodsdialog.ui
    src/profileinputdialog.ui
    src/savetextasdialog.ui
    src/problemsdialog.ui
    src/previewdialog.ui
    src/browserdialog.ui
    src/aboutdialog.ui
	#
	src/resources.qrc
    src/stylesheet_resource.qrc
	#
	src/app_icon.rc
    src/version.rc
)
#endif

## Source Groups ##
source_group(QT FILES ${QTs})

## Dependencies ##
find_package(Qt5Widgets REQUIRED)
# find_package(Qt5WebKit REQUIRED)
find_package(Qt5WinExtras REQUIRED)

## Library ##
add_library(${library_name}Shared STATIC ${SHARED_HDRS} ${SHARED_SRCS})
# Headers
target_include_directories(${library_name}Shared PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src
)
# Dependencies
target_link_libraries(${library_name}Shared PUBLIC Qt5::Widgets Qt5::WinExtras
					  # Qt5::WebKitWidgets
					  Zlib
					  # uibase esptk bsatk
                      Dbghelp advapi32 Version Shlwapi)

## Executable ##
add_executable(${library_name} ${HDRS} ${SRCS} ${QTs})
# Headers
target_include_directories(${library_name} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src
)
# Dependencies
target_link_libraries(${library_name} PUBLIC Qt5::Widgets Qt5::WinExtras
					  # Qt5::WebKitWidgets
					  Zlib
					  # uibase esptk bsatk
                      Dbghelp advapi32 Version Shlwapi)
# QT
set_target_properties(${library_name} PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON)
if(WIN32)
   # Run winddeployqt if it can be found
    find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt HINTS ${QTDIR} ENV QTDIR PATH_SUFFIXES bin)
    add_custom_command(TARGET ${library_name} POST_BUILD
		COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:${library_name}>
	)
endif()
# Install Targets
install(TARGETS ${library_name}Shared EXPORT ${library_name}SharedConfig
    ARCHIVE  DESTINATION lib # <prefix>/lib. DLL import libs.
    LIBRARY  DESTINATION lib # <prefix>/lib. Shared Libs, non DLL.
    RUNTIME  DESTINATION bin # <prefix>/bin. EXEs, DLLs and shit.
)
install(DIRECTORY include/ DESTINATION include)
# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
install(EXPORT ${library_name}SharedConfig DESTINATION cmake)
# This makes the project importable from the build directory
export(TARGETS ${library_name}Shared FILE ${library_name}SharedConfig.cmake)

## Tests ##
# add_executable(${library_name}Tests EXCLUDE_FROM_ALL
    # test/main.cpp
# )
# target_include_directories(${library_name}Tests PRIVATE test)
# target_link_libraries(${library_name}Tests PRIVATE ${library_name} spdlog GTest)
# add_test(${library_name}_Tests Tests)







