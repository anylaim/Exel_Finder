cmake_minimum_required(VERSION 3.16)

project(Exel_Finder VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Поиск Qt ---
find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Core LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core LinguistTools)

# --- Подключение QXlsx ---
add_subdirectory(QXlsx/QXlsx)

# --- Пути к исполняемым файлам Qt Linguist ---
set(LUPDATE_EXE "C:/Qt/6.8.1/msvc2022_64/bin/lupdate.exe")
set(LRELEASE_EXE "C:/Qt/6.8.1/msvc2022_64/bin/lrelease.exe")

# --- Источники проекта ---
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    settings.cpp
    settings.h
    settings.ui
    resources.qrc
)

add_executable(Exel_Finder ${PROJECT_SOURCES})

# --- Переводы ---
set(TRANSLATIONS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/translations)
file(MAKE_DIRECTORY ${TRANSLATIONS_DIR})

set(TS_FILES
    ${TRANSLATIONS_DIR}/exelfinder_ru.ts
    ${TRANSLATIONS_DIR}/exelfinder_en.ts
)

# Генерация TS
add_custom_command(
    OUTPUT ${TS_FILES}
    COMMAND "${LUPDATE_EXE}" -recursive ${CMAKE_CURRENT_SOURCE_DIR} -ts ${TS_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating translation files"
    VERBATIM
)

add_custom_target(generate_ts DEPENDS ${TS_FILES})

# Компиляция QM
set(QM_FILES "")
foreach(TS_FILE ${TS_FILES})
    get_filename_component(TS_NAME ${TS_FILE} NAME_WE)
    set(QM_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TS_NAME}.qm)

    add_custom_command(
        OUTPUT ${QM_FILE}
        COMMAND "${LRELEASE_EXE}" ${TS_FILE} -qm ${QM_FILE}
        DEPENDS ${TS_FILE}
        COMMENT "Compiling translation ${TS_NAME}"
    )

    list(APPEND QM_FILES ${QM_FILE})
endforeach()

# Создание динамического qrc с переводами
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc "<RCC>\n  <qresource prefix=\"/translations\">\n")
foreach(QM_FILE ${QM_FILES})
    get_filename_component(QM_NAME ${QM_FILE} NAME)
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc "    <file>${QM_NAME}</file>\n")
endforeach()
file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc "  </qresource>\n</RCC>\n")

qt_add_resources(TRANSLATIONS_RESOURCE ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc)
target_sources(Exel_Finder PRIVATE ${TRANSLATIONS_RESOURCE})

# --- Линковка ---
target_link_libraries(Exel_Finder PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    QXlsx::QXlsx
)

set_target_properties(Exel_Finder PROPERTIES WIN32_EXECUTABLE TRUE)


# --- Финализация ---
qt_finalize_executable(Exel_Finder)
