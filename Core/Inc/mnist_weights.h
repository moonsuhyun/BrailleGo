#pragma once

#include <stdint.h>

#define MNIST_IMG_W      28
#define MNIST_IMG_H      28
#define MNIST_IMG_SIZE   (MNIST_IMG_W * MNIST_IMG_H)

#define MNIST_INPUT_SIZE  MNIST_IMG_SIZE
#define MNIST_HIDDEN_SIZE 32
#define MNIST_NUM_CLASSES 10

#define MNIST_SHIFT1 6
#define MNIST_SHIFT2 8


#ifdef __cplusplus
extern "C" {
#endif

// 1층: hidden[j] = ReLU( (Σ_i W1[j][i] * img[i]) + B1[j] ) >> SHIFT1
extern const int8_t  g_mnist_w1[MNIST_HIDDEN_SIZE][MNIST_INPUT_SIZE];
extern const int32_t g_mnist_b1[MNIST_HIDDEN_SIZE];

// 2층: logits[k] = (Σ_j W2[k][j] * hidden[j]) + B2[k]) >> SHIFT2
extern const int8_t  g_mnist_w2[MNIST_NUM_CLASSES][MNIST_HIDDEN_SIZE];
extern const int32_t g_mnist_b2[MNIST_NUM_CLASSES];

#ifdef __cplusplus
}
#endif



