clear; clc; clear all; clf;
pkg load signal

# Hamming windown
len = 15;
coefficients = fir1(len,0.5);

# Shift window
coefficients = coefficients .* exp(j*2*pi*(0:length(coefficients)-1)/4);

# Show filter
H = fftshift(fft(coefficients, 4096));
f = linspace(-0.5, 0.5, 4096);
plot(f, 20*log10(abs(H) + 1e-12));
grid on;
xlabel('Normalized Frequency');
ylabel('dB');

# Export coefficients
export("analytic.txt",coefficients);
print('-dpng', 'analytic.png', '-r150');