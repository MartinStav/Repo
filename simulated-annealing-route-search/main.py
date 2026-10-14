import random
import math
import numpy as np
import matplotlib.pyplot as plt

def plot_route(cities, route, title):
    plt.figure()
    plt.scatter(cities[:, 0], cities[:, 1], c='red')
    for i in range(len(route)):
        start_city = cities[route[i]]
        end_city = cities[route[(i + 1) % len(route)]]
        plt.plot([start_city[0], end_city[0]], [start_city[1], end_city[1]], 'blue')
    plt.title(title)
    plt.show()

def generate_cities(numOfCities):
    return np.random.randint(0, 201, size=(numOfCities, 2))

def calculate_distance(route, cities):
    dist = 0
    for i in range(len(route)):
        fromCity = cities[route[i]]
        toCity = cities[route[(i + 1) % len(route)]]
        dist += math.sqrt((fromCity[0] - toCity[0]) ** 2 + (fromCity[1] - toCity[1]) ** 2)
    return dist

def mutation(route):
    new_route = route[:]
    i1, i2 = sorted(random.sample(range(len(route)), 2))
    new_route[i1:i2 + 1] = reversed(new_route[i1:i2 + 1])
    return new_route

def algorithm(cities, route, temp, minTemp, maxIter, coolingRate):
    distanceHistory = []
    distance = calculate_distance(route, cities)
    bestRoute = route[:]
    bestDistance = distance
    iteration = 0

    while temp > minTemp and iteration < maxIter:
        new_route = mutation(route)
        new_distance = calculate_distance(new_route, cities)

        if new_distance <= distance or random.random() < math.exp((distance - new_distance) / temp):
            route = new_route
            distance = new_distance
        iteration += 1

        if distance < bestDistance:
            bestRoute = route[:]
            bestDistance = distance

        distanceHistory.append(bestDistance)
        temp *= coolingRate

    return distanceHistory, bestRoute

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Simulated annealing route search")
    parser.add_argument("--seed", type=int, default=42)
    parser.add_argument("--cities", type=int, default=25)
    parser.add_argument("--iterations", type=int, default=3000)
    parser.add_argument("--no-plots", action="store_true")
    args = parser.parse_args()
    if args.cities < 3 or args.iterations < 1:
        parser.error("cities must be >= 3; iterations must be >= 1")
    random.seed(args.seed)
    np.random.seed(args.seed)
    cities = generate_cities(args.cities)
    route = random.sample(range(args.cities), args.cities)
    history, best = algorithm(cities, route, 1000, 0.1, args.iterations, 0.999)
    print(f"distance={calculate_distance(best, cities):.2f}")
    if not args.no_plots:
        plt.plot(history)
        plt.title("Best route length during simulated annealing")
        plt.xlabel("Iteration")
        plt.ylabel("Route length (coordinate units)")
        plt.show()
        plot_route(cities, best, "Simulated annealing")
