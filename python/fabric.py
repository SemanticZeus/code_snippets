import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Fabric parameters
width, height = 1.0, 1.0       # dimensions of the fabric
num_points = 20                # resolution of the grid
poisson_ratio = 0.3            # Poisson's ratio for the fabric
k = 1000                       # spring constant
dt = 0.01                      # time step for simulation
damping = 0.99                 # damping factor

# Generate the grid points
x = np.linspace(0, width, num_points)
y = np.linspace(0, height, num_points)
X, Y = np.meshgrid(x, y)
Z = np.zeros_like(X)           # Z position (initially flat)

# Apply stretching on two ends of the fabric
# Stretch horizontally (apply forces on left and right edges)
stretch_amount = 0.05

# Velocities grid to store movement between steps
Vx, Vy, Vz = np.zeros_like(X), np.zeros_like(Y), np.zeros_like(Z)

def apply_forces(Z, Vx, Vy, Vz):
    new_Z = Z.copy()
    new_Vx, new_Vy, new_Vz = Vx.copy(), Vy.copy(), Vz.copy()

    # Apply spring forces with Poisson effect
    for i in range(1, num_points - 1):
        for j in range(1, num_points - 1):
            # Horizontal neighbors (left and right)
            left = Z[i, j - 1]
            right = Z[i, j + 1]
            dx = right - left
            lateral_force_x = -k * dx

            # Vertical neighbors (up and down)
            up = Z[i - 1, j]
            down = Z[i + 1, j]
            dy = down - up
            lateral_force_y = -k * dy

            # Diagonal neighbors for Poisson's effect
            diag1 = Z[i - 1, j - 1]
            diag2 = Z[i + 1, j + 1]
            diag3 = Z[i - 1, j + 1]
            diag4 = Z[i + 1, j - 1]
            avg_diag = (diag1 + diag2 + diag3 + diag4) / 4

            # Poisson's effect: contraction perpendicular to the applied force
            poisson_effect = -poisson_ratio * k * (dx + dy - avg_diag)
            new_Vx[i, j] += lateral_force_x * dt + poisson_effect
            new_Vy[i, j] += lateral_force_y * dt + poisson_effect
            new_Vz[i, j] += poisson_effect * dt

    # Update positions and apply damping
    new_Z += new_Vz * dt
    new_Vx *= damping
    new_Vy *= damping
    new_Vz *= damping

    # Apply boundary stretching
    new_Z[0, :] += stretch_amount  # left edge
    new_Z[-1, :] -= stretch_amount # right edge

    return new_Z, new_Vx, new_Vy, new_Vz

# Run the simulation for several steps
num_iterations = 100
for _ in range(num_iterations):
    Z, Vx, Vy, Vz = apply_forces(Z, Vx, Vy, Vz)

# Plot the result
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.plot_surface(X, Y, Z, cmap="viridis")
ax.set_xlabel("X")
ax.set_ylabel("Y")
ax.set_zlabel("Z")
plt.title("2D Fabric Simulation with Poisson's Effect Using Trusses")
plt.show()

