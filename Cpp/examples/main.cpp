#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include <algorithm>
#include <stdio.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include <string>
#include <cstdio>

// Estrutura que armazena os dados da análise com Goertzel
// Simula os dados e o estado de um analisador espectral
struct GoertzelData {
    float frequency = 1000.0f;            // Frequência alvo para o algoritmo Goertzel
    int N = 256;                          // Tamanho do bloco de amostragem
    bool log_data = false;               // Indica se o log de dados está ativado
    bool running = false;                // Indica se a análise está em execução
    float magnitude = 0.0f;              // Resultado simulado da magnitude atual
    float latency = 0.0f;                // Latência de processamento
    float cpu_usage = 0.0f;              // Uso simulado da CPU
    std::vector<float> spectrum_history; // Histórico dos valores de magnitude (para plot)
    double last_update_time = 0.0;       // Momento da última atualização

    std::vector<std::vector<float>> spectrum_history_matrix; // Matriz para heatmap (espectrograma)
    const int heatmap_history_size = 50;  // Número de iterações para o heatmap
    const int spectrum_bins = 10;         // Número de bins de frequência para o espectrograma
};

// Callback para tratamento de erros do GLFW
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// Função que desenha a interface da página de configuração do analisador
void ShowConfigurationPage(GoertzelData& data) {
    ImGui::Text("Configuração do Analisador Goertzel");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Parâmetros");
    
    // Frequência alvo
    ImGui::SliderFloat("Frequência Alvo (Hz)", &data.frequency, 100.0f, 20000.0f, "%.1f Hz");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip(
            "Frequência que o algoritmo tentará detectar no sinal.\n"
            "Ideal para identificar tons específicos, como DTMF ou ruídos fixos.\n"
            "Você pode escolher entre 100 Hz e 20.000 Hz.");
    }

    // Tamanho do bloco (N)
    ImGui::InputInt("Tamanho do Bloco (N)", &data.N, 1, 1024);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip(
            "Quantidade de amostras analisadas por vez.\n"
            "Valores maiores tornam a análise mais precisa, porém mais lenta.\n"
            "Valores menores dão respostas mais rápidas, porém com menos precisão.\n"
            "Recomendado: entre 256 e 1024.");
    }

    // Validação do tamanho do bloco
    if (data.N < 1) data.N = 1;
    if (data.N > 8192) data.N = 8192;

    // Opção de log
    ImGui::Checkbox("Ativar Registro de Dados", &data.log_data);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Habilita o salvamento (simulado) dos resultados da análise para uso posterior.");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Controle de Execução");
    if (data.running) {
        if (ImGui::Button("Pausar Análise")) { data.running = false; }
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Interrompe temporariamente a análise espectral.");
    } else {
        if (ImGui::Button("Iniciar Análise")) { data.running = true; }
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Inicia a análise espectral com os parâmetros definidos.");
    }

    ImGui::SameLine();

    if (ImGui::Button("Resetar Análise")) { 
        data.running = false; 
        data.magnitude = 0.0f; 
        data.latency = 0.0f;
        data.cpu_usage = 0.0f;
        data.spectrum_history.clear();
        data.spectrum_history_matrix.clear();
        data.last_update_time = glfwGetTime();
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Para a análise e redefine todos os valores.");
    }

    ImGui::Spacing();
    ImGui::Separator();
}

