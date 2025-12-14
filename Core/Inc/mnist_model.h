#ifndef INC_MNIST_MODEL_H_
#define INC_MNIST_MODEL_H_

#include <stdint.h>
#include "mnist_weights.h"

#ifdef __cplusplus
extern "C" {
#endif

int Mnist_RunInference(const uint8_t* img);

#ifdef __cplusplus
}
#endif


#endif // INC_MNIST_MODEL_H_