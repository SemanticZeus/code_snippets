import librosa
import numpy as np
import soundfile as sf
import argparse

def remove_background_music(input_file, output_file):
    # Load the audio file
    y, sr = librosa.load(input_file, sr=None)

    # Use Harmonic/Percussive source separation
    y_harmonic, y_percussive = librosa.effects.hpss(y)

    # Since vocals are more likely percussive, we keep that part and reduce the harmonic (background music)
    vocals_only = y_percussive

    # Save the processed audio (vocals only) to an output file
    sf.write(output_file, vocals_only, sr)
    print(f'Processed audio saved to {output_file}')

def main():
    # Set up argument parsing
    parser = argparse.ArgumentParser(description='Remove background music from audio file.')
    parser.add_argument('input_file', type=str, help='Path to the input audio file')
    parser.add_argument('output_file', type=str, help='Path to save the output (vocals only) file')

    # Parse command-line arguments
    args = parser.parse_args()

    # Call the function to process the audio
    remove_background_music(args.input_file, args.output_file)

if __name__ == '__main__':
    main()

