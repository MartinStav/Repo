import numpy as np
import random
import matplotlib.pyplot as plt
import math

# Set a random seed
seed_value = 42
random.seed(seed_value)
np.random.seed(seed_value)

# Generate initial points
def generate_points(n, xylim):
    points = set()
    while len(points) < n:
        points.add((random.randint(*xylim), random.randint(*xylim)))
    return np.array(list(points))

# Generate additional points
def additional_points(init, n, xyoffset):
    points = init.tolist()
    for _ in range(n):
        rand_point = random.choice(points)
        x_offset = random.randint(*xyoffset)
        y_offset = random.randint(*xyoffset)
        new_point = (
            np.clip(rand_point[0] + x_offset, -5000, 5000),
            np.clip(rand_point[1] + y_offset, -5000, 5000)
        )
        points.append(new_point)
    return np.array(points)

def calculate_average_distances(points, labels, centroids):
    average_distances = {}
    for label in labels:
        cluster_points = points[labels == label]
        representative_point = centroids[int(label)]
        distances = np.linalg.norm(cluster_points - representative_point, axis=1)
        average_distance = np.mean(distances)
        average_distances[label] = average_distance
    return average_distances

def get_distance(points):
    return np.linalg.norm(points[:, np.newaxis] - points, axis=2)

def visualize(points, labels, centroids):
    plt.figure(figsize=(8, 8))

    unique_labels = np.unique(labels)
    for label in unique_labels:
        cluster_points = points[labels == label]
        plt.scatter(cluster_points[:, 0], cluster_points[:, 1], s=10, label='Cluster {}'.format(label))
    plt.scatter(centroids[:, 0], centroids[:, 1], s=50, c='red', marker='X', label='Representatives')
    plt.xlabel('X (coordinate units)')
    plt.ylabel('Y (coordinate units)')

    plt.title('Clustering')
    plt.legend()
    plt.show()

def find_closest_clusters(distance_matrix):
    min_dist = float('inf')
    closest_pair = (None, None)
    n = len(distance_matrix)
    for i in range(n):
        for j in range(i + 1, n):
            if distance_matrix[i, j] < min_dist:
                min_dist = distance_matrix[i, j]
                closest_pair = (i, j)
    return closest_pair

def calculate_centroid(points):
    return np.mean(points, axis=0)

def update_centroids(clusters, points):
    return np.array([calculate_centroid(points[cluster]) for cluster in clusters])

def calculate_medoid(points):
    centroid = calculate_centroid(points)
    distance_matrix = np.linalg.norm(points - centroid, axis=1)
    medoid_index = np.argmin(distance_matrix)
    return points[medoid_index]

def update_medoids(clusters, points):
    return np.array([calculate_medoid(points[cluster]) for cluster in clusters])

# Main clustering function
def clustering(points, cluster_num, method):
    n = len(points)
    if not 1 <= cluster_num <= n:
        raise ValueError("cluster_num must be between 1 and the number of points")
    if method not in {"centroid", "medoid"}:
        raise ValueError("method must be centroid or medoid")
    clusters = [[i] for i in range(n)]
    distance_matrix = get_distance(points)

    if method == 'centroid':
        centroids = update_centroids(clusters, points)
    else:
        centroids = update_medoids(clusters, points)

    while len(clusters) > cluster_num:
        i, j = find_closest_clusters(distance_matrix)

        clusters[i].extend(clusters[j])
        clusters.pop(j)

        # Recalculate centroids/medoids for updated clusters
        if method == 'centroid':
            centroids = update_centroids(clusters, points)
        else:
            centroids = update_medoids(clusters, points)

        distance_matrix = np.zeros((len(clusters), len(clusters)))
        for k in range(len(clusters)):
            for m in range(k + 1, len(clusters)):
                distance_matrix[k, m] = distance_matrix[m, k] = math.sqrt(sum((centroids[k][i] - centroids[m][i])**2 for i in range(len(centroids[k]))))

    labels = np.zeros(len(points), dtype=int)
    for i, cluster in enumerate(clusters):
        for point in cluster:
            labels[point] = i

    return labels, centroids

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="Small hierarchical clustering demo")
    parser.add_argument("--seed", type=int, default=42)
    parser.add_argument("--points", type=int, default=100)
    parser.add_argument("--clusters", type=int, default=5)
    parser.add_argument("--no-plots", action="store_true")
    args = parser.parse_args()
    if args.points < 2 or not 1 <= args.clusters <= args.points:
        parser.error("points must be >= 2; clusters must be between 1 and points")
    random.seed(args.seed)
    np.random.seed(args.seed)
    initial_count = min(20, args.points)
    points = additional_points(generate_points(initial_count, (-5000, 5000)), args.points - initial_count, (-100, 100))
    for method in ("centroid", "medoid"):
        labels, representatives = clustering(points, args.clusters, method)
        print(method, calculate_average_distances(points, labels, representatives))
        if not args.no_plots:
            visualize(points, labels, representatives)
