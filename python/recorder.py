import sounddevice as sd
import soundfile as sf

def record_voice(filename, duration=10, samplerate=44100):
    """
    Records voice from the microphone and saves it to a file.
    
    Args:
    filename (str): The path to the output file.
    duration (int): Recording duration in seconds. Default is 10 seconds.
    samplerate (int): Sampling rate in Hz. Default is 44100 Hz.
    """
    print("Recording...")
    audio_data = sd.rec(int(duration * samplerate), samplerate=samplerate, channels=1, dtype='float32')
    sd.wait()  # Wait until recording is finished
    print("Recording finished. Saving to file...")
    sf.write(filename, audio_data, samplerate)
    print("File saved:", filename)

# Example usage
record_voice("my_voice_recording.wav", duration=5)  # Records for 5 seconds

