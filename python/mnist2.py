import tensorflow as tf

mnist = tf.keras.datasets.mnist

(x_train, y_train), (x_test, y_test) = mnist.load_data()
x_train, x_test = x_train / 255.0, x_test / 255.0
# Enable mixed precision for faster computation on compatible GPUs
tf.keras.mixed_precision.set_global_policy('mixed_float16')


with tf.device('/device:CPU:0'):
    # Define the model
    model = tf.keras.models.Sequential([
        tf.keras.layers.Flatten(input_shape=(28, 28)),
        tf.keras.layers.Dense(128, activation='relu', dtype='float32'),  # Avoid mixed precision in dense layers
        tf.keras.layers.Dropout(0.2),
        tf.keras.layers.Dense(10, dtype='float32')  # Avoid mixed precision in the final layer
    ])

    # Convert logits to probabilities
    def get_predictions(x_train):
        predictions = model(x_train[:1], training=False)
        return tf.nn.softmax(predictions).numpy()

    # Define loss function
    loss_fn = tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True)

    # Compile model with GPU-optimized settings
    model.compile(optimizer=tf.keras.optimizers.SGD(learning_rate=0.01, momentum=0.9),  # SGD with momentum for stability
                  loss=loss_fn,
                  metrics=['accuracy'])

    # Prefetch data to keep the GPU busy
    batch_size = 256
    #x_train, y_train = tf.data.Dataset.from_tensor_slices((x_train, y_train)).batch(batch_size).prefetch(tf.data.AUTOTUNE)
    train_dataset = tf.data.Dataset.from_tensor_slices((x_train, y_train)).batch(batch_size).prefetch(tf.data.AUTOTUNE)

    # Train the model
    #model.fit(x_train, y_train, epochs=10)
    model.fit(train_dataset, epochs=10)
