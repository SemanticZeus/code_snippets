#define FR 1
#define BL 2
#define FL 3
#define BR 0


int get_speed(int motor);

void update_pid(int baseThrottle, float roll, float pitch);