// Função que desenha a interface da página de visualização em tempo real
void ShowVisualizationPage(GoertzelData& data) {
    ImGui::Text("Visualização em tempo real");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextWrapped(
        "Gráfico da magnitude ao longo do tempo: mostra a intensidade da frequência alvo detectada "
        "pelo algoritmo Goertzel. É útil para acompanhar como o sinal varia em tempo real, identificar "
        "picos e tendências.");
    ImGui::Spacing();

    // Métricas de desempenho
    ImGui::Text("Métricas de desempenho");
    ImGui::Text("Status: %s", data.running ? "Rodando" : "Pausado/Parado");
    char cpu_usage_text[32];
    snprintf(cpu_usage_text, sizeof(cpu_usage_text), "%.1f %%", data.cpu_usage);
    ImGui::ProgressBar(data.cpu_usage / 100.0f, ImVec2(-1.0f, 0.0f), cpu_usage_text);
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Uso estimado da CPU");
    ImGui::Text("Latência: %.1f ms", data.latency);
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Tempo de processamento por bloco");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    double current_time = glfwGetTime();
    if (data.running && (current_time - data.last_update_time > 0.05)) {
        data.magnitude = std::abs(sin(current_time * data.frequency / 5000.0f));
        data.latency = 5.0f + 2.0f * std::abs(sin(current_time * 1.5f));
        data.cpu_usage = 15.0f + 10.0f * std::abs(cos(current_time * 0.8f));
        if (data.spectrum_history.size() > 200)
            data.spectrum_history.erase(data.spectrum_history.begin());
        data.spectrum_history.push_back(data.magnitude);
        
        // Atualizar matriz do heatmap (espectrograma simulado)
        std::vector<float> current_spectrum(data.spectrum_bins);
        for (int i = 0; i < data.spectrum_bins; ++i) {
            float freq_offset = (i - data.spectrum_bins/2) * 100.0f;
            float distance = std::abs(freq_offset);
            current_spectrum[i] = data.magnitude * std::exp(-distance * 0.01f) + 
                                 0.1f * std::abs(sin(current_time * (i + 1) * 0.5f));
        }
        
        if (data.spectrum_history_matrix.size() >= data.heatmap_history_size) {
            data.spectrum_history_matrix.erase(data.spectrum_history_matrix.begin());
        }
        data.spectrum_history_matrix.push_back(current_spectrum);
        
        data.last_update_time = current_time;
    }

    // Gráfico 1: magnitude no tempo
    if (ImGui::CollapsingHeader("Magnitude ao Longo do Tempo", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImPlot::BeginPlot("##SpectrumMagnitude", ImVec2(-1, 300))) {
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
    if (ImGui::CollapsingHeader("Derivada Temporal da Magnitude")) {
        ImGui::TextWrapped(
            "Este gráfico mostra as variações instantâneas da magnitude, calculadas como a diferença "
            "entre amostras consecutivas. Útil para identificar mudanças rápidas e transientes no sinal.");
        ImGui::Spacing();
        
        if (ImPlot::BeginPlot("##MagnitudeDerivative", ImVec2(-1, 250))) {
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
    if (ImGui::CollapsingHeader("Média Móvel da Magnitude")) {
        ImGui::TextWrapped(
            "Este gráfico mostra a média móvel simples (janela de 10 amostras) da magnitude, "
            "suavizando a série temporal para evidenciar tendências e reduzir ruído.");
        ImGui::Spacing();
        
        if (ImPlot::BeginPlot("##MovingAverage", ImVec2(-1, 250))) {
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
    if (ImGui::CollapsingHeader("Espectrograma Simulado")) {
        ImGui::TextWrapped(
            "Este heatmap mostra a distribuição de magnitude em diferentes frequências ao longo do tempo, "
            "simulando um espectrograma. As cores indicam a intensidade da magnitude para cada frequência.");
        ImGui::Spacing();
        
        if (ImPlot::BeginPlot("##Spectrogram", ImVec2(-1, 300))) {
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

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Gráficos organizados em seções
    if (ImGui::CollapsingHeader("Desempenho do Sistema")) {
        // Gráfico de desempenho CPU e latência
        static std::vector<float> cpu_history;
        static std::vector<float> latency_history;
        static double last_perf_update = 0.0;
        const int max_points = 200;

        if (data.running && (current_time - last_perf_update > 0.05)) {
            if (cpu_history.size() > max_points) cpu_history.erase(cpu_history.begin());
            if (latency_history.size() > max_points) latency_history.erase(latency_history.begin());
            cpu_history.push_back(data.cpu_usage);
            latency_history.push_back(data.latency);
            last_perf_update = current_time;
        }

        ImGui::TextWrapped(
            "Gráfico de desempenho: mostra a evolução do uso de CPU e latência no processamento "
            "da análise. Ajuda a identificar se o algoritmo está consumindo muitos recursos ou "
            "levando muito tempo para processar.");
        ImGui::Spacing();

        if (ImPlot::BeginPlot("##PerformancePlot", ImVec2(-1, 200))) {
            ImPlot::SetupAxes("Tempo (amostras atrás)", "Valor");
            ImPlot::SetupAxisLimits(ImAxis_X1, -max_points, 0, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImGuiCond_Always);
            if (!cpu_history.empty()) {
                std::vector<float> x_perf(cpu_history.size());
                for (size_t i = 0; i < cpu_history.size(); ++i)
                    x_perf[i] = -(float)(cpu_history.size() - 1 - i);
                ImPlot::PlotLine("Uso de CPU (%)", x_perf.data(), cpu_history.data(), cpu_history.size());
            }
            if (!latency_history.empty()) {
                std::vector<float> x_lat(latency_history.size());
                for (size_t i = 0; i < latency_history.size(); ++i)
                    x_lat[i] = -(float)(latency_history.size() - 1 - i);
                ImPlot::PlotLine("Latência (ms)", x_lat.data(), latency_history.data(), latency_history.size());
            }
            ImPlot::EndPlot();
        }
    }

    if (ImGui::CollapsingHeader("Espectro Completo Simulado")) {
        ImGui::TextWrapped(
            "Espectro completo simulado: mostra a magnitude de várias frequências próximas da frequência alvo. "
            "Útil para visualizar a distribuição de energia em frequências vizinhas e detectar possíveis interferências.");
        ImGui::Spacing();

        static std::vector<float> spectrum(10, 0.0f);
        static std::vector<float> freq_bins(10, 0.0f);
        for (int i = 0; i < 10; ++i) {
            freq_bins[i] = data.frequency + (i - 5) * 100.0f;
            float distance = std::abs(freq_bins[i] - data.frequency);
            spectrum[i] = data.magnitude * std::exp(-distance * 0.005f);
        }

        if (ImPlot::BeginPlot("##FullSpectrum", ImVec2(-1, 200))) {
            ImPlot::SetupAxes("Frequência (Hz)", "Magnitude");
            ImPlot::SetupAxisLimits(ImAxis_X1, freq_bins.front(), freq_bins.back(), ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1.5, ImGuiCond_Always);
            ImPlot::PlotBars("Magnitude", freq_bins.data(), spectrum.data(), (int)spectrum.size(), 0.7);
            ImPlot::EndPlot();
        }
    }

    if (ImGui::CollapsingHeader("Histograma da Magnitude")) {
        ImGui::TextWrapped(
            "Histograma da magnitude: mostra a distribuição das magnitudes coletadas recentemente. "
            "Ajuda a avaliar a estabilidade do sinal e a presença de ruído.");
        ImGui::Spacing();

        static std::vector<int> hist_bins(10, 0);
        std::fill(hist_bins.begin(), hist_bins.end(), 0);
        for (float v : data.spectrum_history) {
            int bin = (int)(v / 1.5f * hist_bins.size());
            if (bin >= (int)hist_bins.size()) bin = (int)hist_bins.size() - 1;
            hist_bins[bin]++;
        }

        std::vector<double> bin_positions(hist_bins.size());
        std::vector<double> hist_bins_double(hist_bins.size());
        for (size_t i = 0; i < hist_bins.size(); ++i) {
            bin_positions[i] = (double)i / hist_bins.size() * 1.5;
            hist_bins_double[i] = (double)hist_bins[i];
        }

        if (ImPlot::BeginPlot("##Histogram", ImVec2(-1, 150))) {
            ImPlot::SetupAxes("Magnitude", "Contagem");
            ImPlot::SetupAxisLimits(ImAxis_X1, 0, 1.5, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, *std::max_element(hist_bins_double.begin(), hist_bins_double.end()) + 5, ImGuiCond_Always);
            ImPlot::PlotBars("Histograma", bin_positions.data(), hist_bins_double.data(), (int)hist_bins_double.size(), 1.0);
            ImPlot::EndPlot();
        }
    }

    if (ImGui::CollapsingHeader("Comparação com Limiar")) {
        ImGui::TextWrapped(
            "Comparação da magnitude atual com um limiar (threshold): facilita a visualização rápida "
            "se o sinal está acima do nível considerado significativo.");
        ImGui::Spacing();

        const float threshold = 0.5f;
        double positions[2] = { 0.0, 1.0 };
        double values[2] = { (double)data.magnitude, (double)threshold };
        const char* labels[2] = { "Magnitude", "Limiar" };

        if (ImPlot::BeginPlot("##MagnitudeVsThreshold", ImVec2(-1, 100))) {
            ImPlot::SetupAxes("Estado", "Valor");
            ImPlot::SetupAxisLimits(ImAxis_X1, -1, 2, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1.5, ImGuiCond_Always);
            ImPlot::SetupAxisTicks(ImAxis_X1, positions, 2, labels, true);
            ImPlot::PlotBars("Valores", positions, values, 2, 0.5);
            ImPlot::EndPlot();
        }
    }
}

int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Goertzel Spectrum Analyzer GUI", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Estilo
    ImGui::StyleColorsDark();

    // Inicializa backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Estado inicial do programa
    ImVec4 clear_color = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    GoertzelData goertzel_data;
    goertzel_data.last_update_time = glfwGetTime();
    int current_page = 0; // 0 = Configuração, 1 = Visualização

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents(); // Captura eventos de teclado, mouse etc.

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Menu superior para navegar entre páginas
        bool config_selected = (current_page == 0);
        bool viz_selected = (current_page == 1);
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Navigation")) {
                if (ImGui::MenuItem("Configuration", "Ctrl+C", &config_selected)) { current_page = 0; }
                if (ImGui::MenuItem("Visualization", "Ctrl+V", &viz_selected)) { current_page = 1; }
                ImGui::EndMenu();
            }

            // Mostra nome da página atual à direita
            const char* page_name = (current_page == 0) ? "Configuration" : "Visualization";
            float page_name_width = ImGui::CalcTextSize(page_name).x;
            ImGui::SameLine(ImGui::GetWindowWidth() - page_name_width - 10);
            ImGui::Text("%s", page_name);
            ImGui::EndMainMenuBar();
        }

        const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y + ImGui::GetFrameHeight()));
        ImGui::SetNextWindowSize(ImVec2(main_viewport->WorkSize.x, main_viewport->WorkSize.y - ImGui::GetFrameHeight()));
        ImGui::Begin("MainWindow", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus |
                     ImGuiWindowFlags_NoNavFocus);

        // Renderiza a página selecionada
        if (current_page == 0) {
            ShowConfigurationPage(goertzel_data);
        } else {
            ShowVisualizationPage(goertzel_data);
        }

        ImGui::End(); 

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}