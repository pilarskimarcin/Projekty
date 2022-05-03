# Autorzy:
# Joanna Nużka, 400561
# Kacper Szmajdel, 402019
# Marcin Pilarski, 401802
#
# !/usr/bin/python
# -*- coding: utf-8 -*-
import re
from typing import Optional, List, Dict, TypeVar
from abc import ABC, abstractmethod
from operator import attrgetter
from copy import deepcopy


class Product:
    def __init__(self, name: str, price: float):
        if re.fullmatch(r'[a-zA-Z]+[0-9]+', name):
            self.name = name
            self.price = price
        else:
            raise ValueError

    def __eq__(self, other):
        return self.name == other.name and self.price == other.price

    def __hash__(self):
        return hash((self.name, self.price))


class TooManyProductsFoundError(Exception):
    # Reprezentuje wyjątek związany ze znalezieniem zbyt dużej liczby produktów.
    pass


class Server(ABC):
    n_max_returned_entries: int = 5

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    def get_entries(self, n_letters: int = 1) -> List[Product]:
        result = []
        for product in self.get_products():
            if re.fullmatch('^[a-zA-Z]{{{n}}}\\d{{2,3}}$'.format(n=n_letters), product.name):
                result.append(product)
        if len(result) > self.n_max_returned_entries:
            raise TooManyProductsFoundError
        result.sort(key=attrgetter('price'))
        return result

    @abstractmethod
    def get_products(self) -> List[Product]:
        raise NotImplementedError


class ListServer(Server):
    def __init__(self, products_list: List[Product], *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.products = deepcopy(products_list)

    def get_products(self) -> List[Product]:
        return self.products


class MapServer(Server):
    def __init__(self, products_list: List[Product], *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.products = {}
        for product in products_list:
            temp = deepcopy(product)
            self.products[temp.name] = temp

    def get_products(self) -> List[Product]:
        return list(self.products.values())


ServerType = TypeVar('ServerType', bound=Server)


class Client:
    def __init__(self, server: ServerType):
        self.server = server

    def get_total_price(self, n_letters: Optional[int]) -> Optional[float]:
        try:
            products_returned = self.server.get_entries(n_letters)
        except TooManyProductsFoundError:
            return None
        if products_returned:
            return float(sum([product.price for product in products_returned]))
        else:
            return None
