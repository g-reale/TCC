#include "FrontEnd.hpp"
#include "../include/BackEnd.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <GLFW/glfw3.h>

// Guarda os dados de todos os analisadores criados (1 analisador = 1 frequência monitorada)
std::map<float, FrontEnd::GoertzelAnalyzerData> FrontEnd::Application::s_analyzers_data;

// Valor padrão de frequência que aparece no input ao iniciar o programa
float FrontEnd::Application::s_new_frequency_input = 1000.0f;

// Índice da fonte de áudio selecionada (ex.: microfone padrão do sistema)
int FrontEnd::Application::s_selected_source_index = 0;

// Lista com todas as fontes de áudio detectadas
std::vector<std::string> FrontEnd::Application::s_audio_sources;

// Função para limitar o a frequência em duas cadas decimais
float FrontEnd::Application::limitToTwoDecimals(float value) {
    return std::round(value * 100.0f) / 100.0f;
}

// Inicialização da interface gráfica
void FrontEnd::Application::initialize(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;    
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   

    // Define estilo escuro padrão
    ImGui::StyleColorsDark();

    // Inicializa integração ImGui + GLFW + OpenGL
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Busca todas as fontes de áudio disponíveis no sistema
    s_audio_sources = BackEnd::querySources();
    if (s_audio_sources.empty()) {
        s_audio_sources.push_back("Nenhuma fonte de áudio disponível");
    }

    // Define a primeira fonte como padrão, se houver
    if (!s_audio_sources.empty()) {
        BackEnd::setSource(s_audio_sources[s_selected_source_index]);
    }
}

// Limpeza da interface (quando fechar o programa)
void FrontEnd::Application::cleanup() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

// Gerenciamento de analisadores (criar, remover, iniciar e parar)

// Cria um novo analisador para uma frequência específica
void FrontEnd::Application::addAnalyzer(float frequency) {
    float freq_to_add = limitToTwoDecimals(frequency);
    if (s_analyzers_data.find(freq_to_add) == s_analyzers_data.end()) {
        BackEnd::createAnalyzer(freq_to_add);
        GoertzelAnalyzerData new_data;
        new_data.frequency = freq_to_add;
        s_analyzers_data[freq_to_add] = new_data;
    }
}

// Remove um analisador existente
void FrontEnd::Application::removeAnalyzer(float frequency) {
    float freq_to_remove = limitToTwoDecimals(frequency);
    if (s_analyzers_data.find(freq_to_remove) != s_analyzers_data.end()) {
        BackEnd::destroyAnalyzer(freq_to_remove);
        s_analyzers_data.erase(freq_to_remove);
    }
}

// Inicia um analisador (liga o processamento do backend)
void FrontEnd::Application::startAnalyzer(float frequency) {
    float freq_to_start = limitToTwoDecimals(frequency);
    if (s_analyzers_data.count(freq_to_start)) {
        s_analyzers_data[freq_to_start].running = true;
    }
}

// Para um analisador (pausa o processamento do backend)
void FrontEnd::Application::stopAnalyzer(float frequency) {
    float freq_to_stop = limitToTwoDecimals(frequency);
    if (s_analyzers_data.count(freq_to_stop)) {
        s_analyzers_data[freq_to_stop].running = false;
    }
}

// Funções para seleção de fonte de áudio
void FrontEnd::Application::setAudioSource(const std::string& source) {
    BackEnd::setSource(source);
}

std::vector<std::string> FrontEnd::Application::getAvailableAudioSources() {
    return BackEnd::querySources();
}

