clear; clc; clear all; clf;
pkg load signal;

len = 15;
lpf = fir1(len,0.5);
hpf = lpf .* cos(pi*(0:len));

[H_lpf, w] = freqz(lpf, 1, 4096);
[H_hpf, ~] = freqz(hpf, 1, 4096);

plot(w/(2*pi), 20*log10(abs(H_lpf)+1e-12), 'b'); hold on;
plot(w/(2*pi), 20*log10(abs(H_hpf)+1e-12), 'r');
hold off;
grid on;
legend('LPF','HPF');
xlabel('Normalized Frequency');
ylabel('dB');

export("lpf.txt",lpf);
export("hpf.txt",hpf);
print('-dpng', 'complementary.png', '-r150');