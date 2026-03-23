#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <geode.imgui-fallback/Header.hpp>
#include <fstream>
#include <vector>

using namespace geode::prelude;

// Estructura para los inputs del bot
struct MacroInput {
    int frame;
    bool pressed;
    PlayerButton button;
};

// Variables de control
std::vector<MacroInput> g_macro;
bool g_recording = false;
bool g_playing = false;
size_t g_currentIndex = 0;

// --- REGISTRO DE CLICS (RECORD) ---
class $modify(MyPlayer, PlayerObject) {
    void pushButton(PlayerButton btn) {
        PlayerObject::pushButton(btn);
        if (g_recording && PlayLayer::get()) {
            int f = static_cast<int>(PlayLayer::get()->m_gameState.m_levelTime * 60);
            g_macro.push_back({f, true, btn});
        }
    }

    void releaseButton(PlayerButton btn) {
        PlayerObject::releaseButton(btn);
        if (g_recording && PlayLayer::get()) {
            int f = static_cast<int>(PlayLayer::get()->m_gameState.m_levelTime * 60);
            g_macro.push_back({f, false, btn});
        }
    }
};

// --- REPRODUCCIÓN (PLAYBACK) ---
class $modify(MyPlayLayer, PlayLayer) {
    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);
        if (g_playing && !g_macro.empty()) {
            int currentFrame = static_cast<int>(m_gameState.m_levelTime * 60);
            
            while (g_currentIndex < g_macro.size() && g_macro[g_currentIndex].frame <= currentFrame) {
                auto& input = g_macro[g_currentIndex];
                if (input.pressed) m_player1->pushButton(input.button);
                else m_player1->releaseButton(input.button);
                g_currentIndex++;
            }
        }
    }

    bool init(GJGameLevel* level, bool useReplay, bool dontSave) {
        g_currentIndex = 0; 
        return PlayLayer::init(level, useReplay, dontSave);
    }
};

// --- INTERFAZ IMGUI ---
#include <Geode/modify/MenuLayer.hpp>
class $modify(MyMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        
        ImGuiLayer::get()->delegate([=]() {
            if (ImGui::Begin("XdBot Elliot - 2.208")) {
                ImGui::Text("Bot Status: %s", g_recording ? "Recording" : (g_playing ? "Playing" : "Idle"));
                ImGui::Separator();
                
                ImGui::Checkbox("Grabar", &g_recording);
                ImGui::Checkbox("Reproducir", &g_playing);
                
                if (ImGui::Button("Limpiar Macro")) {
                    g_macro.clear();
                    g_currentIndex = 0;
                }

                if (ImGui::Button("Guardar .gdr2")) {
                    auto macroDir = Mod::get()->getConfigDir() / "macros";
                    (void) utils::file::createDirectoryAll(macroDir);
                    
                    auto path = macroDir / "macro_2208.gdr2";
                    std::ofstream f(path.string());
                    f << "{\"inputs\":[";
                    for(size_t i = 0; i < g_macro.size(); i++) {
                        f << "{\"f\":" << g_macro[i].frame << ",\"p\":" << (g_macro[i].pressed?1:0) << "}";
                        if (i < g_macro.size() - 1) f << ",";
                    }
                    f << "]}";
                    f.close();
                    FLAlertLayer::create("Bot", "Macro guardada!", "OK")->show();
                }
            }
            ImGui::End();
        });
        return true;
    }
};

