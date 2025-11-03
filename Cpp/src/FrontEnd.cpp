#include "FrontEnd.hpp"
#include "../include/BackEnd.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include <algorithm>
#include <cmath>
#include <cfloat> 
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

// Variáveis de controle de magnitude para o espectrograma
double FrontEnd::Application::s_min_magnitude = 0.0;
double FrontEnd::Application::s_max_magnitude = 1.0;


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

    // Define o colormap para o espectrograma
    ImPlot::GetStyle().Colormap = ImPlotColormap_Plasma;

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

        // Formata a magnitude para ficar com duas casas decimais
        char buffer[50];
        std::snprintf(buffer, sizeof(buffer), "Magnitude ao Longo do Tempo (%.2f Hz)", freq);
        std::string header_mag_title(buffer);

        // Permite que a seção seja expandida ou recolhida
        if (ImGui::CollapsingHeader(header_mag_title.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {

            // Cria o gráfico com altura fixa (300px) e largura ajustável
            if (ImPlot::BeginPlot((std::string("##SpectrumMagnitude") + std::to_string(freq)).c_str(), ImVec2(-1, 300))) {

                // Define rótulos dos eixos
                ImPlot::SetupAxes("Tempo (amostras atrás)", "Magnitude");

                // Mostra o eixo X indo de valores negativos até 0 (histórico de amostras)
                ImPlot::SetupAxisLimits(ImAxis_X1, -(float)data.spectrum_history.size(), 0, ImGuiCond_Always);

                // Eixo Y: ajusta dinamicamente de 0 até 10% acima do valor máximo do histórico
                // float y_max = 1.0f; // valor padrão caso não haja histórico
                // if (!data.spectrum_history.empty()) {
                //     y_max = *std::max_element(data.spectrum_history.begin(), data.spectrum_history.end()) * 1.1f;
                // }
                // ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0f, y_max, ImGuiCond_Always);

                // Só plota se houver dados
                if (!data.spectrum_history.empty()) {

                    // Cria os valores do eixo X (tempo negativo → mais antigo)
                    std::vector<float> x_indices(data.spectrum_history.size());
                    for (size_t i = 0; i < data.spectrum_history.size(); ++i)
                        x_indices[i] = -(float)(data.spectrum_history.size() - 1 - i);

                    // Desenha a curva de magnitude ao longo do tempo
                    ImPlot::PushStyleColor(ImPlotCol_Line, IM_COL32(255, 100, 100, 255));
                    ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2.0f);
                    ImPlot::PlotLine("Magnitude", x_indices.data(), data.spectrum_history.data(), data.spectrum_history.size());
                    ImPlot::PopStyleVar();
                    ImPlot::PopStyleColor();
                }

                // Finaliza o gráfico
                ImPlot::EndPlot();
            }
        }


        ImGui::PopID();
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
    }
}

// Função auxiliar para converter std::vector<std::string> para const char* const*
// Necessário para ImPlot::SetupAxisTicks
static std::vector<const char*> convert_to_c_str_array(const std::vector<std::string>& vec) {
    std::vector<const char*> c_str_vec;
    for (const auto& s : vec) {
        c_str_vec.push_back(s.c_str());
    }
    return c_str_vec;
}

// Implementação do ImGui::SliderDouble, pois não é padrão
// Necessário para replicar o comportamento do código de referência
namespace ImGui {
    bool SliderDouble(const char* label, double* v, double v_min, double v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0) {
        return SliderScalar(label, ImGuiDataType_Double, v, &v_min, &v_max, format, flags);
    }
}

