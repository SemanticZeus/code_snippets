*Sample Op Amp circuit

.model npn npn (Is=1.8104e-15 Bf=100 Vaf=100V)
.model pnp pnp (Is=1.8104e-15 Bf=100 Vaf=100V)

.SUBCKT opamp vp vn output vcc vss gnd nulloffset1 nulloffset2

Rshort1 e5 nulloffset1 0
Rshort2 e6 nulloffset2 0

C1 c13 c4 30p

Q1 c1 vp e1 npn
Q2 c1 vn e2 npn
Q3 c3 c9 e1 pnp
Q4 c4 c9 e2 pnp
Q5 c3 b5 e5 npn
Q6 c4 b5 e6 npn
Q7 vcc c3 b5 npn
Q8 c1 c1 vcc pnp
Q9 c9 c1 vcc pnp

R1 e5 vss 1k
R2 b5 vss 50k
R3 e6 vss 1k

Q10 c9  c11 e10 npn
Q11 c11 c11 vss npn
Q12 c12 c12 vcc pnp
Q13 c13 c12 vcc pnp

R4 e10 vss 5k
R5 c12 c11 39k

Q16 c13 b16 c19 npn
R6 c13 b16 4.5k
R7 b16 c19 7.5k

Q15 c19 c4 e15 npn
Q22 c4 e19 vss npn
Q19 c19 e15 e19 npn

R8 e15 vss 50k
R9 e19 vss 50


Q14 vcc c13 e14 npn
Q17 c13 e14 output npn
R10 e14 output 25

Q20 vss c19 e20 pnp
R11 e20 output 50

.ends
