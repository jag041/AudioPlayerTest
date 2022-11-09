#include "WAVParser.h"
#include "GlobalConsts.h"
#include <iostream>
#include "ErrorHandler.h"

WAVParser::WAVParser(WavDirInputFormat format)
{
	sample_list.resize(format.files.size());

	for (size_t i = 0; i < format.files.size(); i++) {

		std::string instrument = format.instruments[i];

		for (size_t j = 0; j < format.files[i].size(); j++) {

			sample_list[i].push_back(read_data(std::string(format.root_file_path + instrument + format.files[i][j])));
		}
	}

	sample_list[0].push_back(get_blank_sample(20000)); // currently set to one beat at 144 bpm ie 20000 samples


}

WAVParser::~WAVParser() {

	for (size_t i = 0; i < sample_list.size(); i++) {

		for (size_t j = 0; j < sample_list[i].size(); j++) {
			std::cout << "freeing sample " << i << "," << j << " in WAVParser destructor" << std::endl;
			free(sample_list[i][j].sample_ptr);
		}

	}
}

std::vector<std::vector<SampleData>> WAVParser::get_samples_with_beats() {

	// a bit hacky right now...
	for (size_t i = 0; i < sample_list[0].size() - 1; i++) {

		if (i < 4) {
			sample_list[1][i].data_size = 20000;
			sample_list[2][i].data_size = 20000;
		}
		else if (i < 8) {
			sample_list[1][i].data_size = 25000;
			sample_list[2][i].data_size = 25000;
		}
		else if (i < 12) {
			sample_list[1][i].data_size = 30000;
			sample_list[2][i].data_size = 30000;
		}

		sample_list[0][i].data_size = sample_sizes[i/3];
		
	}

	return sample_list;


}

size_t WAVParser::get_total_size() const {

	return total_data_size;

}
SampleData WAVParser::get_blank_sample(size_t amount)
{
	SampleData ret;
	size_t num_bytes = amount * sizeof(uint16_t);
	ret.sample_ptr = (uint16_t*)malloc(num_bytes);
	memset(ret.sample_ptr, 0, num_bytes);
	ret.data_size = amount;
	total_data_size += num_bytes;
	return ret;
};


SampleData WAVParser::read_data(std::string filepath) {
	FILE* file;
	char magic[4];
	int32_t filesize;
	int32_t format_length;		// 16
	int16_t format_type;		// 1 = PCM
	int16_t num_channels;		// 1
	int32_t sample_rate;		// 44100
	int32_t bytes_per_second;	// sample_rate * num_channels * bits_per_sample / 8
	int16_t block_align;		// num_channels * bits_per_sample / 8
	int16_t bits_per_sample;	// 16
	int32_t data_size;

	fopen_s(&file, filepath.c_str(), "rb");
	if (file == NULL) {
		throw ErrorHandler(std::string("Failed to open WAV file: " + filepath));
	}

	fread(magic, 1, 4, file);
	std::cout << magic[0] << magic[1] << magic[2] << magic[3] << std::endl;
	if (magic[0] != 'R' || magic[1] != 'I' || magic[2] != 'F' || magic[3] != 'F') {
		throw ErrorHandler(std::string("first four bytes should be RIFF: " + filepath));
	}

	fread(&filesize, 4, 1, file);

	fread(magic, 1, 4, file);
	std::cout << magic[0] << magic[1] << magic[2] << magic[3] << std::endl;
	if (magic[0] != 'W' || magic[1] != 'A' || magic[2] != 'V' || magic[3] != 'E') {
		throw ErrorHandler(std::string("second four bytes should be WAVE: " + filepath));
	}

	fread(magic, 1, 4, file);
	std::cout << magic[0] << magic[1] << magic[2] << magic[3] << std::endl;
	if (magic[0] != 'f' || magic[1] != 'm' || magic[2] != 't' || magic[3] != ' ') {
		throw ErrorHandler(std::string("third four bytes should be fmt : " + filepath));
	}

	fread(&format_length, 4, 1, file); 

	fread(&format_type, 2, 1, file);
	if (format_type != 1) {
		throw ErrorHandler(std::string("format type should be 1: " + filepath));
	}

	fread(&num_channels, 2, 1, file);
	if (num_channels != 1) {
		throw ErrorHandler(std::string("number of channels should be 1: " + filepath));
	}

	fread(&sample_rate, 4, 1, file);
	if (sample_rate != SAMPLE_RATE) {
		throw ErrorHandler(std::string("Sample rate is not 48000: " + filepath));
	}

	fread(&bytes_per_second, 4, 1, file);
	fread(&block_align, 2, 1, file);
	fread(&bits_per_sample, 2, 1, file);
	if (bits_per_sample != BIT_DEPTH) {
		throw ErrorHandler(std::string("bits per sample is not 16: " + filepath));
	}

	//fread(magic, 1, 4, file);
	//std::cout << magic[0] << magic[1] << magic[2] << magic[3] << std::endl;
	//if (magic[0] != 'j' || magic[1] != 'u' || magic[2] != 'n' || magic[3] != 'k') {
	//	throw ErrorHandler(std::string("\'junk\' string missing from: " + filepath));
	//}

	fread(magic, 1, 4, file);
	std::cout << magic[0] << magic[1] << magic[2] << magic[3] << std::endl;
	if (magic[0] != 'd' || magic[1] != 'a' || magic[2] != 't' || magic[3] != 'a') {
		throw ErrorHandler(std::string("\'data\' string missing from: " + filepath));
	}

	fread(&data_size, 4, 1, file);

	

	SampleData data;
	data.data_size = data_size;

	std::cout << "data size: " << data_size << " for " << filepath << std::endl;

	data.sample_ptr = (uint16_t*)malloc(data_size);

	if (data.sample_ptr == NULL) {
		throw ErrorHandler(std::string("failed to allocate WAV data upon parse: " + filepath));
	}

	if (fread(data.sample_ptr, 1, data_size, file) != data_size) {
		throw ErrorHandler(std::string("failed to read bytes in WAV file parse: " + filepath));
	}

	total_data_size += data_size;

	return data;


}