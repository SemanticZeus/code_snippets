import numpy as np
import matplotlib.pyplot as plt

# Load the data from the SSV file
data = np.loadtxt('square_vout.ssv')

# Separate the columns into time and signal
time = data[:, 0]
signal = data[:, 1]

# Calculate the sampling frequency
sampling_interval = time[1] - time[0]  # Assuming uniform sampling
sampling_frequency = 1 / sampling_interval

# Perform FFT
fft_values = np.fft.fft(signal)
fft_magnitude = np.abs(fft_values)  # Magnitude of the FFT
fft_frequency = np.fft.fftfreq(len(fft_values), d=sampling_interval)

# Only take the positive frequencies
positive_frequencies = fft_frequency[:len(fft_frequency) // 2]
positive_magnitude = fft_magnitude[:len(fft_magnitude) // 2]

# Plot the frequency spectrum
plt.figure(figsize=(10, 6))
plt.plot(positive_frequencies, positive_magnitude)
plt.xlabel('Frequency (Hz)')
plt.ylabel('Magnitude')
plt.title('FFT of the Signal')
plt.plot(time, signal)
plt.grid()
plt.show()

