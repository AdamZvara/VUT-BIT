import matplotlib.pyplot as plt
import numpy as np
from scipy.io import wavfile
from scipy.signal import spectrogram, butter, buttord, freqz, lfilter, tf2zpk

filename = "audio/xzvara01.wav"

rate, data = wavfile.read(filename)
# normalize data - values in interval <-1,1>
data = data / 2**15

# 4.1
max = data.max()
min = data.min()
data_size = len(data)
length = data_size/rate

print("Dlzka signalu (vzorky):", data_size)
print("Dlzka signalu (cas):", length)
print("Maximalna hodnota:", max)
print("Minimalna hodnota:", min)

# create x-axis in reasonable units (seconds)
x_axis = np.linspace(0, length, data_size)
plt.plot(x_axis, data)
plt.xlabel("Čas [s]")
plt.ylabel("Hodnoty signalu")
plt.grid(True)
plt.show()

# 4.2
mean = np.mean(data)
max_abs = np.max(np.abs(data))
for i in range(data_size):
    data[i] = data[i] - mean
    data[i] = data[i] / max_abs

# create sliding windows with 1024 with and overlap on 512th sample
x_window = np.lib.stride_tricks.sliding_window_view(x_axis, 1024)[::512]
d_window = np.lib.stride_tricks.sliding_window_view(data, 1024)[::512]

"""
# plotting each window individiually
window_counter = 0
for i in range(len(d_window)):
    plt.clf()
    plt.plot(x_window[i], d_window[i])
    plt.xlabel("Čas [s]")
    plt.ylabel("Hodnota signalu")
    plt.title(str(window_counter))
    plt.grid(True)
    plt.show()
    window_counter += 1
"""

# result window
window = 31
plt.clf()
plt.plot(x_window[window], d_window[window])
plt.xlabel("Čas [s]")
plt.ylabel("Hodnota signalu")
plt.grid(True)
plt.show()

# 4.3
def dft(data : list, N = 1024):
    n = np.arange(N)
    # make k column vector so that k*n produces "DFT matrix" (not yet with complex exponencial!)
    k = n.reshape((N,1))
    # multiply data with DFT matrix
    return np.dot(data, np.exp(-(2j*np.pi*k*n)/N))