// Página de visualização geral
void FrontEnd::Application::showOverallVisualizationPage() {
    ImGui::Text("Visualização Geral de Frequências");
    ImGui::Separator();
    ImGui::Spacing();

    double current_time = glfwGetTime();

    if (s_analyzers_data.empty()) {
        ImGui::Text("Adicione analisadores na página de Configuração para visualizar dados.");
        return;
    }

    BackEnd::update();
    
    // 1. Gráfico de Barras — mostra o espectro de frequência em tempo real
    if (ImGui::CollapsingHeader("Espectro de Frequência", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImPlot::BeginPlot("##OverallSpectrum", ImVec2(-1, 400))) {

            // Configuração dos eixos do gráfico
            ImPlot::SetupAxes("Frequência (Hz)", "Magnitude");
            // ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1.5);

            std::vector<float> frequencies;
            std::vector<float> magnitudes;

            // Atualiza dados de cada analisador ativo
            for (auto& pair : s_analyzers_data) {
                float freq = pair.first;
                GoertzelAnalyzerData& data = pair.second;

                // Atualiza magnitude se o analisador estiver rodando e o tempo mínimo tiver passado
                if (data.running && (current_time - data.last_update_time > 0.05)) {
                    float magnitude = BackEnd::queryFrequency(freq);

                    // Mantém histórico limitado a 200 amostras
                    if (data.spectrum_history.size() > 200)
                        data.spectrum_history.erase(data.spectrum_history.begin());

                    data.spectrum_history.push_back(magnitude);
                    data.last_update_time = current_time;
                }

                // Armazena frequência e última magnitude para plotar
                frequencies.push_back(freq);
                magnitudes.push_back(data.spectrum_history.empty() ? 0.0f : data.spectrum_history.back());
            }

            // Ordena as frequências (garante que o gráfico fique da esquerda p/ direita)
            std::vector<std::pair<float, float>> sorted_data;
            for (size_t i = 0; i < frequencies.size(); ++i)
                sorted_data.push_back({frequencies[i], magnitudes[i]});
            std::sort(sorted_data.begin(), sorted_data.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

            // Separa vetores ordenados de frequência e magnitude
            std::vector<float> sorted_frequencies;
            std::vector<float> sorted_magnitudes;
            for (const auto& p : sorted_data) {
                sorted_frequencies.push_back(p.first);
                sorted_magnitudes.push_back(p.second);
            }

            // Só plota se houver dados válidos
            if (!sorted_frequencies.empty()) {
                float bar_width;

                if (sorted_frequencies.size() == 1) {
                    // Caso especial: só uma frequência → centraliza a barra
                    bar_width = sorted_frequencies[0] * 0.2f;
                    ImPlot::SetupAxisLimits(ImAxis_X1, sorted_frequencies[0] - bar_width, sorted_frequencies[0] + bar_width, ImGuiCond_Always);
                } else {
                    // Múltiplas frequências → calcula o espaçamento médio para ajustar a largura
                    float min_diff = FLT_MAX;
                    for (size_t i = 1; i < sorted_frequencies.size(); ++i)
                        min_diff = std::min(min_diff, sorted_frequencies[i] - sorted_frequencies[i - 1]);

                    bar_width = min_diff * 0.8f;

                    // Adiciona margem nas bordas do gráfico
                    ImPlot::SetupAxisLimits(ImAxis_X1, sorted_frequencies.front() - min_diff, sorted_frequencies.back() + min_diff, ImGuiCond_Always);
                }

                // Desenha o gráfico (usa hastes em vez de barras)
                ImPlot::PushStyleColor(ImPlotCol_Line, IM_COL32(255, 100, 100, 255));
                ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2.0f);    
                ImPlot::PlotStems("Magnitudes", sorted_frequencies.data(), sorted_magnitudes.data(), sorted_frequencies.size());
                ImPlot::PopStyleVar();
                ImPlot::PopStyleColor();
            }

            ImPlot::EndPlot();
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // 2. Bloco do Gráfico de Espectrograma
    // O espectrograma mostrará o histórico de magnitude (eixo Z/cor) ao longo do tempo (eixo X) para cada frequência (eixo Y).
    if (ImGui::CollapsingHeader("Espectrograma (Histórico de Frequências)", ImGuiTreeNodeFlags_DefaultOpen)) {
        
        int N_freqs = s_analyzers_data.size();
        int N_history = 200; // Tamanho fixo do histórico
        
        // Vetor para armazenar todos os dados do espectrograma (N_freqs * N_history)
        // Usamos float para a magnitude
        std::vector<float> heatmap_data(N_freqs * N_history);
        
        // Vetor para guardar as frequências em ordem para o eixo Y
        std::vector<float> freqs_y;

        // Preenche o vetor e o vetor de frequências
        int i = 0;
        for (auto& pair : s_analyzers_data) {
            float freq = pair.first;
            GoertzelAnalyzerData& data = pair.second;

            freqs_y.push_back(freq);

            // Copia o histórico de magnitudes para o vetor
            for (int j = 0; j < N_history; ++j) {
                // Calcula o índice no histórico para mapear o tempo corretamente:
                // O índice 'j' (0 a 199) representa o tempo. O dado mais antigo (0) vai para o índice de tempo mais baixo (0).
                int hist_idx = j - (N_history - data.spectrum_history.size());
                
                float magnitude = 0.0f;
                if (hist_idx >= 0 && hist_idx < data.spectrum_history.size()) {
                    magnitude = data.spectrum_history[hist_idx];
                }
                
                // Usa magnitude linear
                float magnitude_linear = magnitude;
                
                // O índice no vetor é calculado como: (índice da frequência * N_history) + índice do tempo
                heatmap_data[i * N_history + j] = magnitude_linear;
            }
            i++;
        }
        
        // Cria um vetor de pares (frequência, índice original)
        std::vector<std::pair<float, int>> freq_indices;
        for (int k = 0; k < N_freqs; ++k) {
            freq_indices.push_back({freqs_y[k], k});
        }
        
        // Ordena pelo valor da frequência
        std::sort(freq_indices.begin(), freq_indices.end(), [](const auto& a, const auto& b) {
            return a.first < b.first;
        });
        
        // Reorganiza o heatmap_data e freqs_y
        std::vector<float> sorted_heatmap_data(N_freqs * N_history);
        std::vector<float> sorted_freqs_y;
        
        for (int k = 0; k < N_freqs; ++k) {
            int original_index = freq_indices[k].second;
            float freq = freq_indices[k].first;
            
            // Copia a linha de dados (histórico) do índice original para a nova posição (k)
            std::copy(
                heatmap_data.begin() + original_index * N_history,
                heatmap_data.begin() + (original_index + 1) * N_history,
                sorted_heatmap_data.begin() + k * N_history
            );
            
            sorted_freqs_y.push_back(freq);
        }

        // Plotagem do espectrograma.
        // O PlotHeatmap usa índices no eixo Y para mostrar as frequências reais. 
        // O eixo X representa o tempo (0–200) e o Y mostra as frequências em ordem.
        const float plot_height = 400.0f;
        const float scale_width = 100.0f;
        const float w = ImGui::GetContentRegionAvail().x - scale_width - ImGui::GetStyle().ItemSpacing.x;
        
        if (ImPlot::BeginPlot("##Spectrogram", ImVec2(w, plot_height), ImPlotFlags_NoMouseText)) {
            
            ImPlot::SetupAxes("Tempo (Amostras)", "Frequência (Hz)");
            
            // Eixo X (Tempo): 0 (mais antigo) a 200 (mais recente)
            ImPlot::SetupAxisLimits(ImAxis_X1, 0, (double)N_history, ImGuiCond_Always); // TODO
            //ImPlot::SetupAxisLimits(ImAxis_X1, (double)N_history, 0, ImGuiCond_Always);

            
            // Eixo Y (Frequência): 0 a N_freqs
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, (double)N_freqs, ImGuiCond_Always);
            
            // Define os ticks do eixo Y para mostrar as frequências reais
            std::vector<double> y_ticks_pos(N_freqs);
            std::vector<std::string> y_ticks_labels_str(N_freqs);
            
            for (int k = 0; k < N_freqs; ++k) {
                y_ticks_pos[k] = (double)k + 0.5; // Centraliza o tick na "linha" do heatmap
                std::stringstream ss;
                ss << std::fixed << std::setprecision(2) << sorted_freqs_y[k];
                y_ticks_labels_str[k] = ss.str();
            }
            
            // Converte o vetor de std::string para o formato exigido pelo ImPlot (const char* const*)
            std::vector<const char*> y_ticks_labels_c_str = convert_to_c_str_array(y_ticks_labels_str);
            
            ImPlot::SetupAxisTicks(ImAxis_Y1, y_ticks_pos.data(), N_freqs, y_ticks_labels_c_str.data());

            // Plotagem do espectrograma
            ImPlot::PlotHeatmap(
                "##SpectrogramData",                    // Label
                sorted_heatmap_data.data(),             // Dados de magnitude em dB (Z)
                N_freqs,                                // Número de linhas (frequências)
                N_history,                              // Número de colunas (tempo/amostras)
                s_min_magnitude, s_max_magnitude,       // Limites Z (Magnitude Min/Max em dB)
                nullptr,                                // Formato do texto (opcional)
                ImPlotPoint(0, N_freqs),                // Ponto de início (X_min, Y_min)
                ImPlotPoint(N_history, 0)               // Ponto de fim (X_max, Y_max)
            );
            
            ImPlot::EndPlot();
        }
        
        ImGui::SameLine();
        
        // Barra de cores
        ImPlot::ColormapScale("##Scale", s_min_magnitude, s_max_magnitude, ImVec2(scale_width, plot_height), "%.2f");
        
        // Popup para controle de faixa de magnitude
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            ImGui::OpenPopup("Range");
            
        if (ImGui::BeginPopup("Range")) {
            ImGui::Text("Faixa de Magnitude Linear");
            ImGui::SliderDouble("Max", &s_max_magnitude, s_min_magnitude, 10.0);
            ImGui::SliderDouble("Min", &s_min_magnitude, 0.0, s_max_magnitude);
            ImGui::EndPopup();
        }
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
    ImGui::BeginChild(
        "ScrollableRegion", 
        ImVec2(0, 0), 
        false,          // sem borda
        ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar
    );

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

    ImGui::EndChild(); 
    
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
