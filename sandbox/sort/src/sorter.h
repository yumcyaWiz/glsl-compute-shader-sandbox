#ifndef _RENDERER_H
#define _RENDERER_H
#include <random>
#include <vector>

#include "glad/gl.h"
#include "glm/glm.hpp"
//
#include "gcss/buffer.h"

using namespace gcss;

class Sorter {
 private:
  uint32_t N;
  Buffer numbers;

 public:
  Sorter() : N(10000) {
    std::random_device rnd_dev;
    std::mt19937 mt(rnd_dev());
    std::uniform_int_distribution<int> dist(0, N);

    std::vector<int> data(N);
    for (std::size_t i = 0; i < N; ++i) {
      data[i] = dist(mt);
    }

    numbers.setData(data, GL_STATIC);
  }
};

#endif