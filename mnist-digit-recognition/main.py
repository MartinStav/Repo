import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader
from torchvision import datasets, transforms
import matplotlib.pyplot as plt

class MNISTModel(nn.Module):
    def __init__(self):
        super(MNISTModel, self).__init__()
        self.fc1 = nn.Linear(28*28, 256)
        self.relu = nn.ReLU()
        self.fc2 = nn.Linear(256, 128)
        self.fc3 = nn.Linear(128, 10)

    def forward(self, x):
        x = x.view(-1, 28*28)
        x = self.relu(self.fc1(x))
        x = self.relu(self.fc2(x))
        x = self.fc3(x)
        return x
# chatGPT zaciatok
def visualize_predictions(model, test_loader):
    model.eval()
    all_labels = []
    all_predictions = []

    with torch.no_grad():
        for images, labels in test_loader:
            outputs = model(images)
            _, predicted = torch.max(outputs.data, 1)
            all_labels.extend(labels.tolist())
            all_predictions.extend(predicted.tolist())

    all_labels = torch.tensor(all_labels)
    all_predictions = torch.tensor(all_predictions)

    fig = plt.figure(figsize=(20, 1.3))

    for digit in range(10):
        ax = fig.add_subplot(1, 10, digit+1)

        is_digit = (all_labels == digit)
        correct_predictions = ((all_predictions == digit) & is_digit).sum().item()
        total_actual = is_digit.sum().item()
        false_positives = ((all_predictions == digit) & (all_labels != digit)).sum().item()

        ax.text(0.5, 0.8, f'{digit}',
                ha='center', va='center',
                fontsize=24, fontweight='bold')

        stats_text = f'Correct: {correct_predictions}/{total_actual}\nFalse: {false_positives}'
        ax.text(0.5, 0.2, stats_text,
                ha='center', va='center',
                fontsize=10)
        ax.axis('off')

    plt.tight_layout()
    plt.show()
# chatGPT koniec

def train_model(model, optimizer, criterion, train_loader, num_epochs=10):
    model.train()
    for epoch in range(num_epochs):
        running_loss = 0.0
        for images, labels in train_loader:
            optimizer.zero_grad()
            outputs = model(images)
            loss = criterion(outputs, labels)
            loss.backward()
            optimizer.step()
            running_loss += loss.item()
        print(f'Epoch [{epoch+1}/{num_epochs}], Loss: {running_loss/len(train_loader):.4f}')

def test_model(model, test_loader):
    model.eval()
    correct = 0
    total = 0
    with torch.no_grad():
        for images, labels in test_loader:
            outputs = model(images)
            _, predicted = torch.max(outputs.data, 1)
            total += labels.size(0)
            correct += (predicted == labels).sum().item()
    print(f'Test Accuracy: {100 * correct / total:.2f}%')

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="MNIST optimizer comparison")
    parser.add_argument("--seed", type=int, default=42)
    parser.add_argument("--epochs", type=int, default=1)
    parser.add_argument("--no-plots", action="store_true")
    args = parser.parse_args()
    if args.epochs < 1:
        parser.error("epochs must be >= 1")
    transform = transforms.Compose([transforms.ToTensor(), transforms.Normalize((0.5,), (0.5,))])
    train_dataset = datasets.MNIST(root="./data", train=True, transform=transform, download=True)
    test_dataset = datasets.MNIST(root="./data", train=False, transform=transform, download=True)
    test_loader = DataLoader(test_dataset, batch_size=64, shuffle=False)
    for name in ("SGD", "Momentum", "Adam"):
        torch.manual_seed(args.seed)
        model = MNISTModel()
        train_loader = DataLoader(train_dataset, batch_size=64, shuffle=True,
                                  generator=torch.Generator().manual_seed(args.seed))
        if name == "SGD":
            optimizer = optim.SGD(model.parameters(), lr=0.1)
        elif name == "Momentum":
            optimizer = optim.SGD(model.parameters(), lr=0.01, momentum=0.9)
        else:
            optimizer = optim.Adam(model.parameters(), lr=0.001)
        print(f"Training with {name}:")
        train_model(model, optimizer, nn.CrossEntropyLoss(), train_loader, args.epochs)
        test_model(model, test_loader)
        if not args.no_plots:
            visualize_predictions(model, test_loader)
