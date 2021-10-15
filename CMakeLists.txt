project(obs-birddog-p200-controller VERSION 1.0.0)
set(PROJECT_FULL_NAME "BirdDog P200 Controller")

set(obs-birddog-p200-controller_HEADERS
	locale.h
	controller.hpp
	dockwidget.hpp
	joystickwidget.hpp
	presetwidget.hpp
	settingsdialog.hpp)

set(obs-birddog-p200-controller_SOURCES
	module.c
	controller.cpp
	dockwidget.cpp
	joystickwidget.cpp
	presetwidget.cpp
	settingsdialog.cpp)

add_library(obs-birddog-p200-controller MODULE
	${obs-birddog-p200-controller_HEADERS}
	${obs-birddog-p200-controller_SOURCES})

find_package(Qt5Network REQUIRED)
target_link_libraries(obs-birddog-p200-controller
		obs-frontend-api
		libobs
		Qt5::Widgets
		Qt5::Core
		Qt5::Network)

install_obs_plugin_with_data(obs-birddog-p200-controller data)
