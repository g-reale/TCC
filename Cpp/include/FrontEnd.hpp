#ifndef FRONTEND_HPP
#define FRONTEND_HPP

#include <string>
#include <vector>
#include <map>

struct GLFWwindow;

namespace FrontEnd {

    // Estrutura para armazenar os dados de cada analisador Goertzel
    struct GoertzelAnalyzerData {
        float frequency = 0.0f; // Frequência alvo para o algoritmo Goertzel
        bool running = false;   // Indica se a análise está em execução
        std::vector<float> spectrum_history; // Histórico dos valores de magnitude (para plot)
        double last_update_time = 0.0; // Momento da última atualização

        // Para o espectrograma simulado
        std::vector<std::vector<float>> spectrum_history_matrix; // Matriz para heatmap (espectrograma)
        int heatmap_history_size = 50; // Número de iterações para o heatmap
        int spectrum_bins = 10; // Número de bins de frequência para o espectrograma
    };

    // Classe principal do FrontEnd
    class Application {
    public:
        // Inicializa o frontend
        static void initialize(GLFWwindow* window);
        // Renderiza a interface do usuário
        static void render(GLFWwindow* window);
        // Limpa 
        static void cleanup();

        // Funções de gerenciamento de analisadores
        static void addAnalyzer(float frequency);
        static void removeAnalyzer(float frequency);
        static void startAnalyzer(float frequency);
        static void stopAnalyzer(float frequency);

        // Funções para seleção de fonte de áudio
        static void setAudioSource(const std::string& source);
        static std::vector<std::string> getAvailableAudioSources();

        // Função para limitar um float a duas casas decimais
        static float limitToTwoDecimals(float value);

    private:
        // Mapa para gerenciar múltiplas instâncias de GoertzelAnalyzerData por frequência
        static std::map<float, GoertzelAnalyzerData> s_analyzers_data;
        static float s_new_frequency_input; // Frequência a ser adicionada/removida
        static int s_selected_source_index; // Índice da fonte de áudio selecionada
        static std::vector<std::string> s_audio_sources; // Lista de fontes de áudio disponíveis

        // Páginas da interface
        static void showConfigurationPage();
        static void showVisualizationPage();
        static void showAudioSourceSelection();
        static void showOverallVisualizationPage();
    };

}

#endif


