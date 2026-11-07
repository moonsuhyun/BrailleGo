//
// Created by 문수현 on 26. 11. 7..
//

#ifndef BRAILLEGO_MACRO_H
#define BRAILLEGO_MACRO_H

#define SCOPED_LOCK(mutex, code) \
    Mutex_Lock(mutex); \
    code \
    Mutex_Unlock(mutex);

#endif //BRAILLEGO_MACRO_H