dft_result = dft(d_window[window])
x_axis = np.linspace(0, rate//2, 512)

plt.clf()
plt.grid(True)
plt.plot(x_axis, np.absolute(dft_result[:len(dft_result)//2]))
plt.xlabel("Frekvencia [Hz]")
plt.ylabel("|X[k]|")
plt.show()


"""
# compare values with dft_real_results

dft_real_result = np.fft.fft(d_window[window], 1024)
if (np.allclose(dft_result, dft_real_result)):
    print("Okay!")
else:
    print("Something went wrong!")

plt.clf()
plt.grid(True)
plt.plot(x_axis, np.absolute(dft_real_result[:len(dft_real_result)//2]))
plt.xlabel("Frekvencia [Hz]")
plt.ylabel("|X[k]|")
plt.show()
"""

# 4.4
# spectogram over data with Fs, window size 1024, overlapping 512
f, t, Sxx = spectrogram(data, rate, nperseg=1024, noverlap=512)
Sxx = 10*np.log10(np.power(np.absolute(Sxx), 2))
plt.figure(figsize=(6,6))
plt.pcolormesh(t, f, Sxx)
plt.ylabel('Frekvencia [Hz]')
plt.xlabel('Čas [s]')
cbar = plt.colorbar()
cbar.set_label('Spektralna hustota vykonu [dB]', rotation=270, labelpad=15)
plt.show()

# 4.5
def get_freq(data : list, treshold : int):
    freq = []
    for index,val in enumerate(data):
        if (val > treshold):
            freq.append((rate//2)*index//len(data))
    return freq

# get frequencies from single spectrum
# need spectrum at 2.25s, if whole spectrum (3.42s) is 106 indexes long, 2.25 is roughly at 70th index
# set treshold value to -90 (around frequency we are looking for)
frequencies = get_freq(np.transpose(Sxx)[70], -90)

# 4.6
# Compute waveform samples
t = np.arange(0, length, 1/rate)
cos_data = 0

for i in frequencies:
    f = 0.1*np.cos(2*np.pi *i* t)
    # generate separate frequency files
    # wavio.write("freq"+str(i)+".wav", f, rate, sampwidth=3)
    cos_data += f

# Write the samples to a file
wavfile.write("audio/4cos.wav", rate, cos_data)

f, t, Sxx = spectrogram(cos_data, rate, nperseg=1024, noverlap=512)
Sxx = 10*np.log10(np.power(np.absolute(Sxx), 2))
plt.figure(figsize=(6,6))
plt.pcolormesh(t, f, Sxx)
plt.ylabel('Frekvencia [Hz]')
plt.xlabel('Čas [s]')
cbar = plt.colorbar()
cbar.set_label('Spektralna hustota vykonu [dB]', rotation=270, labelpad=15)
plt.show()

# 4.7
def plot_impulse(b, a):
    N_imp = 32
    imp = [1, *np.zeros(N_imp-1)] # jednotkovy impuls
    h = lfilter(b, a, imp)

    plt.figure(figsize=(8,3))
    plt.stem(np.arange(N_imp), h, basefmt=' ')
    plt.gca().set_xlabel('$n$')
    plt.gca().set_title('Impulsní odezva $h[n]$')

    plt.grid(alpha=0.5, linestyle='--')
    plt.tight_layout()
    plt.show()

def plot_zeros_and_poles(b, a):
    z, p, k = tf2zpk(b, a)

    plt.figure(figsize=(4,3.5))

    # jednotkova kruznice
    ang = np.linspace(0, 2*np.pi,100)
    plt.plot(np.cos(ang), np.sin(ang))

     # nuly, poly
    plt.scatter(np.real(z), np.imag(z), marker='o', facecolors='none', edgecolors='r', label='nuly')
    plt.scatter(np.real(p), np.imag(p), marker='x', color='g', label='póly')

    plt.gca().set_xlabel('Realná složka $\mathbb{R}\{$z$\}$')
    plt.gca().set_ylabel('Imaginarní složka $\mathbb{I}\{$z$\}$')

    plt.grid(alpha=0.5, linestyle='--')
    plt.legend(loc='upper left')
    plt.tight_layout()
    plt.show()

def plot_freq_char(b, a):
    w, H = freqz(b, a)

    _, ax = plt.subplots(1, 2, figsize=(8,3))
    ax[0].plot(w / 2 / np.pi * rate, np.abs(H))
    ax[0].set_xlabel('Frekvence [Hz]')
    ax[0].set_title('Modul frekvenční charakteristiky $|H(e^{j\omega})|$')

    ax[1].plot(w / 2 / np.pi * rate, np.angle(H))
    ax[1].set_xlabel('Frekvence [Hz]')
    ax[1].set_title('Argument frekvenční charakteristiky $\mathrm{arg}\ H(e^{j\omega})$')

    for ax1 in ax:
        ax1.grid(alpha=0.5, linestyle='--')

    plt.tight_layout()
    plt.show()

stop_width = 15
pass_width = 50
filter = []
filtered_data = data

for freq in frequencies:
    stop_freq = [freq-stop_width, freq+stop_width]
    pass_freq = [stop_freq[0]-pass_width, stop_freq[1]+pass_width]

    # another option, less quality
    #b,a = iirnotch(frequencies[i], 1, Fs)

    ord, wn = buttord(pass_freq, stop_freq, 3, 40, fs=rate)
    b, a = butter(ord, wn, 'bandstop', fs=rate)

    print("Koeficienty na frekvencii "+str(i+1)+":")
    print("b : ", b)
    print("a : ", a)

    plot_impulse(b, a)

    # 4.8
    plot_zeros_and_poles(b, a)

    # 4.9 frekvencni charakteristika
    plot_freq_char(b, a)

    filtered_data = lfilter(b, a, filtered_data)

# 4.10
wavfile.write("audio/clean_bandstop.wav", rate, filtered_data)

f, t, Sxx = spectrogram(filtered_data, rate, nperseg=1024, noverlap=512)
Sxx = 10*np.log10(np.power(np.absolute(Sxx), 2))
plt.figure(figsize=(6,6))
plt.pcolormesh(t, f, Sxx)
plt.ylabel('Frekvencia [Hz]')
plt.xlabel('Čas [s]')
cbar = plt.colorbar()
cbar.set_label('Spektralna hustota vykonu [dB]', rotation=270, labelpad=15)
plt.show()