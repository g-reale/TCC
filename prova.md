**Lemma 1:** A integral em um cuboide na diagonal de $F: \Re^{2N} \rightarrow \Complex \,|\, \prod_{i=0}^{N-1} X(\Omega_i)\bar{X}(\Omega_{i +N})$ é o produto das potências de diferenças de transformadas de Fourier

$$X(\Omega) = \int_{-\infty}^{\infty} x(t)e^{-j\Omega t}dt$$

integrando em uma banda:

$$ \int_{\alpha}^{\beta} X(\Omega) d\Omega= \int_{\alpha}^{\beta}\int_{-\infty}^{\infty} x(t)e^{-j\Omega t}dt d\Omega$$
$$ \int_{\alpha}^{\beta} X(\Omega) d\Omega= \int_{-\infty}^{\infty} \int_{\alpha}^{\beta} x(t)e^{-j\Omega t}d\Omega dt$$
$$ \int_{\alpha}^{\beta} X(\Omega) d\Omega= \int_{-\infty}^{\infty} \frac{x(t)}{-jt} e^{-j\beta t} dt - \int_{-\infty}^{\infty} \frac{x(t)}{-jt} e^{-j\alpha t} dt $$

seja $y(t) = jx(t)/t$ e $\mathcal{F}\{y\} = Y(\Omega)$ então:

$$ \int_{\alpha}^{\beta} X(\Omega) d\Omega = Y(\beta) - Y(\alpha) \space$$

analogamente, seja $y^*(t) = j\bar{x}(-t)/t$ e $\mathcal{F}\{y^*\} = Y^*(\Omega)$ então:

$$ \int_{\alpha}^{\beta} \bar{X}(\sigma) d\sigma = Y^*(\beta) - Y^*(\alpha) \space$$

multiplicando $Y$ e $Y^*$ e elevando a uma potência:

$$ \int_{\alpha}^{\beta} X(\Omega) d\Omega \int_{\alpha}^{\beta} \bar{X}(\sigma) d\sigma = (Y(\beta) - Y(\alpha))(Y^*(\beta) - Y^*(\alpha))$$
$$ (\int_{\alpha}^{\beta} X(\Omega) d\Omega)^N (\int_{\alpha}^{\beta} \bar{X}(\sigma) d\sigma)^N = (Y(\beta) - Y(\alpha))^N(Y^*(\beta) - Y^*(\alpha))^N$$

expandindo a potência de integrais:

$$ (\int_{\alpha}^{\beta} X(\Omega_0) d\Omega_0 \cdots \int_{\alpha}^{\beta} X(\Omega_{N-1}) d\Omega_{N-1})  (\int_{\alpha}^{\beta} \bar{X}(\sigma_0) d\sigma_0 \cdots \int_{\alpha}^{\beta} \bar{X}(\sigma_{N-1}) d\sigma_{N-1}) = (Y(\beta) - Y(\alpha))^N(Y^*(\beta) - Y^*(\alpha))^N$$
$$ (\int_{\alpha}^{\beta} X(\Omega_0) d\Omega_0 \cdots \int_{\alpha}^{\beta} X(\Omega_{N-1}) d\Omega_{N-1})  (\int_{\alpha}^{\beta} \bar{X}(\sigma_0) d\sigma_0 \cdots \int_{\alpha}^{\beta} \bar{X}(\sigma_{N-1}) d\sigma_{N-1}) = (Y(\beta) - Y(\alpha))^N(Y^*(\beta) - Y^*(\alpha))^N$$
$$(\underbrace{\int \cdots \int_{\alpha}^{\beta}}_{N} X(\Omega_0) \cdots X(\Omega_n)d\Omega_0\cdots d\Omega_{N-1})(\underbrace{\int \cdots \int_{\alpha}^{\beta}}_{N} \bar{X}(\sigma_0) \cdots \bar{X}(\sigma_n)d\sigma_0\cdots d\sigma_{N-1}) = (Y(\beta) - Y(\alpha))^N(Y^*(\beta) - Y^*(\alpha))^N$$
$$\underbrace{\int \cdots \int_{\alpha}^{\beta}}_{2N} X(\Omega_0)\bar{X}(\sigma_0) \cdots X(\Omega_{N-1})\bar{X}(\sigma_{N-1}) \,d\Omega_0d\sigma_0\cdots d\Omega_{N-1}d\sigma_{N-1} = (Y(\beta) - Y(\alpha))^N(Y^*(\beta) - Y^*(\alpha))^N$$

aplicando a notação de produtório:

