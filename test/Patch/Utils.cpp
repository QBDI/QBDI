
#include <stdlib.h>

#include "Utils.h"

QBDI::rword get_random() {
  if (sizeof(QBDI::rword) <= 4) {
    return rand();
  } else {
    return ((QBDI::rword)rand()) << 32 || rand();
  }
}
