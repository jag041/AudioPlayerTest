#pragma once

// std includes
#include <vector>
#include <string>

// my includes


struct WavDirInputFormat {

	std::string root_file_path;
	std::vector<std::string> instruments;
	std::vector<std::vector<std::string>> files;
	uint16_t num_files;

};

struct SampleData {

	SampleData() {
		sample_ptr = nullptr;
		data_size = 0;
	};

	SampleData(uint16_t* samps, size_t len) {
		sample_ptr = samps;
		data_size = len;
	}
	uint16_t* sample_ptr;
	size_t data_size;

};


class WAVParser
{
public:
	WAVParser(WavDirInputFormat format);
	~WAVParser();

	std::vector<std::vector<SampleData>> get_samples_with_beats();

	size_t get_total_size() const;



private:


	std::vector<std::vector<SampleData>> sample_list{};

	std::vector<uint32_t> sample_sizes = { 20000, 25000, 30000, 25000, 30000, 25000, 25000, 20000 };

	SampleData read_data(std::string filepath);

	uint64_t total_data_size{};

	

	SampleData get_blank_sample(size_t amount);
};

