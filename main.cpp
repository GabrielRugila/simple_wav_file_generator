#include <iostream>
#include <cmath>
#include <fstream>

using namespace std;

const int sampleRate = 44100;
const int bitDepth = 16;

class SineOscillator {
    // Sine Wave: y(t) = A * sin(2 * pi * f * t)
    // A = Amplitude, f = Frequency, t or Phi = Time -> Sample Rate
    float frequency, amplitude, angle = 0.0f, offset = 0.0;

    public:
        SineOscillator(float freq, float amp) : frequency(freq), amplitude(amp) {
            offset = 2 * M_PI * frequency / sampleRate;
        }

        float process() {
            auto sample = amplitude * sin(2 * angle);
            angle += offset;
            return sample;
        }
};

void writeToFile(ofstream &file, int value, int size) {
    file.write(reinterpret_cast<const char*> (&value), size);
};

int main() {
    int duration = 4; // real duration will be *2 this value
    int channels = 1;
    ofstream audioFile;
    
    audioFile.open("waveform.wav", ios::binary);

    SineOscillator sineOscillator(440, 0.5);

    // Header Chunk
    audioFile << "RIFF";
    audioFile << "----";
    audioFile << "WAVE";

    // Format Chunk
    audioFile << "fmt ";
    writeToFile(audioFile, 16, 4); // Size
    writeToFile(audioFile, 1, 2); // Compression Code
    writeToFile(audioFile, channels, 2); // Channels
    writeToFile(audioFile, sampleRate, 4); // Sample Rate
    writeToFile(audioFile, sampleRate * bitDepth * channels / 8, 4); // Byte Rate
    writeToFile(audioFile, bitDepth / 8, 2); // Block Align
    writeToFile(audioFile, bitDepth, 2); // Bit Depth

    // Data Chunk
    audioFile << "data";
    audioFile << "----";

    int preAudioPos = audioFile.tellp();
    auto maxAmp = pow(2, bitDepth - 1) - 1;
    for(int i = 0; i < sampleRate * duration; i++) {
        auto sample = sineOscillator.process();
        int intSample = static_cast<int> (sample * maxAmp);
        audioFile.write(reinterpret_cast<char*> (&intSample), 2);
        writeToFile(audioFile, intSample, 2);
    }
    int postAudioPos = audioFile.tellp();

    audioFile.seekp(preAudioPos - 4);
    writeToFile(audioFile, postAudioPos - preAudioPos, 4); // Data Chunk Size

    audioFile.seekp(4, ios::beg);
    writeToFile(audioFile, postAudioPos - 8, 4); // Header Chunk Size

    audioFile.close();
    return 0;
}