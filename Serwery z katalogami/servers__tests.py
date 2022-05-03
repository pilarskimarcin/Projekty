# Autorzy:
# Joanna Nużka, 400561
# Kacper Szmajdel, 402019
# Marcin Pilarski, 401802

import unittest
from collections import Counter

from servers import ListServer, Product, Client, MapServer, TooManyProductsFoundError

server_types = (ListServer, MapServer)
products = [
    Product('a1', 1), Product('a10', 2), Product('a1000', 5), Product('a100', 4), Product('a20', 3),
    Product('ab200', 10), Product('ab20', 8), Product('ab300', 11), Product('ab1000', 12), Product('ab10', 7),
    Product('ab2000', 13), Product('ab1', 6), Product('ab100', 9), Product('abc100', 19), Product('abc10', 16),
    Product('abc1000', 22), Product('abc300', 21), Product('abc1', 14), Product('abc30', 18), Product('abc2', 15),
    Product('abc20', 17), Product('abc200', 20)
]


class ProductTest(unittest.TestCase):
    def test_wrong_product_name_only_letters(self):
        with self.assertRaises(ValueError):
            product = Product("ABc", 5.8)

    def test_wrong_product_name_only_numbers(self):
        with self.assertRaises(ValueError):
            product = Product("123", 3)

    def test_wrong_product_name_wrong_order(self):
        with self.assertRaises(ValueError):
            product = Product("23Ab6h", 4)

    def test_wrong_product_name_special_characters(self):
        with self.assertRaises(ValueError):
            product = Product("ab#2", 6)

    def test_good_product_name_small_letters(self):
        name = "ab23"
        price = 7
        product = Product(name, price)
        self.assertEqual(product.name, name)
        self.assertEqual(product.price, price)

    def test_good_product_name_big_letters(self):
        name = "AB23"
        price = 7
        product = Product(name, price)
        self.assertEqual(product.name, name)
        self.assertEqual(product.price, price)

    def test_good_product_name_mixed_letters(self):
        name = "AbC1"
        price = 6.5
        product = Product(name, price)
        self.assertEqual(product.name, name)
        self.assertEqual(product.price, price)


class ServerTest(unittest.TestCase):
    def test_get_entries_returns_proper_entries_no_argument(self):
        for server_type in server_types:
            server = server_type(products)
            entries = server.get_entries()
            self.assertEqual(Counter([products[num] for num in [1, 3, 4]]), Counter(entries))

    def test_get_entries_returns_proper_entries_n_1(self):
        for server_type in server_types:
            server = server_type(products)
            entries = server.get_entries(1)
            self.assertEqual(Counter([products[num] for num in [1, 3, 4]]), Counter(entries))

    def test_get_entries_returns_proper_entries_n_2(self):
        for server_type in server_types:
            server = server_type(products)
            entries = server.get_entries(2)
            self.assertEqual(Counter([products[num] for num in [5, 6, 7, 9, 12]]), Counter(entries))

    def test_get_entries_too_many_items(self):
        for server_type in server_types:
            server = server_type(products)
            with self.assertRaises(TooManyProductsFoundError):
                entries = server.get_entries(3)

    def test_get_entries_no_entries_found(self):
        for server_type in server_types:
            server = server_type(products)
            entries = server.get_entries(4)
            self.assertEqual([], entries)

    def test_get_entries_returns_sorted_list(self):
        for server_type in server_types:
            server = server_type(products)
            entries = server.get_entries(1)
            self.assertEqual([products[num] for num in [1, 4, 3]], entries)


class ClientTest(unittest.TestCase):
    def test_total_price_None_as_argument(self):
        for server_type in server_types:
            server = server_type(products)
            client = Client(server)
            self.assertEqual(None, client.get_total_price(None))

    def test_total_price_for_normal_execution_n_1(self):
        for server_type in server_types:
            server = server_type(products)
            client = Client(server)
            self.assertEqual(9, client.get_total_price(1))

    def test_total_price_for_normal_execution_n_2(self):
        for server_type in server_types:
            server = server_type(products)
            client = Client(server)
            self.assertEqual(45, client.get_total_price(2))

    def test_total_price_too_many_items(self):
        for server_type in server_types:
            server = server_type(products)
            client = Client(server)
            self.assertEqual(None, client.get_total_price(3))

    def test_total_price_no_entries_found(self):
        for server_type in server_types:
            server = server_type(products)
            client = Client(server)
            self.assertEqual(None, client.get_total_price(4))


if __name__ == '__main__':
    unittest.main()
