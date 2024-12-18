import tensorflow as tf
from tensorflow.keras.datasets import mnist
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Flatten
from tensorflow.keras.utils import to_categorical
import time

# Load and preprocess the MNIST dataset
(x_train, y_train), (x_test, y_test) = mnist.load_data()
x_train, x_test = x_train / 255.0, x_test / 255.0
y_train, y_test = to_categorical(y_train), to_categorical(y_test)

# Build a simple neural network model
def build_model():
    model = Sequential([
        Flatten(input_shape=(28, 28)),
        Dense(128, activation='relu'),
        Dense(10, activation='softmax')
    ])
    model.compile(optimizer='adam', loss='categorical_crossentropy', metrics=['accuracy'])
    return model

# Function to train the model on a specific device
def train_on_device(device):
    with tf.device(device):
        model = build_model()
        start_time = time.time()
        model.fit(x_train, y_train, epochs=100, batch_size=128, validation_data=(x_test, y_test), verbose=0)
        training_time = time.time() - start_time
    return training_time

# Train on CPU
cpu_time = train_on_device('/device:CPU:0')
print(f"Training time on CPU: {cpu_time:.2f} seconds")

# Train on GPU
gpu_time = train_on_device('/device:GPU:0')
print(f"Training time on GPU: {gpu_time:.2f} seconds")