// Página de configuração de analisadores
void FrontEnd::Application::showConfigurationPage() {
    ImGui::Text("Seleção de Fonte de Áudio");
    ImGui::Separator();
    ImGui::Spacing();

    if (s_audio_sources.empty()) {
        ImGui::Text("Nenhuma fonte de áudio disponível.");
        return;
    }

    const char* current_source = s_audio_sources[s_selected_source_index].c_str();
    if (ImGui::BeginCombo("##AudioSourceCombo", current_source)) {
        for (int i = 0; i < s_audio_sources.size(); ++i) {
            bool is_selected = (s_selected_source_index == i);
            if (ImGui::Selectable(s_audio_sources[i].c_str(), is_selected)) {
                s_selected_source_index = i;
                setAudioSource(s_audio_sources[s_selected_source_index]);
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();

    
    ImGui::Text("Gerenciamento de Analisadores Goertzel");
    ImGui::Separator();
    ImGui::Spacing();

    // Input para inserir nova frequência
    ImGui::Text("Adicionar/Remover Analisador");
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
        "Clique sobre o valor para editar e digite a frequência desejada, ou use os botões + / -");
    ImGui::InputFloat("Frequência (Hz)", &s_new_frequency_input, 1.0f, 100.0f, "%.2f Hz");
    s_new_frequency_input = limitToTwoDecimals(s_new_frequency_input);

    
    // Botões para adicionar/remover
    ImGui::SameLine();
    if (ImGui::Button("Adicionar")) {
        addAnalyzer(s_new_frequency_input);
        startAnalyzer(s_new_frequency_input);
    }

    ImGui::Separator();
    ImGui::Spacing();

    // Lista de analisadores já criados
    ImGui::Text("Analisadores Ativos");
    if (s_analyzers_data.empty()) {
        ImGui::Text("Nenhum analisador ativo.");
    } else {
        for (auto it = s_analyzers_data.begin(); it != s_analyzers_data.end(); ) {
            float freq = it->first;
            GoertzelAnalyzerData& data = it->second;

            ImGui::PushID(freq);
            ImGui::Text("Frequência: %.2f Hz", freq);
            ImGui::SameLine();
            
            // Botão iniciar/pausar
            if (data.running) {
                if (ImGui::Button("Pausar")) {
                    stopAnalyzer(freq);
                }
            } else {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "Pausado");
            }
            ImGui::SameLine();

            // Botão remover
            if (ImGui::Button("Remover")) {
                removeAnalyzer(freq);
                ImGui::PopID();
                break; // sai do loop porque o iterador ficou inválido
            }

            ImGui::PopID();
            ++it;
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
}

// Página de visualização individual por analisador
// (Magnitude no tempo, Derivada, Média móvel, Heatmap)
void FrontEnd::Application::showVisualizationPage() {
    ImGui::Text("Visualização por Analisador");
    ImGui::Separator();
    ImGui::Spacing();

    double current_time = glfwGetTime();

    if (s_analyzers_data.empty()) {
        ImGui::Text("Adicione analisadores na página de Configuração para visualizar dados.");
        return;
    }

    BackEnd::update();
    for (auto& pair : s_analyzers_data) {
        float freq = pair.first;
        GoertzelAnalyzerData& data = pair.second;

        ImGui::PushID(freq);
        ImGui::Text("Analisador: %.2f Hz", freq);
        ImGui::Text("Status: %s", data.running ? "Rodando" : "Pausado/Parado");

        if (data.running && (current_time - data.last_update_time > 0)) {
            float magnitude = BackEnd::queryFrequency(freq);
            if (data.spectrum_history.size() > 200)
                data.spectrum_history.erase(data.spectrum_history.begin());
            data.spectrum_history.push_back(magnitude);
            data.last_update_time = current_time;
        }

        // Gráfico 1: magnitude no tempo
        std::string header_mag_title = "Magnitude ao Longo do Tempo (" + std::to_string(freq) + " Hz)";
        if (ImGui::CollapsingHeader(header_mag_title.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImPlot::BeginPlot((std::string("##SpectrumMagnitude") + std::to_string(freq)).c_str(), ImVec2(-1, 300))) {
                ImPlot::SetupAxes("Tempo (amostras atrás)", "Magnitude");
                ImPlot::SetupAxisLimits(ImAxis_X1, -(float)data.spectrum_history.size(), 0, ImGuiCond_Always);
                if (!data.spectrum_history.empty()) {
                    std::vector<float> x_indices(data.spectrum_history.size());
                    for (size_t i = 0; i < data.spectrum_history.size(); ++i)
                        x_indices[i] = -(float)(data.spectrum_history.size() - 1 - i);
                    ImPlot::PlotLine("Magnitude", x_indices.data(), data.spectrum_history.data(), data.spectrum_history.size());
                }
                ImPlot::EndPlot();
            }
        }

        ImGui::PopID();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
    }
}

// Página de visualização geral
void FrontEnd::Application::showOverallVisualizationPage() {
    ImGui::Text("Visualização Geral de Frequências");
    ImGui::Separator();
    ImGui::Spacing();

    if (s_analyzers_data.empty()) {
        ImGui::Text("Adicione analisadores na página de Configuração para visualizar dados.");
        return;
    }

    double current_time = glfwGetTime();

    if (ImPlot::BeginPlot("##OverallSpectrum", ImVec2(-1, 400))) {
        ImPlot::SetupAxes("Frequência (Hz)", "Magnitude");
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1.5);

        // Coletar dados de todas as frequências ativas
        std::vector<float> frequencies;
        std::vector<float> magnitudes;

        for (auto& pair : s_analyzers_data) {
            float freq = pair.first;
            GoertzelAnalyzerData& data = pair.second;

            if (data.running && (current_time - data.last_update_time > 0.05)) {
                float magnitude = BackEnd::queryFrequency(freq);
                if (data.spectrum_history.size() > 200)
                    data.spectrum_history.erase(data.spectrum_history.begin());
                data.spectrum_history.push_back(magnitude);
                data.last_update_time = current_time;
            }
            frequencies.push_back(freq);
            magnitudes.push_back(data.spectrum_history.empty() ? 0.0f : data.spectrum_history.back());
        }

        // Ordenar as frequências para plotagem correta
        std::vector<std::pair<float, float>> sorted_data;
        for (size_t i = 0; i < frequencies.size(); ++i) {
            sorted_data.push_back({frequencies[i], magnitudes[i]});
        }
        std::sort(sorted_data.begin(), sorted_data.end(), [](const auto& a, const auto& b) {
            return a.first < b.first;
        });

        std::vector<float> sorted_frequencies;
        std::vector<float> sorted_magnitudes;
        for (const auto& p : sorted_data) {
            sorted_frequencies.push_back(p.first);
            sorted_magnitudes.push_back(p.second);
        }

        if (!sorted_frequencies.empty()) {
            ImPlot::PlotBars("Magnitudes", sorted_frequencies.data(), sorted_magnitudes.data(), sorted_frequencies.size(), 5.0f); // Largura da barra ajustável
        }
        ImPlot::EndPlot();
    }

    ImGui::Spacing();
    ImGui::Separator();
}

// Função principal de renderização da interface
void FrontEnd::Application::render(GLFWwindow* window) {
    // Iniciar o frame do ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Criar a janela principal
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("Goertzel Analyzer", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // Abas para navegação
    if (ImGui::BeginTabBar("##MainTabBar")) {
        if (ImGui::BeginTabItem("Configurações")) {
            showConfigurationPage();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Visualização Geral")) {
            showOverallVisualizationPage();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Visualização por Analisador")) {
            showVisualizationPage();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();

    // Renderiza tudo na tela
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


