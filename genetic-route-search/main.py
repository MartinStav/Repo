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

def initial_population(populationSize, numOfCities):
    population = []
    for _ in range(populationSize):
        population.append(random.sample(range(numOfCities), numOfCities))
    return population

def calculate_distance(route, cities):
    dist = 0
    for i in range(len(route)):
        fromCity = cities[route[i]]
        toCity = cities[route[(i + 1) % len(route)]]
        dist += math.sqrt((fromCity[0] - toCity[0]) ** 2 + (fromCity[1] - toCity[1]) ** 2)
    return dist

def calculate_fitness(population, cities):
    fitness = []
    for route in population:
        fitness.append(10000 / max(calculate_distance(route, cities), 1e-12))
    return fitness

def crossover(parent1, parent2):
    size = len(parent1)
    p1 = [None] * size

    start = random.randint(0, size - 1)
    p1[start:] = parent1[start:]

    p2_pos = 0
    for i in range(size):
        if parent2[i] not in p1:
            while p1[p2_pos] is not None:
                p2_pos += 1
            p1[p2_pos] = parent2[i]
    return p1


def mutation(route):
    new_route = route[:]
    i1, i2 = sorted(random.sample(range(len(route)), 2))
    new_route[i1:i2 + 1] = reversed(new_route[i1:i2 + 1])
    return new_route

def tournament_selection(population, fitness):
    tournament = random.sample(range(len(population)), 3)
    winner = population[tournament[0]]
    for i in tournament:
        if fitness[i] > fitness[population.index(winner)]:
            winner = population[i]
    return winner

def roulette_selection(population, fitness):
    totalFitness = sum(fitness)
    probability = [f / totalFitness for f in fitness]
    return population[np.random.choice(range(len(population)), p=probability)]

def evolve(population, cities, mutationRate, selection):
    newPopulation = []
    fitness = calculate_fitness(population, cities)

    bestRoute = population[fitness.index(max(fitness))]
    newPopulation.append(bestRoute)

    for i in range(len(population) - 1):
        if(selection == 'tournament'):
            parent1 = tournament_selection(population, fitness)
            parent2 = tournament_selection(population, fitness)
        elif(selection == 'roulette'):
            parent1 = roulette_selection(population, fitness)
            parent2 = roulette_selection(population, fitness)

        if random.random() < 0.5:
            child = crossover(parent1, parent2)
        else:
            child = parent1[:]

        if random.random() < mutationRate:
            child = mutation(child)

        newPopulation.append(child)
    return newPopulation

def algorithm(cities, populationSize, generations, mutationRate, selection):
    population = initial_population(populationSize, len(cities))
    FitnessHistory = []

    for generation in range(generations):
        population = evolve(population, cities, mutationRate, selection)
        fitness = calculate_fitness(population, cities)
        bestFitness = max(fitness)
        FitnessHistory.append(bestFitness)

        if generation % 100 == 0:
            print(f"Generácia {generation}, Najlepšia fitness: {bestFitness}")

    return FitnessHistory, population[fitness.index(max(fitness))]

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Genetic travelling-salesperson demo")
    parser.add_argument("--seed", type=int, default=42)
    parser.add_argument("--cities", type=int, default=25)
    parser.add_argument("--generations", type=int, default=200)
    parser.add_argument("--population", type=int, default=30)
    parser.add_argument("--no-plots", action="store_true")
    args = parser.parse_args()
    if args.cities < 3 or args.population < 3 or args.generations < 1:
        parser.error("cities/population must be >= 3; generations must be >= 1")
    random.seed(args.seed)
    np.random.seed(args.seed)
    cities = generate_cities(args.cities)
    for selection in ("tournament", "roulette"):
        random.seed(args.seed)
        np.random.seed(args.seed)
        history, route = algorithm(cities, args.population, args.generations, 0.1, selection)
        print(f"{selection}: distance={calculate_distance(route, cities):.2f}")
        if not args.no_plots:
            plt.figure()
            plt.plot(history, label=selection)
            plt.xlabel("Generation")
            plt.ylabel("Fitness (10000 / route length)")
            plt.legend()
            plt.show()
            plot_route(cities, route, selection)
