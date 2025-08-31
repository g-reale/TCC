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
        BackEnd::start(); // O backend inicia todos os analisadores
        s_analyzers_data[freq_to_start].running = true;
    }
}

// Para um analisador (pausa o processamento do backend)
void FrontEnd::Application::stopAnalyzer(float frequency) {
    float freq_to_stop = limitToTwoDecimals(frequency);
    if (s_analyzers_data.count(freq_to_stop)) {
        BackEnd::stop(); // O backend para todos os analisadores
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
    ImGui::Text("Gerenciamento de Analisadores Goertzel");
    ImGui::Separator();
    ImGui::Spacing();

    // Input para inserir nova frequência
    ImGui::Text("Adicionar/Remover Analisador");
    ImGui::InputFloat("Frequência (Hz)", &s_new_frequency_input, 1.0f, 100.0f, "%.2f Hz");
    s_new_frequency_input = limitToTwoDecimals(s_new_frequency_input);

    // Botões para adicionar/remover
    ImGui::SameLine();
    if (ImGui::Button("Adicionar")) {
        addAnalyzer(s_new_frequency_input);
    }
    ImGui::SameLine();
    if (ImGui::Button("Remover")) {
        removeAnalyzer(s_new_frequency_input);
    }

    ImGui::Spacing();
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
                if (ImGui::Button("Iniciar")) {
                    startAnalyzer(freq);
                }
            }
            ImGui::SameLine();

            // Botão remover
            if (ImGui::Button("Remover")) {
                removeAnalyzer(freq);
                it = s_analyzers_data.erase(it);
                ImGui::PopID();
                continue;
            }
            ImGui::PopID();
            ++it;
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
}

