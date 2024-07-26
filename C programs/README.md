# Hash Table Implementation in C

Implementation of a hash table in C, designed to store and manage a collection of records with string keys and double values. The hash table supports insertion, deletion, searching, and updating of records.

## Features

- **Insert**: Add a new record to the hash table.
- **Delete**: Remove a record from the hash table.
- **Search**: Retrieve a record from the hash table.
- **Update**: Modify the balance of an existing record in the hash table.

## Usage

The program supports the following commands:

- **i <name> <balance>**: Insert a new record with the specified name and balance.
- **s <name>**: Search for a record with the specified name and print its balance.
- **d <name>**: Delete the record with the specified name.
- **u <name> <new_balance>**: Update the balance of the record with the specified name.

# Binary Tree Implementation in C

Self-balancing binary tree (AVL tree) in C. The tree stores records of Person with integer IDs and associated data strings. The implementation supports insertion, deletion, and searching of records.

## Features

- **Insert**: Add a new record to the binary tree.
- **Delete**: Remove a record from the binary tree.
- **Search**: Retrieve a record from the binary tree by ID.

## Usage

The program supports the following commands:

- **i <id> <data>**: Insert a new record with the specified ID and data.
- **s <id> [toId]**: Search for a record with the specified ID. If toId is provided, search for all records with IDs in the range [id, toId].
- **d <id>**: Delete the record with the specified ID.