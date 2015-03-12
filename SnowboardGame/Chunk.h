#ifndef Chunk_H
#define Chunk_H

#include <iostream>
#include <stdlib.h>
#include <vector>

class Chunk{
public:
	std::vector<unsigned int> idx;
	Chunk(std::vector<unsigned int> vec){
		idx = vec;
	}
};

#endif