// Página de seleção da fonte de áudio
void FrontEnd::Application::showAudioSourceSelection() {
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

    for (auto& pair : s_analyzers_data) {
        float freq = pair.first;
        GoertzelAnalyzerData& data = pair.second;

        ImGui::PushID(freq);
        ImGui::Text("Analisador: %.2f Hz", freq);
        ImGui::Text("Status: %s", data.running ? "Rodando" : "Pausado/Parado");

        if (data.running && (current_time - data.last_update_time > 0.05)) {
            float magnitude = BackEnd::queryFrequency(freq);
            if (data.spectrum_history.size() > 200)
                data.spectrum_history.erase(data.spectrum_history.begin());
            data.spectrum_history.push_back(magnitude);
            
            // Atualizar matriz do heatmap (espectrograma simulado)
            std::vector<float> current_spectrum(data.spectrum_bins);
            for (int i = 0; i < data.spectrum_bins; ++i) {
                float freq_offset = (i - data.spectrum_bins/2) * 100.0f;
                float distance = std::abs(freq_offset);
                current_spectrum[i] = magnitude * std::exp(-distance * 0.01f) + 
                                     0.1f * std::abs(sin(current_time * (i + 1) * 0.5f));
            }
            
            if (data.spectrum_history_matrix.size() >= data.heatmap_history_size) {
                data.spectrum_history_matrix.erase(data.spectrum_history_matrix.begin());
            }
            data.spectrum_history_matrix.push_back(current_spectrum);
            
            data.last_update_time = current_time;
        }

        // Gráfico 1: magnitude no tempo
        std::string header_mag_title = "Magnitude ao Longo do Tempo (" + std::to_string(freq) + " Hz)";
        if (ImGui::CollapsingHeader(header_mag_title.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImPlot::BeginPlot((std::string("##SpectrumMagnitude") + std::to_string(freq)).c_str(), ImVec2(-1, 300))) {
                ImPlot::SetupAxes("Tempo (amostras atrás)", "Magnitude");
                ImPlot::SetupAxisLimits(ImAxis_X1, -(float)data.spectrum_history.size(), 0, ImGuiCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1.5);
                if (!data.spectrum_history.empty()) {
                    std::vector<float> x_indices(data.spectrum_history.size());
                    for (size_t i = 0; i < data.spectrum_history.size(); ++i)
                        x_indices[i] = -(float)(data.spectrum_history.size() - 1 - i);
                    ImPlot::PlotLine("Magnitude", x_indices.data(), data.spectrum_history.data(), data.spectrum_history.size());
                }
                ImPlot::EndPlot();
            }
        }

        // Gráfico 2: Derivada Temporal da Magnitude
        std::string header_deriv_title = "Derivada Temporal da Magnitude (" + std::to_string(freq) + " Hz)";
        if (ImGui::CollapsingHeader(header_deriv_title.c_str())) {
            ImGui::TextWrapped(
                "Este gráfico mostra as variações instantâneas da magnitude, calculadas como a diferença "
                "entre amostras consecutivas. Útil para identificar mudanças rápidas e transientes no sinal.");
            ImGui::Spacing();
            
            if (ImPlot::BeginPlot((std::string("##MagnitudeDerivative") + std::to_string(freq)).c_str(), ImVec2(-1, 250))) {
                ImPlot::SetupAxes("Tempo (amostras atrás)", "Variação da Magnitude");
                
                if (data.spectrum_history.size() > 1) {
                    // Calcular derivada numérica simples
                    std::vector<float> derivative(data.spectrum_history.size() - 1);
                    std::vector<float> x_derivative(data.spectrum_history.size() - 1);
                    
                    for (size_t i = 0; i < derivative.size(); ++i) {
                        derivative[i] = data.spectrum_history[i + 1] - data.spectrum_history[i];
                        x_derivative[i] = -(float)(derivative.size() - 1 - i);
                    }
                    
                    // Encontrar limites para os eixos
                    float min_deriv = *std::min_element(derivative.begin(), derivative.end());
                    float max_deriv = *std::max_element(derivative.begin(), derivative.end());
                    float margin = std::max(0.1f, std::max(std::abs(min_deriv), std::abs(max_deriv)) * 0.1f);
                    
                    ImPlot::SetupAxisLimits(ImAxis_X1, -(float)derivative.size(), 0, ImGuiCond_Always);
                    ImPlot::SetupAxisLimits(ImAxis_Y1, min_deriv - margin, max_deriv + margin, ImGuiCond_Always);
                    
                    ImPlot::PlotLine("Derivada", x_derivative.data(), derivative.data(), derivative.size());
                    
                    // Linha de referência zero
                    float zero_line[2] = {0.0f, 0.0f};
                    float x_zero[2] = {-(float)derivative.size(), 0.0f};
                    ImPlot::PlotLine("Zero", x_zero, zero_line, 2);
                }
                ImPlot::EndPlot();
            }
        }

        // Gráfico 3: Média Móvel da Magnitude
        std::string header_avg_title = "Média Móvel da Magnitude (" + std::to_string(freq) + " Hz)";
        if (ImGui::CollapsingHeader(header_avg_title.c_str())) {
            ImGui::TextWrapped(
                "Este gráfico mostra a média móvel simples (janela de 10 amostras) da magnitude, "
                "suavizando a série temporal para evidenciar tendências e reduzir ruído.");
            ImGui::Spacing();
            
            if (ImPlot::BeginPlot((std::string("##MovingAverage") + std::to_string(freq)).c_str(), ImVec2(-1, 250))) {
                ImPlot::SetupAxes("Tempo (amostras atrás)", "Magnitude");
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1.5);
                
                const int window_size = 10;
                if (data.spectrum_history.size() >= window_size) {
                    // Calcular média móvel
                    std::vector<float> moving_avg;
                    std::vector<float> x_moving_avg;
                    
                    for (size_t i = window_size - 1; i < data.spectrum_history.size(); ++i) {
                        float sum = 0.0f;
                        for (int j = 0; j < window_size; ++j) {
                            sum += data.spectrum_history[i - j];
                        }
                        moving_avg.push_back(sum / window_size);
                        x_moving_avg.push_back(-(float)(data.spectrum_history.size() - 1 - i));
                    }
                    
                    ImPlot::SetupAxisLimits(ImAxis_X1, -(float)data.spectrum_history.size(), 0, ImGuiCond_Always);
                    
                    // Plotar magnitude original (mais transparente)
                    if (!data.spectrum_history.empty()) {
                        std::vector<float> x_indices(data.spectrum_history.size());
                        for (size_t i = 0; i < data.spectrum_history.size(); ++i)
                            x_indices[i] = -(float)(data.spectrum_history.size() - 1 - i);
                        
                        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 1.0f);
                        ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
                        ImPlot::PlotLine("Magnitude Original", x_indices.data(), data.spectrum_history.data(), data.spectrum_history.size());
                        ImPlot::PopStyleColor();
                        ImPlot::PopStyleVar();
                    }
                    
                    // Plotar média móvel (mais destacada)
                    ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2.0f);
                    ImPlot::PlotLine("Média Móvel", x_moving_avg.data(), moving_avg.data(), moving_avg.size());
                    ImPlot::PopStyleVar();
                }
                ImPlot::EndPlot();
            }
        }

        // Gráfico 4: Heatmap do Espectro ao Longo do Tempo
        std::string header_heatmap_title = "Espectrograma Simulado (" + std::to_string(freq) + " Hz)";
        if (ImGui::CollapsingHeader(header_heatmap_title.c_str())) {
            ImGui::TextWrapped(
                "Este heatmap mostra a distribuição de magnitude em diferentes frequências ao longo do tempo, "
                "simulando um espectrograma. As cores indicam a intensidade da magnitude para cada frequência.");
            ImGui::Spacing();
            
            if (ImPlot::BeginPlot((std::string("##Spectrogram") + std::to_string(freq)).c_str(), ImVec2(-1, 300))) {
                ImPlot::SetupAxes("Tempo (amostras atrás)", "Frequência (Hz)");
                
                if (!data.spectrum_history_matrix.empty()) {
                    // Preparar dados para o heatmap
                    int time_steps = data.spectrum_history_matrix.size();
                    int freq_bins = data.spectrum_bins;
                    
                    // Converter matriz para formato linear para ImPlot
                    std::vector<float> heatmap_data(time_steps * freq_bins);
                    for (int t = 0; t < time_steps; ++t) {
                        for (int f = 0; f < freq_bins; ++f) {
                            heatmap_data[t * freq_bins + f] = data.spectrum_history_matrix[t][f];
                        }
                    }
                    
                    float freq_min = data.frequency - (freq_bins/2) * 100.0f;
                    float freq_max = data.frequency + (freq_bins/2) * 100.0f;
                    
                    ImPlot::SetupAxisLimits(ImAxis_X1, -(float)time_steps, 0, ImGuiCond_Always);
                    ImPlot::SetupAxisLimits(ImAxis_Y1, freq_min, freq_max, ImGuiCond_Always);

                    ImPlot::PlotHeatmap("Intensidade", heatmap_data.data(), freq_bins, time_steps, 
                                       0.0f, 1.0f, nullptr, 
                                       ImPlotPoint(-(float)time_steps, freq_min), 
                                       ImPlotPoint(0, freq_max));
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

                // Adicionar esta verificação para print no console
                const float AUDIO_THRESHOLD = 0.01f; // Limiar para considerar que há áudio (ajuste conforme necessário)
                if (magnitude > AUDIO_THRESHOLD) {
                    printf("Audio detectado na frequencia %.2f Hz! Magnitude: %.4f\n", freq, magnitude);
                } else {
                    printf("Nenhum audio significativo na frequencia %.2f Hz. Magnitude: %.4f\n", freq, magnitude);
                }
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
        if (ImGui::BeginTabItem("Configurações: Frequências")) {
            showConfigurationPage();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Configurações: Fonte de Áudio")) {
            showAudioSourceSelection();
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


