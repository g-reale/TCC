[English](#dominant-frequency-extraction-using-area-based-binary-search) | [Português](#extração-de-frequência-dominante-por-busca-binária-baseada-em-área)

# Dominant Frequency Extraction using Area-Based Binary Search

This repository contains the source code, academic documentation, and experimental validation for Gabriel Reale's undergraduate thesis (TCC) at the **Federal University of São Paulo (UNIFESP)**. The thesis proposes a Digital Signal Processing (DSP) algorithm named **Wide Binary Area Search (WBASS)** with $O(n)$ linear complexity to estimate the dominant frequency of a discrete-time signal.

---


## Overview

In digital signal processing, extracting the dominant frequency, defined as the frequency of maximum magnitude in a signal's spectrum, is traditionally solved using the Fast Fourier Transform (FFT). The standard FFT-based method computes the full spectrum and then searches for the peak magnitude, yielding an overall $O(n \log n)$ complexity.

This project implements two alternative algorithms that target only the dominant frequency:
1. **Wide Binary Area Search (WBASS)**: An algorithm analyzed as having $O(n)$ complexity, which iteratively reduces the frequency search space using complementary low-pass/high-pass filtering, Parseval's energy theorem, frequency modulation, and decimation.
2. **Binary Area Search (BASS)**: A predecessor algorithm that approximates spectral energy over specific frequency intervals using the Goertzel algorithm to guide a binary search.

The repository also includes the **Goertzel Analyzer** GUI application, a real-time spectral visualization tool developed during a prior Scientific Initiation (IC) project to monitor audio inputs via PulseAudio.

---

## Motivation

Real-time signal analysis on embedded systems is constrained by processing power and memory. While the FFT is highly versatile, it introduces computational overhead of $O(n \log n)$ when the only requirement is to extract a single dominant frequency.

An algorithm with $O(n)$ complexity could potentially enable more efficient execution in resource-constrained scenarios, such as low-power microcontrollers or dedicated real-time monitoring systems. 

---

## Project Context

This repository represents the practical implementation and documentation of **Gabriel Reale's** undergraduate thesis (TCC) at UNIFESP, under the academic supervision of **Prof. Dr. André Marcorin de Oliveira**.

---

## Main Contributions

* **Linear Asymptotic Complexity**: WBASS is analyzed as an algorithm with $O(n)$ complexity, which belongs to a lower asymptotic class than the FFT-based peak search ($O(n \log n)$).
* **Filter-Driven Search Space Reduction**: Instead of computing a full spectral transform, WBASS performs binary search directly on the signal energy using complementary filters.
* **Supporting Real-Time Visualization Tool**: Includes an OpenGL/GLFW interface powered by Dear ImGui and ImPlot, enabling dynamic frequency analyzer management and simultaneous algorithm comparison.
* **Formal Analysis**: The thesis includes proofs of convergence (for pure tones with ideal filters) and asymptotic complexity analysis.

---

## Technical Background

### 1. The Dominant Frequency Problem
For a discrete-time signal $x[n]$ and its Discrete-Time Fourier Transform (DTFT) $X(\Omega)$:
$$X(\Omega) = \sum_{k = -\infty}^{\infty} x[k] e^{j\Omega k}$$
The goal is to find the frequency $\Omega_m$ of maximum magnitude:
$$\Omega_m \triangleq \underset{\Omega \in [-\pi,\pi]}{\arg \max} \lvert X(\Omega) \rvert$$

### 2. The Goertzel Algorithm (with Pole Stabilization)
The Goertzel algorithm computes the DFT magnitude for a single frequency. It is structured as a second-order IIR filter followed by a first-order FIR filter.

The system is marginally stable because its poles lie on the unit circle, which can cause instability in digital implementations due to quantization effects. A stability factor $r$ (set to $0.9995$ in the implementation) is introduced to pull the poles inside the unit circle:
$$s[n] = x[n] + 2r\cos(\Omega)s[n-1] - r^2s[n-2]$$
$$\lvert y[N-1] \rvert^2 = (s[N-1] - r\cos(\Omega)s[N-2])^2 + (r\sin(\Omega)s[N-2])^2$$

### 3. Wide Binary Area Search (WBASS)
WBASS applies a binary search strategy on the frequency spectrum:
* **Analytic Signal**: The real input signal is converted to a complex analytic signal using a filter that removes negative-frequency components.
* **Complementary Filtering**: The bandwidth $[0, \pi]$ is divided into a lower band $[0, \pi/2)$ and an upper band $[\pi/2, \pi]$ using complementary low-pass and high-pass filters.
* **Energy Decision (Parseval's Theorem)**: The time-domain energies of the filtered signals are compared. By Parseval's theorem, these are proportional to the spectral energies in each band.
* **Decimation and Frequency Shift**: If the lower band contains more energy, the signal is decimated by 2. If the upper band contains more energy, the signal is first frequency-shifted down by $\pi/2$ (multiplication by a complex exponential) and then decimated by 2.
* **Iteration**: This process repeats iteratively $\log_2(n)$ times. At each step, the search interval is halved. The relationship between the observed frequency and the original frequency is given by:
  $$\Omega_m^{k+1} = \begin{cases} 2\Omega_m^k & \text{if } E_a^k < E_b^k \\ 2\Omega_m^k - \pi & \text{if } E_b^k \le E_a^k \end{cases}$$

where $E_a^k$ and $E_b^k$ are the energies of the high-pass and low-pass filtered signals at iteration $k$, respectively.

The thesis proves convergence for pure tones assuming ideal filters.

---

## Repository Structure

```text
.
├── tcc_gabriel_reale.pdf                        # Gabriel Reale's final thesis document
└── g_TCC
    ├── goertzel_real_time_visualization_interface.pdf # Scientific Initiation paper
    ├── Cpp/                                     # C++ source code & build files
    │   ├── CMakeLists.txt                       # Build configuration
    │   ├── install.sh                           # Dependency install script
    │   ├── main.cpp                             # App entry point
    │   ├── testbench.cpp                        # Benchmark suite
    │   ├── include/                             # Header files (.hpp)
    │   ├── src/                                 # Implementation files (.cpp)
    │   ├── templates/                           # C++ template implementations (.tpp)
    │   ├── filters/                             # MATLAB/Octave scripts and filter coefficients (.txt)
    │   ├── results/                             # Saved CSV files from testbench execution
    │   ├── imgui/                               # Dear ImGui library submodule
    │   └── implot/                              # Dear ImPlot library submodule
    └── text/                                    # LaTeX documentation files
        ├── tcc.tex                              # Main LaTeX compiler file
        ├── tcc.pdf                              # Compiled LaTeX PDF
        ├── images/                              # Diagrams and GUI screenshots
        └── chapters/                            # Individual LaTeX chapters (.tex)
```

---

## Architecture

The system follows a modular architecture separating audio acquisition (PulseAudio), digital signal processing (Backend), and interactive rendering (Frontend).

### System Data Flow
![System Data Flow](/Images/system_data_flow.png)

### WBASS Execution Loop
![WBASS Execution Loop](/Images/wbass_execution_loop.png)

<br>

---

## Implementation Details

* **Language Standard**: Built on the **C++20** standard (e.g., template-based `std::array` compile-time size allocation).
* **Filter Modules**: Core DSP routines use difference equations mapped through `Filter` classes that load FIR coefficients from text files (`lpf.txt`, `hpf.txt`, `analytic.txt`).
* **Third-Party Libraries**:
  * **PulseAudio**: Used for capturing audio output streams.
  * **liquid-dsp**: Provides the FFT implementation (`fft_plan`) used as the comparative baseline.
  * **GLFW/OpenGL/Glad**: Handles windows, input events, and rendering contexts.
  * **Dear ImGui / ImPlot**: Implements immediate-mode UI layouts and plots.

---

## Scientific Initiation Background

The Goertzel Analyzer application was originally developed within a Scientific Initiation (IC) project led technically by **Gabriel Reale**, who was responsible for the backend, signal-processing logic, PulseAudio capture, and Goertzel analyzer management.

**Fernanda Bucheri** contributed to the construction of the frontend interface, primarily working on the Dear ImGui and ImPlot visualization layers, under Gabriel's technical guidance.

Both **Gabriel Reale** and **Fernanda Bucheri** were academically supervised by **Prof. Dr. André Marcorin de Oliveira** during the Scientific Initiation project.

The resulting application provides an interactive interface for configuring the audio analysis process. Users can select the PulseAudio input source, add new frequencies, and manage the active Goertzel analyzers.

![Goertzel Analyzer Configuration](/Images/config_screen.png)

The main visualization compares the magnitude measured by the Goertzel analyzers with the peak-frequency estimates from each algorithm.

![Real-time Algorithm Comparison](/Images/spectrum_stems.png)

The interface also provides a magnitude-over-time view for each active frequency analyzer, showing the most recent 200 magnitude values.

![Magnitude Over Time](/Images/magnitude_over_time.png)

A scrolling spectrogram displays the magnitude history of all active frequency channels, making it easier to observe spectral changes over time.

![Spectrogram Heatmap](/Images/spectrogram_heatmap.png)

In Gabriel's subsequent undergraduate thesis, this core infrastructure was expanded to include the implementation, validation, and benchmarking of BASS and WBASS.

---

## Academic Documentation

The folder `g_TCC/text/` contains the complete LaTeX setup to compile Gabriel Reale's thesis:
* `introdução.tex`: Motivation, goals, and problem definition.
* `fundamentação.tex`: Mathematical foundations (DFT, FFT, Nyquist, Parseval, Goertzel).
* `desenvolvimento.tex`: Algorithm descriptions (BASS/WBASS), proofs, and software architecture.
* `references.bib`: BibTeX references.

---

## How to Build

### Dependencies
Before compiling, install the system dependencies (PulseAudio, GLFW3, OpenGL, and Glad headers).

Run the provided install script:
```bash
cd g_TCC/Cpp
chmod +x install.sh
./install.sh
```
*Note: The script contains a typo on line 1 (`uptade` instead of `update`). Correct it before running, or run `sudo apt update` manually first. The script installs dependencies via `apt-get` and copies Glad headers to `/usr/include/`.*

### Compilation
Configure the project with CMake. You can toggle performance optimizations:

* **Performance Mode (recommended for benchmarking)**:
  ```bash
  mkdir -p build && cd build
  cmake -DPERFORMANCE=ON ..
  make
  ```
  *Compiles with `-O3 -march=native` flags.*

* **Debug Mode**:
  ```bash
  mkdir -p build && cd build
  cmake -DPERFORMANCE=OFF ..
  make
  ```
  *Compiles with `-fsanitize=address` for memory error detection.*

---

## How to Run

### Main GUI Application
After building, execute the main binary:
```bash
./BAS
```
This launches the interactive GUI with tabs for audio configuration (*Configurações*), overall spectrum and spectrogram (*Visualização Geral*), and per-analyzer magnitude timeline (*Visualização por Analisador*).

### Benchmark Suite
To run the automated asymptotic complexity benchmarks:
```bash
./testbench
```
This benchmarks WBASS, BASS, and FFT across array sizes from $2^4$ to $2^{17}$ samples, with 100 repetitions per size. The output timings are written as semicolon-separated values to `results/` (`wbass.csv`, `bass.csv`, `fft.csv`).

---

## Experimental Results

### Performance and Complexity

The empirical execution-time analysis was performed using log-log plots. The results are consistent with the expected asymptotic behavior: BASS and WBASS exhibit approximately linear growth $O(n)$, while the FFT follows the expected $O(n \log n)$ behavior.

The regression slopes on the log-log scale were approximately $1$ for BASS and WBASS and approximately $1.15$ for the FFT.

---

## Skills and Technologies Demonstrated

* **Programming**: Modern C++ (C++20), CMake, Template Metaprogramming.
* **Digital Signal Processing**: Goertzel Filters, FFT, Hilbert Transform / Analytic Signals, Decimation, Frequency Modulation.
* **Graphics & UI**: Dear ImGui, ImPlot, GLFW, OpenGL.
* **Audio Engineering**: PulseAudio, Real-time Audio Buffering.
* **Data Analysis**: Benchmarking, MATLAB/Octave filter design (`fir1`, `freqz`, linear regressions).
* **Research**: Academic writing, LaTeX, Technical validation.

---

## Potential Applications

* **Real-time Frequency Monitoring**: Monitoring specific target frequencies in telemetry or acoustic sensing applications.
* **Low-Power Embedded Devices**: Dominant frequency detection on microcontrollers with limited processing resources (potential application, not experimentally validated in this work).
* **Educational Toolkits**: Interactive visualization of signal spectra and Goertzel filter characteristics.

---

## How to Cite

If you use this repository, the WBASS algorithm, or the associated academic documentation, please cite this work using the ABNT format below.

### ABNT

OLIVEIRA, Gabriel Reale Marques de. **Dominant Frequency Extraction using Area-Based Binary Search**. 2026. Undergraduate Thesis (Computer Engineering) — Federal University of São Paulo, São José dos Campos, 2026. Available at: https://github.com/g-reale/TCC. Accessed on: July 2026.

### IEEE

G. R. M. de Oliveira, “Dominant Frequency Extraction using Area-Based Binary Search,” Undergraduate Thesis, Computer Engineering, Federal University of São Paulo, São José dos Campos, Brazil, 2026. [Online]. Available: https://github.com/g-reale/TCC. Accessed: July 2026.

---

## References

1. Cooley, J. W., & Tukey, J. W. (1965). *An algorithm for the machine calculation of complex Fourier series*. Mathematics of Computation, 19(90), 297–301.
2. Goertzel, G. (1958). *An Algorithm for the Evaluation of Finite Trigonometric Series*. The American Mathematical Monthly, 65(1), 34–35.
3. Oppenheim, A. V., Willsky, A. S., & Nawab, S. H. (1997). *Signals and Systems* (2nd ed.). Prentice Hall.
4. Bucheri, F. S., Reale, G., & Oliveira, A. M. (2024). *Development of an Interactive Graphical Interface for Real-Time Visualization of the Goertzel Algorithm*. Scientific Initiation, UNIFESP.


---

<br>
<br>
<br>

# Extração de Frequência Dominante por Busca Binária Baseada em Área

Este repositório contém o código-fonte, a documentação acadêmica e a validação experimental para o Trabalho de Conclusão de Curso (TCC) de Gabriel Reale na **Universidade Federal de São Paulo (UNIFESP)**. O trabalho propõe um algoritmo de Processamento Digital de Sinais (DSP) denominado **Wide Binary Area Search (WBASS)** com complexidade linear $O(n)$ para estimar a frequência dominante de um sinal em tempo discreto.

---

## Visão Geral

No processamento digital de sinais, a extração da frequência dominante, definida como a frequência de máximo módulo no espectro de um sinal, é tradicionalmente resolvida utilizando a Transformada Rápida de Fourier (FFT). O método padrão baseado em FFT calcula o espectro completo e, em seguida, busca pelo pico de magnitude, resultando em uma complexidade geral de $O(n \log n)$.

Este projeto implementa dois algoritmos alternativos que visam apenas a frequência dominante:
1. **Wide Binary Area Search (WBASS)**: Um algoritmo analisado como tendo complexidade $O(n)$, que reduz iterativamente o espaço de busca de frequências utilizando filtragem complementar passa-baixas/passa-altas, o teorema de energia de Parseval, modulação de frequência e decimação.
2. **Binary Area Search (BASS)**: Um algoritmo predecessor que aproxima a energia espectral sobre intervalos de frequência específicos utilizando o algoritmo de Goertzel para guiar uma busca binária.

O repositório também inclui a aplicação gráfica **Goertzel Analyzer**, uma ferramenta de visualização espectral em tempo real desenvolvida durante um projeto anterior de Iniciação Científica (IC) para monitorar entradas de áudio via PulseAudio.

---

## Motivação

A análise de sinais em tempo real em sistemas embarcados é limitada pelo poder de processamento e pela memória. Embora a FFT seja altamente versátil, ela introduz um custo computacional de $O(n \log n)$ quando o único requisito é extrair uma única frequência dominante.

Um algoritmo com complexidade $O(n)$ poderia potencialmente permitir uma execução mais eficiente em cenários com recursos limitados, como microcontroladores de baixo consumo ou sistemas dedicados de monitoramento em tempo real.

---


## Contexto do Projeto

Este repositório representa a implementação prática e a documentação do trabalho de conclusão de curso (TCC) de **Gabriel Reale** na UNIFESP, sob a supervisão acadêmica do **Prof. Dr. André Marcorin de Oliveira**.

---

## Principais Contribuições

* **Complexidade Assintótica Linear**: O WBASS é analisado como um algoritmo de complexidade $O(n)$, pertencente a uma classe assintótica inferior à busca de pico baseada em FFT ($O(n \log n)$).
* **Redução do Espaço de Busca Guiada por Filtros**: Em vez de computar uma transformada espectral completa, o WBASS realiza a busca binária diretamente na energia do sinal utilizando filtros complementares.
* **Ferramenta de Apoio para Visualização em Tempo Real**: Inclui uma interface OpenGL/GLFW alimentada por Dear ImGui e ImPlot, permitindo o gerenciamento dinâmico de analisadores de frequência e a comparação simultânea de algoritmos.
* **Análise Formal**: O TCC inclui provas de convergência (para tons puros com filtros ideais) e análise de complexidade assintótica.

---

## Fundamentação Teórica

### 1. O Problema da Frequência Dominante
Para um sinal em tempo discreto $x[n]$ e sua Transformada de Fourier de Tempo Discreto (DTFT) $X(\Omega)$:
$$X(\Omega) = \sum_{k = -\infty}^{\infty} x[k] e^{j\Omega k}$$
O objetivo é encontrar a frequência $\Omega_m$ de máximo módulo:
$$\Omega_m \triangleq \underset{\Omega \in [-\pi,\pi]}{\arg \max} \lvert X(\Omega) \rvert$$

### 2. O Algoritmo de Goertzel (com Estabilização de Polos)
O algoritmo de Goertzel calcula a magnitude da DFT para uma única frequência. Ele é estruturado como um filtro IIR de segunda ordem seguido por um filtro FIR de primeira ordem.

O sistema é marginalmente estável porque seus polos estão sobre o círculo unitário, o que pode causar instabilidade em implementações digitais devido a efeitos de quantização. Um fator de estabilidade $r$ (definido como $0,9995$ na implementação) é introduzido para deslocar os polos para dentro do círculo unitário:
$$s[n] = x[n] + 2r\cos(\Omega)s[n-1] - r^2s[n-2]$$
$$\lvert y[N-1] \rvert^2 = (s[N-1] - r\cos(\Omega)s[N-2])^2 + (r\sin(\Omega)s[N-2])^2$$

### 3. Wide Binary Area Search (WBASS)
O WBASS aplica uma estratégia de busca binária no espectro de frequência:
* **Sinal Analítico**: O sinal real de entrada é convertido em um sinal analítico complexo usando um filtro que remove os componentes de frequência negativa.
* **Filtragem Complementar**: A banda $[0, \pi]$ é dividida em uma banda inferior $[0, \pi/2)$ e uma banda superior $[\pi/2, \pi]$ usando filtros complementares passa-baixas e passa-altas.
* **Decisão de Energia (Teorema de Parseval)**: As energias no domínio do tempo dos sinais filtrados são comparadas. Pelo teorema de Parseval, elas são proporcionais às energias espectrais em cada banda.
* **Decimação e Deslocamento de Frequência**: Se a banda inferior contiver mais energia, o sinal é decimado por 2. Se a banda superior contiver mais energia, o sinal é primeiro deslocado em frequência para baixo por $\pi/2$ (multiplicação por uma exponencial complexa) e depois decimado por 2.
* **Iteração**: Esse processo é repetido iterativamente $\log_2(n)$ vezes. A cada passo, o intervalo de busca é reduzido pela metade. A relação entre a frequência observada e a frequência original é dada por:
  $$\Omega_m^{k+1} = \begin{cases} 2\Omega_m^k & \text{se } E_a^k < E_b^k \\ 2\Omega_m^k - \pi & \text{se } E_b^k \le E_a^k \end{cases}$$

onde $E_a^k$ e $E_b^k$ são as energias dos sinais filtrados por passa-altas e passa-baixas na iteração $k$, respectivamente.

O TCC prova a convergência para tons puros assumindo filtros ideais.

---

## Estrutura do Repositório

```text
.
├── tcc_gabriel_reale.pdf                        # Documento final do TCC de Gabriel Reale
└── g_TCC
    ├── goertzel_real_time_visualization_interface.pdf # Artigo da Iniciação Científica
    ├── Cpp/                                     # Código-fonte em C++ e arquivos de compilação
    │   ├── CMakeLists.txt                       # Configuração do CMake
    │   ├── install.sh                           # Script de instalação de dependências
    │   ├── main.cpp                             # Ponto de entrada do aplicativo
    │   ├── testbench.cpp                        # Suite de benchmarks
    │   ├── include/                             # Arquivos de cabeçalho (.hpp)
    │   ├── src/                                 # Arquivos de implementação (.cpp)
    │   ├── templates/                           # Implementações de templates C++ (.tpp)
    │   ├── filters/                             # Scripts MATLAB/Octave e coeficientes de filtros (.txt)
    │   ├── results/                             # Arquivos CSV salvos da execução do testbench
    │   ├── imgui/                               # Submódulo da biblioteca Dear ImGui
    │   └── implot/                              # Submódulo da biblioteca Dear ImPlot
    └── text/                                    # Arquivos de documentação em LaTeX
        ├── tcc.tex                              # Arquivo principal de compilação LaTeX
        ├── tcc.pdf                              # PDF compilado do LaTeX
        ├── images/                              # Diagramas e capturas de tela da GUI
        └── chapters/                            # Capítulos individuais em LaTeX (.tex)
```

---

## Arquitetura

O sistema segue uma arquitetura modular que separa a aquisição de áudio (PulseAudio), o processamento digital de sinais (Backend) e a renderização interativa (Frontend).

### Fluxo de Dados do Sistema
![Fluxo de Dados do Sistema](/Images/system_data_flow.png)

<br>

### Loop de Execução do WBASS
![Loop de Execução do WBASS](/Images/wbass_execution_loop.png)

---

## Detalhes de Implementação

* **Padrão de Linguagem**: Desenvolvido sob o padrão **C++20** (por exemplo, alocação de tamanho em tempo de compilação baseada em templates `std::array`).
* **Módulos de Filtro**: As rotinas principais de DSP usam equações de diferenças mapeadas através de classes `Filter` que carregam coeficientes FIR de arquivos de texto (`lpf.txt`, `hpf.txt`, `analytic.txt`).
* **Bibliotecas de Terceiros**:
  * **PulseAudio**: Usada para capturar fluxos de saída de áudio.
  * **liquid-dsp**: Fornece a implementação da FFT (`fft_plan`) usada como linha de base comparativa.
  * **GLFW/OpenGL/Glad**: Gerencia janelas, eventos de entrada e contextos de renderização.
  * **Dear ImGui / ImPlot**: Implementa layouts de interface de usuário de modo imediato e gráficos.

---

## Contexto de Iniciação Científica

O aplicativo Goertzel Analyzer foi originalmente desenvolvido no contexto de um projeto de Iniciação Científica (IC) conduzido tecnicamente por **Gabriel Reale**, responsável pelo backend, pela lógica de processamento de sinais, pela captura via PulseAudio e pelo gerenciamento dos analisadores Goertzel.

**Fernanda Bucheri** contribuiu para a construção do frontend da interface, atuando principalmente nas camadas de visualização com Dear ImGui e ImPlot, sob a orientação técnica de Gabriel.

Tanto **Gabriel Reale** quanto **Fernanda Bucheri** foram supervisionados academicamente pelo **Prof. Dr. André Marcorin de Oliveira** durante o projeto de Iniciação Científica.

A aplicação resultante fornece uma interface interativa para configurar o processo de análise de áudio. Os usuários podem selecionar a fonte de entrada PulseAudio, adicionar novas frequências e gerenciar os analisadores Goertzel ativos.

![Configuração do Analisador Goertzel](/Images/config_screen.png)

A visualização principal compara a magnitude medida pelos analisadores Goertzel com as estimativas de frequência de pico de cada algoritmo.

![Comparação de Algoritmos em Tempo Real](/Images/spectrum_stems.png)

A interface também fornece uma visualização de magnitude ao longo do tempo para cada analisador de frequência ativo, mostrando os 200 valores de magnitude mais recentes.

![Magnitude ao Longo do Tempo](/Images/magnitude_over_time.png)

Um espectrograma rolante exibe o histórico de magnitude de todos os canais de frequência ativos, tornando mais fácil observar as mudanças espectrais ao longo do tempo.

![Mapa de Calor do Espectrograma](/Images/spectrogram_heatmap.png)

No trabalho de conclusão de curso subsequente de Gabriel, essa infraestrutura central foi expandida para incluir a implementação, validação e benchmark do BASS e WBASS.

---

## Documentação Acadêmica

A pasta `g_TCC/text/` contém a estrutura completa em LaTeX para compilar o TCC de Gabriel Reale:
* `introdução.tex`: Motivação, objetivos e definição do problema.
* `fundamentação.tex`: Fundamentos matemáticos (DFT, FFT, Nyquist, Parseval, Goertzel).
* `desenvolvimento.tex`: Descrição dos algoritmos (BASS/WBASS), demonstrações e arquitetura do software.
* `references.bib`: Referências do BibTeX.

---

## Como Compilar

### Dependências
Antes de compilar, instale as dependências do sistema (PulseAudio, GLFW3, OpenGL e cabeçalhos Glad).

Execute o script de instalação fornecido:
```bash
cd g_TCC/Cpp
chmod +x install.sh
./install.sh
```
*Nota: O script contém um erro de digitação na linha 1 (`uptade` em vez de `update`). Corrija-o antes de executar ou execute `sudo apt update` manualmente primeiro. O script instala as dependências via `apt-get` e copia os cabeçalhos Glad para `/usr/include/`.*

### Compilação
Configure o projeto com o CMake. Você pode alternar as otimizações de desempenho:

* **Modo de Desempenho (recomendado para benchmarks)**:
  ```bash
  mkdir -p build && cd build
  cmake -DPERFORMANCE=ON ..
  make
  ```
  *Compila com as flags `-O3 -march=native`.*

* **Modo de Depuração**:
  ```bash
  mkdir -p build && cd build
  cmake -DPERFORMANCE=OFF ..
  make
  ```
  *Compila com `-fsanitize=address` para detecção de erros de memória.*

---

## Como Executar

### Aplicativo Gráfico Principal
Após a compilação, execute o binário principal:
```bash
./BAS
```
Isso inicia a interface gráfica interativa com abas para configuração de áudio (*Configurações*), espectro geral e espectrograma (*Visualização Geral*) e linha do tempo de magnitude por analisador (*Visualização por Analisador*).

### Suite de Benchmarks
Para executar os benchmarks automatizados de complexidade assintótica:
```bash
./testbench
```
Isso avalia o WBASS, o BASS e a FFT em tamanhos de array de $2^4$ a $2^{17}$ amostras, com 100 repetições por tamanho. Os tempos resultantes são gravados como valores separados por ponto e vírgula na pasta `results/` (`wbass.csv`, `bass.csv`, `fft.csv`).

---

## Resultados Experimentais

### Desempenho e Complexidade

A análise empírica do tempo de execução foi realizada utilizando gráficos log-log. Os resultados são consistentes com o comportamento assintótico esperado: o BASS e o WBASS exibem crescimento aproximadamente linear $O(n)$, enquanto a FFT segue o comportamento esperado de $O(n \log n)$.

As inclinações das regressões na escala log-log foram de aproximadamente $1$ para o BASS e o WBASS, e de aproximadamente $1,15$ para a FFT.

---

## Habilidades e Tecnologias Demonstradas

* **Programação**: C++ Moderno (C++20), CMake, Metaprogramação de Templates.
* **Processamento Digital de Sinais**: Filtros Goertzel, FFT, Transformada de Hilbert / Sinais Analíticos, Decimação, Modulação de Frequência.
* **Gráficos e Interface de Usuário**: Dear ImGui, ImPlot, GLFW, OpenGL.
* **Engenharia de Áudio**: PulseAudio, Buffer de Áudio em Tempo Real.
* **Análise de Dados**: Benchmarking, projeto de filtros no MATLAB/Octave (`fir1`, `freqz`, regressões lineares).
* **Pesquisa**: Escrita acadêmica, LaTeX, validação técnica.

---

## Aplicações Potenciais

* **Monitoramento de Frequência em Tempo Real**: Monitoramento de frequências-alvo específicas em aplicações de telemetria ou sensoriamento acústico.
* **Dispositivos Embarcados de Baixo Consumo**: Detecção de frequência dominante em microcontroladores com recursos de processamento limitados (aplicação potencial, não validada experimentalmente neste trabalho).
* **Conjuntos de Ferramentas Educacionais**: Visualização interativa de espectros de sinais e características de filtros Goertzel.

---

## Como Citar

Se você utilizar este repositório, o algoritmo WBASS ou a documentação acadêmica associada, cite este trabalho preferencialmente no formato ABNT.

### ABNT

OLIVEIRA, Gabriel Reale Marques de. **Extração de Frequência Dominante por Busca Binária Baseada em Área**. 2026. Trabalho de Conclusão de Curso (Graduação em Engenharia de Computação) — Universidade Federal de São Paulo, São José dos Campos, 2026. Disponível em: https://github.com/g-reale/TCC. Acesso em: julho de 2026.

### IEEE

G. R. M. de Oliveira, “Extração de Frequência Dominante por Busca Binária Baseada em Área,” Trabalho de Conclusão de Curso, Engenharia de Computação, Universidade Federal de São Paulo, São José dos Campos, Brasil, 2026. [Online]. Disponível em: https://github.com/g-reale/TCC. Acesso em: julho de 2026.

---

## Referências

1. Cooley, J. W., & Tukey, J. W. (1965). *An algorithm for the machine calculation of complex Fourier series*. Mathematics of Computation, 19(90), 297–301.
2. Goertzel, G. (1958). *An Algorithm for the Evaluation of Finite Trigonometric Series*. The American Mathematical Monthly, 65(1), 34–35.
3. Oppenheim, A. V., Willsky, A. S., & Nawab, S. H. (1997). *Signals and Systems* (2nd ed.). Prentice Hall.
4. Bucheri, F. S., Reale, G., & Oliveira, A. M. (2024). *Development of an Interactive Graphical Interface for Real-Time Visualization of the Goertzel Algorithm*. Iniciação Científica, UNIFESP.