from pydub import AudioSegment
import pyrubberband as pyrb
import soundfile as sf
import numpy as np

def change_voice(file_path, output_path):
    # Load the audio file
    audio = AudioSegment.from_file(file_path)

    # Convert audio to samples and sample rate
    samples = audio.get_array_of_samples()
    sample_rate = audio.frame_rate

    # Using numpy to accommodate pyrubberband processing
    samples_np = np.array(samples).astype(np.float32)

    # Pitch shifting (male to female generally needs a higher pitch)
    shifted_samples = pyrb.pitch_shift(samples_np, sample_rate, n_steps=4)

    # Save the modified audio
    sf.write(output_path, shifted_samples, sample_rate, format='WAV')

# Usage
change_voice("my_voice_recording.wav", "output_file.wav")

