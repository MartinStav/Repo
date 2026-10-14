import importlib.util
from pathlib import Path
import random
import unittest
from unittest.mock import Mock, patch

import numpy as np

ROOT = Path(__file__).resolve().parents[1]


def load(folder, filename='main.py'):
    spec = importlib.util.spec_from_file_location(folder.replace('-', '_'), ROOT / folder / filename)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


class RouteTests(unittest.TestCase):
    def test_genetic_routes_remain_permutations(self):
        m = load('genetic-route-search')
        random.seed(42)
        cities = np.array([[0, 0], [0, 1], [1, 0], [1, 1]])
        population = m.initial_population(10, 4)
        for mode in ('tournament', 'roulette'):
            for _ in range(10):
                population = m.evolve(population, cities, 0.8, mode)
                self.assertEqual(len(population), 10)
                for route in population:
                    self.assertEqual(sorted(route), [0, 1, 2, 3])

    def test_annealing_improves_crossed_route_and_keeps_best(self):
        m = load('simulated-annealing-route-search')
        random.seed(42)
        cities = np.array([[0, 0], [0, 1], [1, 0], [1, 1]])
        history, route = m.algorithm(cities, [0, 3, 1, 2], 10, 0.01, 300, 0.99)
        self.assertTrue(all(b <= a for a, b in zip(history, history[1:])))
        self.assertAlmostEqual(m.calculate_distance(route, cities), 4)
        self.assertLessEqual(len(history), 300)


class ClusteringTests(unittest.TestCase):
    def test_duplicate_points_can_merge(self):
        m = load('hierarchical-clustering')
        points = np.array([[0, 0], [0, 0], [10, 10], [10, 10]])
        for method in ('centroid', 'medoid'):
            labels, representatives = m.clustering(points, 2, method)
            self.assertEqual(labels[0], labels[1])
            self.assertEqual(labels[2], labels[3])
            self.assertNotEqual(labels[0], labels[2])
            self.assertEqual(representatives.shape, (2, 2))

    def test_invalid_cluster_count_is_rejected(self):
        m = load('hierarchical-clustering')
        with self.assertRaises(ValueError):
            m.clustering(np.zeros((2, 2)), 3, 'centroid')


class NetworkTests(unittest.TestCase):
    def test_fresh_models_are_reproducible_and_independent(self):
        m = load('logic-gate-neural-network')
        first = m.build_model(2, 42)
        second = m.build_model(2, 42)
        np.testing.assert_array_equal(first.layers[0].weights, second.layers[0].weights)
        first.layers[0].weights[:] = 0
        self.assertFalse(np.all(second.layers[0].weights == 0))

    def test_linear_backward_matches_finite_difference(self):
        m = load('logic-gate-neural-network')
        np.random.seed(42)
        layer = m.LinearLayer(2, 1)
        x = np.array([[0.2, 0.7], [0.6, -0.3]])
        y = np.array([[0.8], [0.1]])
        before = layer.weights.copy()
        eps = 1e-6
        numerical = np.zeros_like(before)
        for index in np.ndindex(before.shape):
            layer.weights[index] = before[index] + eps
            plus = m.mse_loss(layer.forward(x), y)
            layer.weights[index] = before[index] - eps
            minus = m.mse_loss(layer.forward(x), y)
            numerical[index] = (plus - minus) / (2 * eps)
            layer.weights[index] = before[index]
        predictions = layer.forward(x)
        layer.backward(m.mse_loss_gradient(predictions, y), 0.01, momentum=0)
        np.testing.assert_allclose((before - layer.weights) / 0.01, numerical, atol=1e-7)

    def test_xor_learning_reduces_loss(self):
        m = load('logic-gate-neural-network')
        model = m.build_model(1, 42)
        x = np.array([[0, 0], [0, 1], [1, 0], [1, 1]])
        y = np.array([[0], [1], [1], [0]])
        initial = m.mse_loss(model.forward(x), y)
        for _ in range(2000):
            predictions = model.forward(x)
            model.backward(m.mse_loss_gradient(predictions, y), 0.1, 0.9)
        self.assertLess(m.mse_loss(model.forward(x), y), initial / 3)


class ProtocolTests(unittest.TestCase):
    def test_heartbeats_continue_until_disconnected(self):
        m = load('udp-file-transfer', 'peer.py')
        peer = m.Peer.__new__(m.Peer)
        peer.isConnected = True
        peer.last_heartbeat = 0
        peer.heartbeat_count = 1
        peer.seqNum = 0
        peer.peer_address = ('localhost', 9000)
        peer.sock = Mock()
        count = 0

        def sent(*args):
            nonlocal count
            count += 1
            if count == 2:
                peer.isConnected = False

        peer.sock.sendto.side_effect = sent
        with patch.object(m.time, 'sleep'), patch.object(m.time, 'time', return_value=0):
            peer.send_heartbeat()
        self.assertEqual(count, 2)

    def test_udp_header_round_trip(self):
        m = load('udp-file-transfer', 'peer.py')
        peer = m.Peer.__new__(m.Peer)
        header = peer.create_header(6, 1, 4095, 12345, 300)
        self.assertEqual(len(header), 8)
        self.assertEqual(peer.read_header(header), (6, 1, 4095, 12345, 300))

    def test_crc_known_check_value(self):
        m = load('udp-file-transfer', 'peer.py')
        peer = m.Peer.__new__(m.Peer)
        self.assertEqual(peer.calculate_crc(b'123456789'), 0xAEE7)


if __name__ == '__main__':
    unittest.main()
