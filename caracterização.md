---
marp: true
theme: beam
---


<!-- _class: title -->

# O algorítimo BAS (Binary Area Search)
Gabriel Reale Marques de Oliveira
Unifesp SJC
21/11/24

---

# Problema

- **Objetivo:** extração da frequência de pico ($\Omega_p$) em uma banda de frequências ($[\alpha,\beta]$) para um sinal arbitrário ($x(t)$):
$$\alpha,\beta \in \Re \,\,|\,\, \alpha \leq \beta$$
$$X(\Omega) = \mathcal{F}\{x(t)\}$$
$$\Omega_p \in [\alpha,\beta] \,\, | \,\, \Omega_p = max_{\Omega \in [\alpha,\beta]}(|X(\Omega)|)$$


---

# Aplicações

A **frequência de pico** é um indicador clássico no mundo de análise de sinais, e está envolvido em vários problemas da disciplina como:
- Processamento de fala, especialmente na detecção do sexo por meio da voz;
- Demodulação de sinais de rádio;
- Calculo de outros indicadores do sinal;

Tipicamente está envolvida em problemas de **processamento em janela, em tempo real e em sistemas embarcados**, nesses ambientes a **performance é crítica**.

---

# Estado do arte

- **Estado da arte:** Solução em complexidade assintótica $O(n log(n))$ 
- **Algoritimo BAS**: Solução em complexidade assintótica $O(log(n)^2)$

$$O(log(n)^2) < O(n log(n))$$

O ganho de performance possibilita que dispositivos limitados em *hardware* (embarcados) solucionem o problema, alternativamente, pode-se o aumentar a precisão da conta mantendo o mesmo tempo de execução.

---

# Algoritimo BAS

https://www.desmos.com/calculator/aablroknst
https://github.com/g-reale/TCC/blob/main/prova.ipynb

---

# Fontes

[Trends in audio signal feature extraction methods](https://calebrascon.info/PDA/Topic4/addresources/features.pdf)
Alan Oppenheim, Ramamurthy Mani - Signals & systems, 2nd edition-Pearson Education Limited (1996)