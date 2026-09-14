cmake_minimum_required(VERSION 3.16)
project(ClassicalRecommender LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)   # Q_OBJECT 클래스의 moc 자동 처리

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

add_executable(ClassicalRecommender
    main.cpp
    features.h          features.cpp
    track.h             track.cpp
    musiclibrary.h      musiclibrary.cpp
    keyword.h           keyword.cpp
    recommender.h       recommender.cpp
    audiopreviewplayer.h audiopreviewplayer.cpp
    keywordpage.h       keywordpage.cpp
    choicepage.h        choicepage.cpp
    playlistpage.h      playlistpage.cpp
    addmusicdialog.h    addmusicdialog.cpp
    mainwindow.h        mainwindow.cpp
    styles.h
)

target_link_libraries(ClassicalRecommender PRIVATE Qt6::Widgets Qt6::Multimedia)

# 프로젝트 루트 경로를 프로그램에 심어준다.
# build 폴더 바깥의 tracks.json 과 music/ 폴더를 항상 이 경로 기준으로 읽는다.
target_compile_definitions(ClassicalRecommender PRIVATE
    PROJECT_ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