$$\underbrace{\int \cdots \int_{\alpha}^{\beta}}_{2N} \prod_{i=0}^{N-1} X(\Omega_i) \times\prod_{j=0}^{N-1} \bar{X}(\sigma_j) \times \prod_{k=0}^{N-1} d\Omega_kd\sigma_k = (Y(\beta) - Y(\alpha))^N(Y^*(\beta) - Y^*(\alpha))^N$$

alterando os rótulos $\sigma$ para simplificar a notação:

$$\underbrace{\int \cdots \int_{\alpha}^{\beta}}_{2N} \prod_{i=0}^{N-1} X(\Omega_i) \times\prod_{j=0}^{N-1} \bar{X}(\Omega_{j +N}) \times \prod_{k=0}^{2N-1} d\Omega_k = (Y(\beta) - Y(\alpha))^N(Y^*(\beta) - Y^*(\alpha))^N$$
$$\underbrace{\int \cdots \int_{\alpha}^{\beta}}_{2N} \prod_{i=0}^{N-1} X(\Omega_i)\bar{X}(\Omega_{i +N})d\Omega_i d\Omega_{i+N} = (Y(\beta) - Y(\alpha))^N(Y^*(\beta) - Y^*(\alpha))^N$$
$$\underbrace{\int \cdots \int_{\alpha}^{\beta}}_{2N} F(\Omega_0,\cdots,\Omega_{2N-1}) d\Omega_0 \cdots d\Omega_{2N-1} = (Y(\beta) - Y(\alpha))^N(Y^*(\beta) - Y^*(\alpha))^N$$


**Lemma 2:** A integral de uma potência par do módulo de $X$ pode ser obtida pela integração de $F: \Re^{2N} \rightarrow \Complex \,|\, \prod_{i=0}^{N-1} X(\Omega_i)\bar{X}(\Omega_{i +N})$ em sua diagonal:

$$F(\Omega_0,\cdots,\Omega_{2N-1}) = \prod_{i=0}^{N-1} X(\Omega_i)\bar{X}(\Omega_{i +N})$$

na diagonal $\Omega_0 = \cdots = \Omega_{2N-1}$

$$F(\Omega,\cdots,\Omega) = \prod_{i=0}^{N-1} X(\Omega)\bar{X}(\Omega)$$
$$F(\Omega,\cdots,\Omega) = \prod_{i=0}^{N-1} |X(\Omega)|^2$$
$$F(\Omega,\cdots,\Omega) = |X(\Omega)|^{2N}$$

integrando:

$$\int_{\alpha}^{\beta} F(\Omega,\cdots,\Omega) = \int_{\alpha}^{\beta} |X(\Omega)|^{2N} d\Omega$$

**Lemma 3:** A integral de $F: \Re^{2N} \rightarrow \Complex \,|\, \prod_{i=0}^{N-1} X(\Omega_i)\bar{X}(\Omega_{i +N})$ em sua diagonal pode ser aproximada pela soma de integrais de $M$ cuboides $2N$-dimensionais na diagonal

Tomando $\Delta = \frac{\beta-\alpha}{M}$ e integrando $F$ em $M$ cuboides tem se:

$$\sum_{i = 0}^{M-1} \underbrace{\int \cdots \int_{i\Delta + \alpha}^{\Delta(i+1)+\alpha}}_{\times 2N} F(\Omega_0, \dots, \Omega_{2N-1}) \, d\Omega_0 \cdots d\Omega_{2N-1}$$

Tomando $S \subset \Re \,|\, S = [i\Delta + \alpha,(i+1)\Delta + \alpha]$ então:

$$ \Omega_0, \cdots,\Omega_{2N-1} \in S$$
$$\Omega_j,\Omega_k \,|\, (\Omega_j,\Omega_k) \in S \times S$$
$$ min(S) - max(S) \leq \Omega_j - \Omega_k \leq max(S) - min(S) $$
$$ (i\Delta + \alpha) - ((i+1)\Delta + \alpha) \leq \Omega_j - \Omega_k \leq ((i+1)\Delta + \alpha) - (i\Delta + \alpha) $$
$$ -\Delta \leq \Omega - \sigma \leq \Delta \therefore |\Omega_j - \Omega_k| \leq \Delta$$
$$\Delta = \frac{(\beta - \alpha)}{M} \therefore |\Omega_j - \Omega_k| \leq \frac{(\beta - \alpha)}{M}$$
$$ \lim_{M\rightarrow\infty} |\Omega_j - \Omega_k| \leq \frac{(\beta - \alpha)}{M}$$
$$ \lim_{M\rightarrow\infty} |\Omega_j - \Omega_k| \leq 0 \therefore \lim_{M\rightarrow\infty}\Omega_j = \Omega_k \therefore \lim_{M\rightarrow\infty} \Omega_0 = \Omega_1 = \cdots =\Omega_{2N-1}$$

