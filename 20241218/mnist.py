import tensorflow as tf
from tensorflow.keras import layers, models

def load_and_preprocess_mnist():
    (x_train, y_train), (x_test, y_test) = tf.keras.datasets.mnist.load_data()
    x_train, x_test = x_train / 255.0, x_test / 255.0
    x_train = x_train[..., tf.newaxis]
    x_test = x_test[..., tf.newaxis]
    train_ds = tf.data.Dataset.from_tensor_slices((x_train, y_train))
    test_ds = tf.data.Dataset.from_tensor_slices((x_test, y_test))
    train_ds = train_ds.shuffle(10000).batch(64).prefetch(tf.data.AUTOTUNE)
    test_ds = test_ds.batch(64).prefetch(tf.data.AUTOTUNE)

    return train_ds, test_ds

def create_model():
    model = models.Sequential([
        layers.Conv2D(32, kernel_size=(3, 3), activation='relu', input_shape=(28, 28, 1)),
        layers.MaxPooling2D(pool_size=(2, 2)),
        layers.Conv2D(64, kernel_size=(3, 3), activation='relu'),
        layers.MaxPooling2D(pool_size=(2, 2)),
        layers.Flatten(),
        layers.Dense(128, activation='relu'),
        layers.Dense(10, activation='softmax')
    ])
    return model

if __name__ == "__main__":
    train_ds, test_ds = load_and_preprocess_mnist()
    with tf.device('/CPU:0'):
        model = create_model()
        model.compile(optimizer='adam',
                  loss='sparse_categorical_crossentropy',
                  metrics=['accuracy'])
        tf.debugging.set_log_device_placement(True)
        model.fit(train_ds, epochs=5, validation_data=test_ds)
        test_loss, test_acc = model.evaluate(test_ds)

    print(f"Test accuracy: {test_acc:.4f}")

