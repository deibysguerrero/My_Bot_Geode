cmake_minimum_required(VERSION 3.25)

# 1. Esto busca las herramientas de Geode (IMPORTANTE)
find_package(Geode REQUIRED)

# 2. Esto define tu proyecto
project(XdBotAndroid VERSION 1.2.5)

# 3. Esto crea el mod usando el código de la carpeta src
add_geode_mod(
    ${PROJECT_NAME}
    SOURCES 
        src/main.cpp
)

# 4. Esto enlaza ImGui para que funcione el menú
target_link_libraries(${PROJECT_NAME} PUBLIC geode.imgui-fallback)