Ou seja, dado $M$ grande o suficiente, a distancia entre quais quer 2 argumentos de $F$ é tão pequena quanto se desejar. Substituindo o limite na relação inicial:

$$\lim_{M \rightarrow \infty} \sum_{i = 0}^{M-1} \int \cdots \int_{i\Delta + \alpha}^{\Delta(i+1)+\alpha} F(\Omega_0, \dots, \Omega_{2N-1}) \, d\Omega_0 \cdots d\Omega_{2N-1} = \sum_{i = 0}^{M-1} \int \cdots \int_{i\Delta + \alpha}^{\Delta(i+1)+\alpha} F(\Omega, \dots, \Omega) \, d\Omega \cdots d\Omega$$
$$\sum_{i = 0}^{M-1} \int \cdots \int_{i \frac{\beta-\alpha}{M} + \alpha}^{(i+1) \frac{\beta-\alpha}{M} + \alpha} F(\Omega, \dots, \Omega) \, d\Omega \cdots d\Omega$$
$$\int \cdots \int_{0\frac{\beta-\alpha}{M} + \alpha}^{(M-1+1) \frac{\beta-\alpha}{M} + \alpha} F(\Omega, \dots, \Omega) \, d\Omega \cdots d\Omega$$
$$\int \cdots \int_{\alpha}^{\beta-\alpha + \alpha} F(\Omega, \dots, \Omega) \, d\Omega \cdots d\Omega$$
$$\int \cdots \int_{\alpha}^{\beta} F(\Omega, \dots, \Omega) \, d\Omega \cdots d\Omega$$
$$(\beta-\alpha)^{2N-1}\int_{\alpha}^{\beta} F(\Omega, \dots, \Omega) \, d\Omega \cdots d\Omega$$
$$\therefore \int_{\alpha}^{\beta} F(\Omega, \dots, \Omega) \, d\Omega \cdots d\Omega = \lim_{N\rightarrow\infty} \frac{1}{(\beta-\alpha)^{2N-1}}\sum_{i = 0}^{M-1} \int \cdots \int_{i\Delta + \alpha}^{\Delta(i+1)+\alpha} F(\Omega_0, \dots, \Omega_{2N-1}) \, d\Omega_0 \cdots d\Omega_{2N-1}$$

**Teorema:** A integral de uma potência par do módulo de $X$ pode ser aproximada pela soma de $M$ produtos de potências de diferença de transformadas de Fourier:

pelo **Lemma 3:**

$$\int_{\alpha}^{\beta} F(\Omega, \dots, \Omega) \, d\Omega = \lim_{N\rightarrow\infty} \frac{1}{(\beta-\alpha)^{2N-1}}\sum_{i = 0}^{M-1} \int \cdots \int_{i\Delta + \alpha}^{\Delta(i+1)+\alpha} F(\Omega_0, \dots, \Omega_{2N-1}) \, d\Omega_0 \cdots d\Omega_{2N-1}$$

pelo **Lemma 2:**

$$\int_{\alpha}^{\beta} |X(\Omega)|^{2N} \, d\Omega = \lim_{N\rightarrow\infty} \frac{1}{(\beta-\alpha)^{2N-1}}\sum_{i = 0}^{M-1} \int \cdots \int_{i\Delta + \alpha}^{\Delta(i+1)+\alpha} F(\Omega_0, \dots, \Omega_{2N-1}) \, d\Omega_0 \cdots d\Omega_{2N-1}$$

pelo **Lemma 1**

- seja $y(t) = jx(t)/t$ e $\mathcal{F}\{y\} = Y(\Omega)$
- seja $y^*(t) = j\bar{x}(-t)/t$ e $\mathcal{F}\{y^*\} = Y^*(\Omega)$

$$\int_{\alpha}^{\beta} |X(\Omega)|^{2N} \, d\Omega = \lim_{N\rightarrow\infty} \frac{1}{(\beta-\alpha)^{2N-1}}\sum_{i = 0}^{M-1} (Y(\Delta(i+1)+\alpha) - Y(i\Delta + \alpha))^N(Y^*(\Delta(i+1)+\alpha) - Y^*(i\Delta + \alpha))^N$$
$$\square$$