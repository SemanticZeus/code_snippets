from scipy import linalg
import numpy as np

def problem_1_25(R1):
    # first experiment
    A = np.zeros((2,2))
    B = np.zeros(2)
    R3 = (-8-2*R1)/3
    V1 = 8+2*R1
    V2 = V1-2
    i2 = (8-V2)
    A[0]=[8-V2, -V2]
    B[0] = -1
    #second experiment
    V1 = 12+2.4*R1
    i3 = -V1/R3
    iRL = -2.4+i3
    V2 = V1-iRL*4
    A[1] = [12-V2, -V2]
    B[1] = -iRL
    (R2,R4) = linalg.solve(A,B)
    R2=1/R2
    R4=1/R4
    print(f"R1 = {R1:0.3f}, R2 = {R2:0.3f}, R3 = {R3:0.3f}, R4 = {R4:0.3f}")

    for RL,Vin in [(2,8),(4,12)]:
        A = [[1/R1+1/R3+1/RL, -1/RL],[-1/RL, 1/RL+1/R2+1/R4]]
        B = [Vin/R1, Vin/R2]
        x = linalg.solve(A,B)
        VL = x[0]-x[1]
        i1 = (Vin-x[0])/R1
        i3 = -x[0]/R3
        print(f"RL = {RL}, Vin = {Vin} i3 = {i3:0.2f}")
        print(f"i1 = {i1:0.2f}, VL = {VL:0.2f}")


for R1 in range(-10,10):
    try:
        problem_1_25(R1)
        print("-"*24)
        print("\n"*3)


    except:
        pass


