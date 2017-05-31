/**
 * MIT License
 * Copyright (c) 2017 Hossein Mobasher
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/**
 *  This implementation is based on the WAVE PCM soundfile format.
 *  http://soundfile.sapp.org/doc/WaveFormat/
 *  Created by Hossein Mobasher <hoseinmobasher@gmail.com>
 */

#include <iostream>
#include <vector>
#include <fstream>

void raw_to_wav(std::string file_path, std::string file_name, int sample_rate, int bits_per_sample, int num_channels, int byte_rate);

int main() {
    const int SAMPLE_RATE = 48000;
    const int NUM_CHANNELS = 2;
    const int BITS_PER_SAMPLE = 16;
    const int BYTE_RATE = SAMPLE_RATE * NUM_CHANNELS * BITS_PER_SAMPLE / 8;

    std::string file_path = "/Users/hossein/Desktop/";
    std::string file_name = "record";

    raw_to_wav(file_path, file_name, SAMPLE_RATE, BITS_PER_SAMPLE, NUM_CHANNELS, BYTE_RATE);
}

void writeInt(std::vector<char> &output, const int value) {
    output.push_back((char) (value >> 0));
    output.push_back((char) (value >> 8));
    output.push_back((char) (value >> 16));
    output.push_back((char) (value >> 24));
}

void writeShort(std::vector<char> &output, const short value) {
    output.push_back((char) (value >> 0));
    output.push_back((char) (value >> 8));
}

void writeString(std::vector<char> &output, const std::string value) {
    for (int i = 0; i < value.length(); i++) {
        output.push_back(value[i]);
    }
}

void raw_to_wav(std::string file_path,
                std::string file_name,
                int srate = 48000, // sample rate
                int bps = 16, // bits per sample
                int nc = 2, // num channels (1 = mono, 2 = stereo)
                int brate = 192000) // byte rate
{
    std::vector<char> vector;

    std::ifstream file;
    file.exceptions(
            std::ifstream::badbit |
            std::ifstream::failbit |
            std::ifstream::eofbit);

    file.open(file_path + "/" + file_name + ".raw", std::ifstream::in | std::ifstream::binary);
    file.seekg(0, std::ios::end);
    std::streampos length(file.tellg());

    if (length) {
        file.seekg(0, std::ios::beg);
        vector.resize(static_cast<std::size_t>(length));
        file.read(&vector.front(), static_cast<std::size_t>(length));
    }

    std::vector<char> output;

    int n = 1;
    // little endian if true
    if(1 == *(char *) & n) {
        writeString(output, "RIFF"); // chunk id
    } else {
        writeString(output, "RIFX"); // chunk id
    }

    writeInt(output, (const int) (36 + vector.size())); // chunk size
    writeString(output, "WAVE"); // format
    writeString(output, "fmt "); // subchunk 1 id
    writeInt(output, 16); // subchunk 1 size
    writeShort(output, 1); // audio format (1 = PCM)
    writeShort(output, (const short) nc); // number of channels
    writeInt(output, srate); // sample rate
    writeInt(output, brate); // byte rate
    writeShort(output, (short) (nc * bps / 8)); // block align
    writeShort(output, (short) bps); // bits per sample
    writeString(output, "data"); // subchunk 2 id
    writeInt(output, (const int) vector.size()); // subchunk 2 size

    output.insert(output.end(), vector.begin(), vector.end());

    std::ofstream wav_file;
    wav_file.open(file_path + "/" + file_name + ".wav", std::ios::binary);
    wav_file.write(&output[0], output.size());
    wav_file.close();
}