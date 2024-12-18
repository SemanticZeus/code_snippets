import numpy as np
m = 100
for da in range(1,m):
    for db in range(1,m):
        for dc in range(1,m):
            a = 20*da/(3*db*dc)
            a=np.sqrt(a)
            b = 5/dc * 1/a
            c = 4/db * 1/a
            s1 = np.floor(a)*b*c
            s2 = np.floor(b)*a*c
            s3 = np.floor(c)*b*a
            if np.abs(s1-3)<.01 and np.abs(s2-4)<.01 and np.abs(s3-5)<.01:
                print(f"{s1:.2f}, {s2:.2f}, {s3:.2f}")
                print(f"{a}, {b}, {c}")
                print(f"{da}, {db}, {dc}")
                print("---"*5)
