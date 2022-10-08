import time

import win_precise_time as wpt

for _ in range(20):
    print(f"{time.time()=:.9f} - {wpt.time()=:.9f}")
