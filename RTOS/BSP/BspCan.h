/*
 * BspCan.h
 *
 *  bxCAN driver (register level) for the BSP layer.
 */

#ifndef BSP_BSPCAN_H_
#define BSP_BSPCAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "Kernel.h"

typedef enum
{
    BSP_CAN_MODE_NORMAL,
    BSP_CAN_MODE_LOOPBACK,      /* silent loopback: TX is looped back internally, nothing goes on the bus */
} BspCanMode_t;

int32_t  BSP_CAN_Init(BspCanMode_t mode);
int32_t  BSP_CAN_Transmit(const CanFrame_t* frame);
int32_t  BSP_CAN_GetFrame(CanFrame_t* out);
int32_t  BSP_CAN_GetFrameBlocking(CanFrame_t* out);
uint32_t BSP_CAN_GetRxDropCount(void);
uint32_t BSP_CAN_GetTxPendingCount(void);
uint32_t BSP_CAN_GetRxPendingCount(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_BSPCAN_H_ */
