from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


def run(name, data, *args):
    return subprocess.run([str(ROOT / 'build' / name), *map(str, args)], input=data,
                          text=True, capture_output=True, timeout=10, check=True).stdout.strip()


class ProgramTests(unittest.TestCase):
    def test_avl_insert_range_delete(self):
        data = (ROOT / 'avl-tree/examples/input.txt').read_text()
        self.assertEqual(run('avl_tree', data), '5 Demo Beta\n10 Demo Alpha\n20 Demo Gamma')

    def test_hash_update_delete(self):
        data = (ROOT / 'hash-table/examples/input.txt').read_text()
        self.assertEqual(run('hash_table', data), '10,00\n12,50\nsearch failed')

    def test_hash_rejects_negative_balance_update(self):
        self.assertEqual(run('hash_table', 'i Demo Alpha 2000 10,00\nu Demo Alpha 2000 -20,00\ns Demo Alpha 2000\n'), 'update failed\n10,00')

    def test_dijkstra_selects_shorter_indirect_route(self):
        data = (ROOT / 'shortest-path/examples/input.txt').read_text()
        self.assertEqual(run('shortest_path', data), '6: [0, 1, 2, 3]')

    def test_dijkstra_unreachable_vertex(self):
        self.assertEqual(run('shortest_path', '3 1\n(0, 1, 1)\ns 0 2\n'), 'shortest 0 2 failed')

    def test_prim_tree(self):
        data = (ROOT / 'minimum-spanning-tree/examples/input.txt').read_text()
        self.assertEqual(run('minimum_spanning_tree', data), '6: [(0, 1), (1, 2), (2, 3)]')

    def test_prim_disconnected_component(self):
        self.assertEqual(run('minimum_spanning_tree', '4\n(0, 1, 5)\ns 0\n'), '5: [(0, 1)]')

    def test_scheduling_uses_earlier_free_slot(self):
        self.assertIn('Total profit: 199', run('job_scheduling', '3\n2 100\n2 99\n1 1\n'))

    def test_scheduling_skips_unprofitable_and_invalid_tasks(self):
        self.assertIn('Total profit: 10', run('job_scheduling', '3\n-1 100\n1 -5\n1 10\n'))

    def test_optimal_merge_cost(self):
        self.assertIn('Total cost: 50', run('optimal_file_merge', '4\n16 8 2 4\n'))

    def test_inventory_sale_and_storage_cost(self):
        with tempfile.TemporaryDirectory() as d:
            p = Path(d)
            (p / 'items').write_text('1 10 2\n')
            (p / 'stock').write_text('1 5\n')
            self.assertEqual(run('inventory_manager', 'sell 1 2\n', p / 'items', p / 'stock', p / 'out', 100), '114')
            self.assertEqual((p / 'out').read_text(), '1 3\n')

    def test_photo_organizer_keeps_duplicate_when_requested(self):
        with tempfile.TemporaryDirectory() as d:
            p = Path(d)
            source = p / 'input'
            source.mkdir()
            for name in ('a.png', 'b.png'):
                (source / name).write_bytes(b'synthetic-test-content')
            result = run('photo_organizer', '', source, p / 'output', '--keep-duplicates')
            self.assertIn('Photos organized: 1', result)
            self.assertIn('Duplicates moved: 1', result)
            self.assertEqual(len(list((p / 'output').rglob('*.png'))), 2)


if __name__ == '__main__':
    unittest.main()
