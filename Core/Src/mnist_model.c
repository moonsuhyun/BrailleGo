//
// Created by ��������� on 25. 12. 11..
//

#include "mnist_model.h"

static inline int32_t clamp_i32(int32_t x, int32_t lo, int32_t hi)
{
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

int Mnist_RunInference(const uint8_t* img)
{
    int32_t hidden[MNIST_HIDDEN_SIZE];
    int32_t logits[MNIST_NUM_CLASSES];

    // -----------------------------
    // 1층: INPUT(784) -> HIDDEN(32)
    // -----------------------------
    for (int j = 0; j < MNIST_HIDDEN_SIZE; ++j) {
        int32_t acc = g_mnist_b1[j];

        // Σ_i W1[j][i] * img[i]
        for (int i = 0; i < MNIST_INPUT_SIZE; ++i) {
            int32_t w = (int32_t)g_mnist_w1[j][i];  // [-128,127]
            int32_t x = (int32_t)img[i];            // [0,255]
            acc += w * x;
        }

        // 스케일 다운
        acc >>= MNIST_SHIFT1;

        // ReLU
        if (acc < 0) acc = 0;

        acc = clamp_i32(acc, 0, 32767);

        hidden[j] = acc;
    }

    // -----------------------------
    // 2층: HIDDEN(32) -> CLASS(10)
    // -----------------------------
    for (int k = 0; k < MNIST_NUM_CLASSES; ++k) {
        int32_t acc = g_mnist_b2[k];

        for (int j = 0; j < MNIST_HIDDEN_SIZE; ++j) {
            int32_t w = (int32_t)g_mnist_w2[k][j];  // [-128,127]
            int32_t h = hidden[j];                  // >= 0
            acc += w * h;
        }

        acc >>= MNIST_SHIFT2;
        logits[k] = acc;
    }

    // -----------------------------
    // argmax(logits)
    // -----------------------------
    int best_class = 0;
    int32_t best_logit = logits[0];

    for (int k = 1; k < MNIST_NUM_CLASSES; ++k) {
        if (logits[k] > best_logit) {
            best_logit = logits[k];
            best_class = k;
        }
    }

    return best_class;
}
