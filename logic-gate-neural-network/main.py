import numpy as np
import matplotlib.pyplot as plt

class LinearLayer:
    def __init__(self, input_size, output_size):
        limit = np.sqrt(2.0 / (input_size + output_size))
        self.weights = np.random.uniform(-limit, limit, (input_size, output_size))
        self.biases = np.zeros((1, output_size))
        self.weight_momentum = np.zeros_like(self.weights)
        self.bias_momentum = np.zeros_like(self.biases)

    def forward(self, x):
        self.input = x
        self.output = np.dot(x, self.weights) + self.biases
        return self.output

    def backward(self, grad_output, learning_rate, momentum=0.9):
        grad_input = np.dot(grad_output, self.weights.T)
        grad_weights = np.dot(self.input.T, grad_output)
        grad_biases = np.sum(grad_output, axis=0, keepdims=True)

        self.weight_momentum = momentum * self.weight_momentum - learning_rate * grad_weights
        self.bias_momentum = momentum * self.bias_momentum - learning_rate * grad_biases

        self.weights += self.weight_momentum
        self.biases += self.bias_momentum
        return grad_input

class Sigmoid:
    def forward(self, x):
        self.output = 1 / (1 + np.exp(-x))
        return self.output

    def backward(self, grad_output):
        return grad_output * (self.output * (1 - self.output))

class Tanh:
    def forward(self, x):
        self.output = np.tanh(x)
        return self.output

    def backward(self, grad_output):
        return grad_output * (1 - self.output ** 2)

class ReLU:
    def forward(self, x):
        self.input = x
        self.output = np.maximum(0, x)
        return self.output

    def backward(self, grad_output):
        return grad_output * (self.input > 0)

class NeuralNetwork:
    def __init__(self, layers):
        self.layers = layers

    def forward(self, x):
        for layer in self.layers:
            x = layer.forward(x)
        return x

    def backward(self, grad_output, learning_rate, momentum=0.9):
        for layer in reversed(self.layers):
            if isinstance(layer, LinearLayer):
                grad_output = layer.backward(grad_output, learning_rate, momentum)
            else:
                grad_output = layer.backward(grad_output)
        return grad_output

def mse_loss(predictions, targets):
    return np.mean((predictions - targets) ** 2)

def mse_loss_gradient(predictions, targets):
    return 2 * (predictions - targets) / targets.size

# chatGPT zaciatok
def plot_training_history(losses_dict, config_name, lr):
    plt.figure(figsize=(10, 6))

    for problem, losses in losses_dict.items():
        plt.plot(losses, label=problem)

    plt.title(f"{config_name}\nLearning Rate = {lr}")
    plt.xlabel("Epoch")
    plt.ylabel("Loss")
    plt.grid(True)
    plt.legend()
    plt.show()

# chatGPT koniec
def build_model(hidden_layers, seed=42):
    np.random.seed(seed)
    layers = [LinearLayer(2, 4), Tanh()]
    if hidden_layers == 2:
        layers.extend([LinearLayer(4, 4), Tanh()])
    layers.extend([LinearLayer(4, 1), Sigmoid()])
    return NeuralNetwork(layers)


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="NumPy XOR/AND/OR network comparison")
    parser.add_argument("--seed", type=int, default=42)
    parser.add_argument("--epochs", type=int, default=500)
    parser.add_argument("--no-plots", action="store_true")
    args = parser.parse_args()
    if args.epochs < 1:
        parser.error("epochs must be >= 1")
    X = np.array([[0, 0], [0, 1], [1, 0], [1, 1]])
    targets = {"XOR": np.array([[0], [1], [1], [0]]),
               "AND": np.array([[0], [0], [0], [1]]),
               "OR": np.array([[0], [1], [1], [1]])}
    for hidden_layers in (1, 2):
        for lr in (0.01, 0.1):
            for momentum in (0.0, 0.9):
                histories = {}
                for name, Y in targets.items():
                    model = build_model(hidden_layers, args.seed)
                    losses = []
                    for _ in range(args.epochs):
                        predictions = model.forward(X)
                        losses.append(mse_loss(predictions, Y))
                        model.backward(mse_loss_gradient(predictions, Y), lr, momentum)
                    histories[name] = losses
                    print(f"layers={hidden_layers} lr={lr} momentum={momentum} {name}: loss={losses[-1]:.6f}")
                if not args.no_plots:
                    plot_training_history(histories, f"{hidden_layers} hidden layers; momentum={momentum}", lr)
