import numpy as np

from scipy.io import wavfile
samplerate, data = wavfile.read('FisiereWAV/15.wav')

wavFileInfo = open("wafeInfo.txt", "w")
wavFileInfo.write(str(samplerate)+'\n')
wavFileInfo.write(str(data.size)+'\n')
wavFileInfo.close()

print(samplerate)
print(data.size)
print(data)

np.savetxt("waveData.txt", data, fmt="%2.0f")

from scipy.signal import hilbert

analytic_signal = hilbert(data)
amplitude_envelope = np.abs(analytic_signal)  # anvelopa semnalului

np.savetxt("anvelopa.txt", amplitude_envelope, fmt="%2.0f")